/**
  ******************************************************************************
  * @file    tcp_udp.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module OS Demo tcp_udp Application C File.
  *
  *          This file contains:
  *           -socket api function,how to creat socket,recv and send data
  * 
  ******************************************************************************
**/
#include "rw_app.h"

#ifdef  TCPS_TEST
#define RW_TCPS_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_tcpserver_task;          
#endif

#ifdef  TCPC_TEST 
#define RW_TCPC_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_tcpclient_task;  
#endif

#ifdef  UDPS_TEST
#define RW_UDPS_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_udpserver_task; 
#endif

#ifdef  UDPC_TEST
#define RW_UDPC_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_udpclient_task; 
#endif

int RAK_TcpClient(uint16_t destPort, int destIp)
{
    SOCKADDR_IN     destAddr;
    SOCKADDR_IN     LocalAddr;
    int             sockfd;
    int             ret;
    int             lPort;

    //filling the TCP client socket address
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons((uint16_t)destPort);
    destAddr.sin_addr = htonl((int)destIp);
    
    // creating a TCP socket
    sockfd = socket( AF_INET,SOCK_STREAM, 0);
    if (sockfd < 0 )
    {
      close(sockfd);
      return RW_ERR;
    };
    
    srand((int)get_stamp());
    /*port : 1024 - 32768 */
    lPort = (rand() % (32768-1024+1))+ 1024; 
    
    //filling the TCP client local port
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;
    
    // binding the TCP socket to the TCP server address
    ret = bind(sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret < RW_OK )
    {
      close(sockfd);
      return RW_ERR;
    }
    
    // connecting to TCP server
    ret = connect(sockfd, (SOCKADDR_IN *)&destAddr, sizeof(destAddr));
    if( ret < 0 )
    {
      close(sockfd);
      return RW_ERR;
    }
   
    return sockfd;
}

void RAK_TcpClient_EventHandle(void *p_arg)
{
       int      ret = 0;
static int      send_recvCnt = 0;
       int      temp_len;
       
    char* c_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);

    if(NULL == c_buffer){
	DPRINTF("RAK_TcpClient: Allocate buffer failed.\n");
	while(1);
    }  
    
    while(1)
    {      
reconnect: 
  
      if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
      {
        rw_sysSleep(100);
        goto reconnect;      
      }
      
      if (app_demo_ctx.tcp_cloud_sockfd == INVAILD_SOCK_FD)
      {
        if((ret =RAK_TcpClient(25001, 0xC0A80389)) >= 0)
        {
          app_demo_ctx.tcp_cloud_sockfd = ret;
          app_demo_ctx.tcp_cloud_status = STATUS_OK;
          DPRINTF("RAK_TcpClient sockfd = %u creat \n\r",app_demo_ctx.tcp_cloud_sockfd);
        }
        else
        {
          if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
          { 
            DPRINTF("RAK_TcpClient creat failed code=%d\n", ret);
            rw_sysSleep(100);
            goto reconnect;
          }
        }    
      }
      
      if (app_demo_ctx.tcp_cloud_status <= STATUS_INIT)
      {
         if(app_demo_ctx.tcp_cloud_status < STATUS_INIT)
         {
            close(app_demo_ctx.tcp_cloud_sockfd);
            app_demo_ctx.tcp_cloud_status = STATUS_INIT;
            app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
         }
         goto reconnect;
      }
     
      ret = recv(app_demo_ctx.tcp_cloud_sockfd, c_buffer, MAX_RECV_PACKET_LEN, 0);
      if (ret <= 0 )
      {
        if(ret ==RW_ERR_SOCKET_INVAILD){
          DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.tcp_cloud_sockfd);
          app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
        }
        
      }else
      {
  //      DPRINTF("recv packets on sockfd=%d  data_len=%d\n\r", app_demo_ctx.tcp_cloud_sockfd, ret );              
        send_recvCnt += ret;
      }
      
      if(send_recvCnt > 0)
      {
        temp_len = send_recvCnt >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:send_recvCnt;
        ret = send(app_demo_ctx.tcp_cloud_sockfd, c_buffer, temp_len, 0);
        if (ret <= 0 )
        {
  //        DPRINTF("send errorcode =%d\n\r",ret);
          if(ret ==RW_ERR_SOCKET_INVAILD)
          {
            app_demo_ctx.tcp_cloud_sockfd =INVAILD_SOCK_FD;
            send_recvCnt = 0;
          }                  
        }else
        {
          send_recvCnt -= ret;
        }
      }
    }
}

int RAK_TcpServer(uint16_t lPort)
{
    SOCKADDR_IN     LocalAddr;
    int             sockfd;
    int             ret;
    
    //filling the TCP server socket address
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;

    // creating a TCP socket
    sockfd = socket(AF_INET,SOCK_STREAM, 0);
    if (sockfd < RW_OK )
    {
      close(sockfd);
      return sockfd;
    }
    
    // binding the TCP socket to the TCP server address
    ret = bind(sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret < RW_OK )
    {
      close(sockfd);
      return ret;
    }

    // putting the socket for listening to the incoming TCP connection
    ret = listen(sockfd, 1);
    if (ret < RW_OK )
    {
      close(sockfd);
      return ret;
    }

    return sockfd;
}

int RAK_TcpSRecvClients(int sever_fd, RW_APP_CTX* app_ctx)
{
    SOCKADDR_IN     new_tcpinfo;
    int             info_size;
    int             sockfd;
    int             i= 0;
       
    if (select(sever_fd, 1)!= RW_OK)
    {
      return RW_ERR;
    }
  
    sockfd = accept(sever_fd, (SOCKADDR_IN *)&new_tcpinfo,  (socklen_t*)&info_size);   //the socket as non blocking
    if (sockfd > 0 )
    {
      DPRINTF("recv new sockfd=%d from ip=0x%x ,port=%d \n\r", sockfd, ntohl(new_tcpinfo.sin_addr),
                                                                      ntohs(new_tcpinfo.sin_port));
      for(i=0; i<ALLOW_MAX_NUMS; i++)
      {
        if(app_ctx->ltcps_clientinfos[i].tcpc_sockfd == -1)
        {
          app_ctx->ltcps_clientinfos[i].tcpc_sockfd =sockfd; 
          app_ctx->ltcps_clientinfos[i].tcpc_info.sin_addr = ntohl(new_tcpinfo.sin_addr);
          app_ctx->ltcps_clientinfos[i].tcpc_info.sin_port = ntohl(new_tcpinfo.sin_port);
          app_ctx->ltcps_clientinfos[i].tcpc_info.sin_family = ntohl(new_tcpinfo.sin_family);
          app_ctx->tcpc_num++;
          break;
        }
      }
      if(i == ALLOW_MAX_NUMS)
      {
        DPRINTF("recv too many  connections close it \n\r");
        close(sockfd);
      }             
    }

    return RW_OK;
}

void RAK_TcpServer_EventHandle(void *p_arg)
{
    int ret = 0,i=0;
    int send_templen =0;
    
    char* s_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);

    if(NULL == s_buffer){
	DPRINTF("RAK_TcpServer: Allocate buffer failed.\n");
	while(1);
    }
   
    while(1)
    {
server_loop:
  
      if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
      {
        rw_sysSleep(100);
        goto server_loop;      
      }
    
      if (app_demo_ctx.ltcps_sockfd == INVAILD_SOCK_FD)
      {
          if((ret =RAK_TcpServer(25000)) >= 0)
          {
             app_demo_ctx.ltcps_sockfd = ret;
             DPRINTF("RAK_TcpServer sockfd = %u creat \n",app_demo_ctx.ltcps_sockfd);
          }else{
             DPRINTF("RAK_TcpServer creat failed code=%d\n", ret);
             rw_sysSleep(100);
             goto server_loop;
          }
      }
      
      RAK_TcpSRecvClients(app_demo_ctx.ltcps_sockfd , &app_demo_ctx);
      
      // send and recv loopback testing
      if (app_demo_ctx.tcpc_num > 0)
      {
        for (i=0; i< ALLOW_MAX_NUMS; i++)
        {
          if (app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd ==INVAILD_SOCK_FD)
             continue;
          
            ret = recv(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, s_buffer, MAX_RECV_PACKET_LEN, 0);
            if (ret <= 0 )
            {
              if (ret ==RW_ERR_SOCKET_INVAILD){
                DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd);
                app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
                app_demo_ctx.tcpc_num --;
                app_demo_ctx.ltcps_clientinfos[i].loopback_count = 0;
                break;
              }
            }else{
  //            DPRINTF("recv packets on sockfd=%d  data_len=%d\n\r", app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, ret );                 
              app_demo_ctx.ltcps_clientinfos[i].loopback_count += ret;
            }
            
             if(app_demo_ctx.ltcps_clientinfos[i].loopback_count > 0)
              {
                  send_templen = app_demo_ctx.ltcps_clientinfos[i].loopback_count >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:app_demo_ctx.ltcps_clientinfos[i].loopback_count;
                  ret = send(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, s_buffer, send_templen, 0);
                  if (ret <= 0 )
                  {
  //                  DPRINTF("send errorcode =%d\n\r",ret);
                    if(ret ==RW_ERR_SOCKET_INVAILD)
                    {
                      app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
                      app_demo_ctx.tcpc_num --;
                      app_demo_ctx.ltcps_clientinfos[i].loopback_count = 0;
                      break;
                    }                  
                  }else
                  {
                    app_demo_ctx.ltcps_clientinfos[i].loopback_count  -=ret;
                    break;
                  }
              }
          }
      } 
    }        
}

int RAK_UdpServer(uint16_t lPort)
{
    SOCKADDR_IN     LocalAddr;
    int             sockfd;
    int             ret;
    
    //filling the UDP server socket address
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;

    // creating a UDP socket
    sockfd = socket(AF_INET,SOCK_DGRAM, 0);
    if (sockfd < RW_OK )
    {
      return sockfd;
    }
    
    // binding the UDP socket to the UDP server address
    ret = bind(sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret < RW_OK )
    {
      close(sockfd);
      return ret;
    }

    return sockfd;

}

void RAK_UdpServer_EventHandle(void *p_arg)
{
    SOCKADDR_IN     servAddr;
    socklen_t       addrlen;
    int             ret = 0;
    static int      send_recvCnt = 0;
    int             temp_len;
    
    char* s_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);

    if(NULL == s_buffer){
	DPRINTF("RAK_UdpServer: Allocate buffer failed.\n");
	while(1);
    }  
    
  while(1)
  {
server_loop: 
  
    if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
    {
      rw_sysSleep(100);
      goto server_loop;      
    }
    
    if (app_demo_ctx.ludps_sockfd == INVAILD_SOCK_FD)
    {
        if((ret =RAK_UdpServer(25002)) >= 0)
        {
           app_demo_ctx.ludps_sockfd = ret;
           DPRINTF("RAK_UdpServer sockfd = %u creat \n",app_demo_ctx.ludps_sockfd);
        }else{
           DPRINTF("RAK_UdpServer creat failed code =%d\n", ret);
           rw_sysSleep(100);
           goto server_loop;
        }
    }
    
    ret = recvfrom(app_demo_ctx.ludps_sockfd, s_buffer, MAX_RECV_PACKET_LEN, 0, (SOCKADDR_IN *)&servAddr,  &addrlen);
    if (ret <= 0 )
    {
      if(ret ==RW_ERR_SOCKET_INVAILD){
//        DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.ludps_sockfd);
        close(app_demo_ctx.ludps_sockfd);
        app_demo_ctx.ludps_sockfd = INVAILD_SOCK_FD; //close
      }     
    }else{
//      DPRINTF("recvfrom 0x%x:%d  packets on sockfd=%d  data_len=%d\n\r", ntohl(servAddr.sin_addr), ntohs(servAddr.sin_port), app_demo_ctx.ludps_sockfd, ret );         
      send_recvCnt += ret;
    }
    
    if(send_recvCnt > 0)
    {
      temp_len = send_recvCnt >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:send_recvCnt;
      ret = sendto(app_demo_ctx.ludps_sockfd, s_buffer, temp_len, 0, (SOCKADDR_IN *)&servAddr, sizeof(servAddr));
      if (ret <= 0 )
      {
//        DPRINTF("send errorcode =%d\n\r",ret);
        if(ret ==RW_ERR_SOCKET_INVAILD)
        {
          app_demo_ctx.ludps_sockfd =INVAILD_SOCK_FD;
          send_recvCnt = 0;
        }                  
      }else
      {
        send_recvCnt -= ret;
      }
    }
  }
}

int RAK_UdpClient(uint16_t lPort, uint16_t destPort, int destIp)
{
    SOCKADDR_IN     LocalAddr;
    SOCKADDR_IN     destAddr;
    int             sockfd;
    int             ret;
    
    //filling the UDP local socket address
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;
    
    //filling the UDP client socket address
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons((uint16_t)destPort);
    destAddr.sin_addr = htonl((int)destIp);

    // creating a UDP socket

     sockfd = socket(AF_INET,SOCK_DGRAM, 0);
     if (sockfd < RW_OK )
     {
       return RW_ERR;
     }

    // binding the UDP socket to the UDP server address
    ret = bind(sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret == RW_ERR )
    {
      close(sockfd);
      return ret;
    }

    // connect set IP/Port filter
    ret = connect(sockfd, (SOCKADDR_IN *)&destAddr, sizeof(destAddr));
    if (ret == RW_ERR )
    {
      close(sockfd);
      return ret;
    }
    
    return sockfd;

}

void RAK_UdpClient_EventHandle(void *p_arg)
{
       int      ret = 0;
SOCKADDR_IN     servAddr;
socklen_t       addrlen;
static int      send_recvCnt = 0;
       int      temp_len;
 
    char* c_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);

    if(NULL == c_buffer){
	DPRINTF("RAK_UdpClient: Allocate buffer failed.\n");
	while(1);
    }  
    
  while(1)
  {
client_loop:
    
    if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
    {
      rw_sysSleep(100);
      goto client_loop;      
    }
    
    if (app_demo_ctx.udpc_sockfd == INVAILD_SOCK_FD)
    {
      if((ret =RAK_UdpClient(12345, 25003, 0xC0A80389)) >= 0)
      {
        app_demo_ctx.udpc_sockfd = ret;
        app_demo_ctx.udpc_status = STATUS_OK;
        DPRINTF("RAK_UdpClient sockfd = %u creat \n\r",app_demo_ctx.udpc_sockfd);
      }
      else
      {
          DPRINTF("RAK_UdpClient creat failed code =%d\n", ret);
          rw_sysSleep(100);
          goto client_loop;     
      }    
    }
    
    if (app_demo_ctx.udpc_status <= STATUS_INIT)
    {
       if(app_demo_ctx.udpc_status < STATUS_INIT)
       {
          close(app_demo_ctx.udpc_sockfd);
          app_demo_ctx.udpc_status = STATUS_INIT;
          app_demo_ctx.udpc_sockfd = INVAILD_SOCK_FD; //close udp ,for next reconnect.
       }
       goto client_loop;
    }
   
    ret = recvfrom(app_demo_ctx.udpc_sockfd, c_buffer, MAX_RECV_PACKET_LEN, 0, (SOCKADDR_IN *)&servAddr,  &addrlen);
    if (ret <= 0 )
    {
      if(ret ==RW_ERR_SOCKET_INVAILD){
        DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.udpc_sockfd);
        app_demo_ctx.udpc_sockfd = INVAILD_SOCK_FD; //clear tcp ,for next reconnect.
      }
      
    }else
    {
//      DPRINTF("recvfrom 0x%x:%d  packets on sockfd=%d  data_len=%d\n\r", ntohl(servAddr.sin_addr), ntohs(servAddr.sin_port), app_demo_ctx.udpc_sockfd, ret );                
      send_recvCnt += ret;
    }
    
    if(send_recvCnt > 0)
    {
      temp_len = send_recvCnt >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:send_recvCnt;
      ret = sendto(app_demo_ctx.udpc_sockfd, c_buffer, temp_len, 0, (SOCKADDR_IN *)&servAddr, sizeof(servAddr));
      if (ret <= 0 )
      {
//        DPRINTF("send errorcode =%d\n\r",ret);
        if(ret ==RW_ERR_SOCKET_INVAILD)
        {
          app_demo_ctx.udpc_sockfd =INVAILD_SOCK_FD;
          send_recvCnt = 0;
        }                  
      }else
      {
        send_recvCnt -= ret;
      }
    }
   }
}

#ifdef  TCPS_TEST
int creat_tcpsTask(void)
{    
    int  xRet = 0;
    
    xRet =xTaskCreate(RAK_TcpServer_EventHandle, 
                      "TcpServer", 
                      configMINIMAL_STACK_SIZE * 2, 
                      NULL, 
                      RW_TCPS_TASK_PRIO, 
                      &g_tcpserver_task); 
    
    if(xRet !=pdPASS){
       DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
       while(1);
    }
    
}
#endif

#ifdef  TCPC_TEST
int creat_tcpcTask(void)
{
    int  xRet = 0;
    
    xRet =xTaskCreate(RAK_TcpClient_EventHandle, 
                      "TcpClient", 
                      configMINIMAL_STACK_SIZE * 2, 
                      NULL, 
                      RW_TCPC_TASK_PRIO, 
                      &g_tcpclient_task); 
    
    if(xRet !=pdPASS){
       DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
       while(1);
    }

}
#endif

#ifdef  UDPS_TEST
int creat_udpsTask(void)
{
    int  xRet = 0;
    
    xRet =xTaskCreate(RAK_UdpServer_EventHandle, 
                      "UdpServer", 
                      configMINIMAL_STACK_SIZE * 2, 
                      NULL, 
                      RW_UDPS_TASK_PRIO, 
                      &g_udpserver_task); 
    
    if(xRet !=pdPASS){
       DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
       while(1);
    }

}
#endif

#ifdef  UDPC_TEST
int creat_udpcTask(void)
{
    int  xRet = 0;
    
    xRet =xTaskCreate(RAK_UdpClient_EventHandle, 
                      "UdpClient", 
                      configMINIMAL_STACK_SIZE * 2, 
                      NULL, 
                      RW_UDPC_TASK_PRIO, 
                      &g_udpclient_task); 
    
    if(xRet !=pdPASS){
       DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
       while(1);
    }

}
#endif

