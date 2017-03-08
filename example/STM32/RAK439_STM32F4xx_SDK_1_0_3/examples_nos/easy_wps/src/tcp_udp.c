/**
  ******************************************************************************
  * @file    tcp_udp.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module non-OS Demo tcp_udp Application C File.
  *
  *          This file contains:
  *           -socket api function,how to creat socket,recv and send data
  * 
  ******************************************************************************
**/
#include "rw_app.h"

char               temp_buf[MAX_RECV_PACKET_LEN] ={0};


int RAK_TcpClient(uint16_t destPort, int destIp)
{
    SOCKADDR_IN     destAddr;
    SOCKADDR_IN     LocalAddr;
    static int      tmp_sockfd = INVAILD_SOCK_FD;
    int             sockfd;
    int             ret;
    int             lPort;

    //filling the TCP client socket address
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons((uint16_t)destPort);
    destAddr.sin_addr = htonl((int)destIp);
    
    // creating a TCP socket  
    if(tmp_sockfd ==INVAILD_SOCK_FD)
    {
      tmp_sockfd = socket( AF_INET,SOCK_STREAM, 0);
      if (tmp_sockfd < RW_OK)
      {
        return RW_ERR;
      }
    }
    
    srand((int)get_stamp());
    /*port : 1024 - 32768 */
    lPort = (rand() % (32768-1024+1))+ 1024; 
    
    //filling the TCP client local port
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;
    
    // binding the TCP socket to the TCP server address
    ret = bind(tmp_sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
       close(tmp_sockfd);
       tmp_sockfd = INVAILD_SOCK_FD;
       return ret;
    }
    
    // connecting to TCP server
    ret = connect(tmp_sockfd, (SOCKADDR_IN *)&destAddr, sizeof(destAddr));
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
      close(tmp_sockfd);
      tmp_sockfd = INVAILD_SOCK_FD;
      return ret;
    }
   
    if (ret == RW_OK) 
    {     
      sockfd = tmp_sockfd ;
      tmp_sockfd = INVAILD_SOCK_FD;
      return sockfd;
    }
    
    return ret;
}

void RAK_TcpClient_EventHandle(void)
{
       int      ret = 0;
static int      send_recvCnt = 0;
       int      temp_len;
      
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
          DPRINTF("RAK_TcpClient creat failed\n\r");
        }
        return;
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
       return ;
    }
   
    ret = recv(app_demo_ctx.tcp_cloud_sockfd, temp_buf, MAX_RECV_PACKET_LEN, 0);
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
      ret = send(app_demo_ctx.tcp_cloud_sockfd, temp_buf, temp_len, 0);
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

int RAK_TcpServer(uint16_t lPort)
{
    SOCKADDR_IN     LocalAddr;
    static int      tmp_sockfd = INVAILD_SOCK_FD;    
    int             sockfd;
    int             ret;
    
    //filling the TCP server socket address
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;

    // creating a TCP socket
    if(tmp_sockfd == INVAILD_SOCK_FD)
    {
      tmp_sockfd = socket(AF_INET,SOCK_STREAM, 0);
      if (tmp_sockfd < RW_OK )
      {
        return RW_ERR;
      }
    }
    // binding the TCP socket to the TCP server address
    ret = bind(tmp_sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
      close(sockfd);
      return ret;
    }

    // putting the socket for listening to the incoming TCP connection
    ret = listen(tmp_sockfd, 1);
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
      close(tmp_sockfd);
      return ret;
    }

    if (ret == RW_OK) 
    {
      sockfd = tmp_sockfd;
      tmp_sockfd = INVAILD_SOCK_FD;
      return sockfd;
    }
    
    return ret;
}

int RAK_TcpSRecvClients(int sever_fd, RW_APP_CTX* app_ctx)
{
    SOCKADDR_IN     new_tcpinfo;
    int             ret = 0;
    int             info_size;
    int             sockfd;
    int             i= 0;
       
    ret = select(sever_fd, 1);
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT) 
    {
        return ret;
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
          app_ctx->ltcps_clientinfos[i].tcpc_info.sin_port = ntohs(new_tcpinfo.sin_port);
          app_ctx->ltcps_clientinfos[i].tcpc_info.sin_family = ntohs(new_tcpinfo.sin_family);
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
    else if (sockfd == RW_ERR || sockfd == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD) 
    {
        return sockfd;
    }

    return RW_OK;
}

void RAK_TcpServer_EventHandle(void)
{
    int ret = 0,i=0;
    int send_templen =0;
    
    if (app_demo_ctx.ltcps_sockfd == INVAILD_SOCK_FD)
    {
        if((ret =RAK_TcpServer(25000)) >= 0)
        {
           app_demo_ctx.ltcps_sockfd = ret;
           DPRINTF("RAK_TcpServer sockfd = %u creat \n",app_demo_ctx.ltcps_sockfd);
        }
        else
        {
          if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
          {             
              DPRINTF("RAK_TcpServer creat failed code=%d \n", ret);
           }
           return;
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
        
          ret = recv(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, temp_buf, MAX_RECV_PACKET_LEN, 0);
          if (ret <= 0 )
          {
            if (ret ==RW_ERR_SOCKET_INVAILD){
              DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd);
              close(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd);
              app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
              app_demo_ctx.tcpc_num --;
              app_demo_ctx.ltcps_clientinfos[i].loopback_count = 0;
              break;
            }
          }else
          {
//            DPRINTF("recv packets on sockfd=%d  data_len=%d\n\r", app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, ret );                 
            app_demo_ctx.ltcps_clientinfos[i].loopback_count += ret;
          }

          if(app_demo_ctx.ltcps_clientinfos[i].loopback_count > 0)
            {
                send_templen = app_demo_ctx.ltcps_clientinfos[i].loopback_count >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:app_demo_ctx.ltcps_clientinfos[i].loopback_count;
                ret = send(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, temp_buf, send_templen, 0);
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

int RAK_UdpServer(uint16_t lPort)
{
    SOCKADDR_IN     LocalAddr;
    static int      tmp_sockfd =INVAILD_SOCK_FD;
    int             sockfd;
    int             ret;
    
    //filling the UDP server socket address
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons((uint16_t)lPort);
    LocalAddr.sin_addr = 0;

    // creating a UDP socket
    if(tmp_sockfd == INVAILD_SOCK_FD)
    {
      tmp_sockfd = socket(AF_INET,SOCK_DGRAM, 0);
      if (tmp_sockfd < RW_OK )
      {
        return RW_ERR;
      }
    }
    // binding the UDP socket to the UDP server address
    ret = bind(tmp_sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
      close(tmp_sockfd);
      return ret;
    }

    if (ret == RW_OK) 
    {
      sockfd = tmp_sockfd;
      tmp_sockfd = INVAILD_SOCK_FD;
      return sockfd;
    }
    
    return ret;

}

void RAK_UdpServer_EventHandle(void)
{
    SOCKADDR_IN     servAddr;
    socklen_t       addrlen;
    int             ret = 0;
    static int      send_recvCnt = 0;
    int             temp_len;
    
    if (app_demo_ctx.ludps_sockfd == INVAILD_SOCK_FD)
    {
        if((ret =RAK_UdpServer(25002)) >= 0)
        {
           app_demo_ctx.ludps_sockfd = ret;
           DPRINTF("RAK_UdpServer sockfd = %u creat \n",app_demo_ctx.ludps_sockfd);
        }else{
           DPRINTF("RAK_UdpServer creat failed code =%d\n", ret);
           return;
        }
    }
    
    ret = recvfrom(app_demo_ctx.ludps_sockfd, temp_buf, MAX_RECV_PACKET_LEN, 0, (SOCKADDR_IN *)&servAddr,  &addrlen);
    if (ret <= 0 )
    {
      if(ret ==RW_ERR_SOCKET_INVAILD){
        DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.ludps_sockfd);
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
      ret = sendto(app_demo_ctx.ludps_sockfd, temp_buf, temp_len, 0, (SOCKADDR_IN *)&servAddr, sizeof(servAddr));
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

int RAK_UdpClient(uint16_t lPort, uint16_t destPort, int destIp)
{
    SOCKADDR_IN     LocalAddr;
    SOCKADDR_IN     destAddr;
    static int      tmp_sockfd =INVAILD_SOCK_FD;
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
    if(tmp_sockfd == INVAILD_SOCK_FD)
    {
      tmp_sockfd = socket(AF_INET,SOCK_DGRAM, 0);
      if (tmp_sockfd < RW_OK )
      {
        return RW_ERR;
      }
    }
    // binding the UDP socket to the UDP server address
    ret = bind(tmp_sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
      close(tmp_sockfd);
      return ret;
    }

    // connect set IP/Port filter
    ret = connect(tmp_sockfd, (SOCKADDR_IN *)&destAddr, sizeof(destAddr));
    if (ret == RW_ERR || ret == RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD)
    {
      close(tmp_sockfd);
      tmp_sockfd = INVAILD_SOCK_FD;
      return ret;
    }
    
    if (ret == RW_OK) 
    {
      sockfd = tmp_sockfd;
      tmp_sockfd = INVAILD_SOCK_FD;
      return sockfd;
    }
    
    return ret;

}

void RAK_UdpClient_EventHandle(void)
{
       int      ret = 0;
SOCKADDR_IN     servAddr;
socklen_t       addrlen;
static int      send_recvCnt = 0;
       int      temp_len;
      
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
        if(ret == RW_ERR || ret==RW_ERR_TIME_OUT || ret ==RW_ERR_SOCKET_INVAILD) 
        { 
          DPRINTF("RAK_UdpClient creat failed code =%d\n", ret);
        }
        return;
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
       return ;
    }
   
    ret = recvfrom(app_demo_ctx.udpc_sockfd, temp_buf, MAX_RECV_PACKET_LEN, 0, (SOCKADDR_IN *)&servAddr,  &addrlen);
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
      ret = sendto(app_demo_ctx.udpc_sockfd, temp_buf, temp_len, 0, (SOCKADDR_IN *)&servAddr, sizeof(servAddr));
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
