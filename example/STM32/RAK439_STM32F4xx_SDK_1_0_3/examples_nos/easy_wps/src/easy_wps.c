/**
  ******************************************************************************
  * @file    easy_wps.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module non-OS Demo easy_wps Application C File.
  *
  *          This file contains:
  *           -easy config network ,response the module mac to andriod/ios app
  * 
  ******************************************************************************
**/
#include "rw_app.h"

#define EASY_RSP_TIMEOUT    10000                     //ms

void rw_easy_responseToAPP(void)
{
    SOCKADDR_IN     servAddr;
    socklen_t       addrlen;
    int             ret = 0 ;
    uint8_t         respone_easy[42]={0};
    
    if (app_demo_ctx.easy_sockfd == INVAILD_SOCK_FD)
    {
        if((ret =RAK_UdpServer(55555)) >= 0)    //rak easyconfig use local port 55555
        {
           app_demo_ctx.easy_sockfd = ret;
           DPRINTF("RAK_UdpServer sockfd = %u creat \n\r",app_demo_ctx.easy_sockfd);
        }else{
           DPRINTF("RAK_UdpServer creat failed\n\r");
           return;
        }
        rwSetFutureStamp((rw_stamp_t*)&app_demo_ctx.easy_rsptimeout, EASY_RSP_TIMEOUT);
    }
    
    ret = recvfrom(app_demo_ctx.easy_sockfd, temp_buf, MAX_RECV_PACKET_LEN, 0, (SOCKADDR_IN *)&servAddr,  &addrlen);
    if (ret <= 0 )
    {
      if(ret ==RW_ERR_SOCKET_INVAILD){
        DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.easy_sockfd);
        close(app_demo_ctx.easy_sockfd);
        app_demo_ctx.easy_sockfd = INVAILD_SOCK_FD; //close
      }
      
    }else
    {
      temp_buf[ret]= 0;
      DPRINTF("recvfrom 0x%x:%d on sockfd=%d data_len=%d :%s\n\r", ntohl(servAddr.sin_addr), ntohs(servAddr.sin_port), app_demo_ctx.easy_sockfd, ret ,temp_buf);             
      
      if (0 == strncmp(temp_buf,"@LT_EASY_DEVICE@",16)) 
      {
        if (rwIsStampPassed((rw_stamp_t*)&app_demo_ctx.easy_rsptimeout))
        {
          DPRINTF("Easy response timeout ...stop rsponse\n\r");
          return;
        }
      }else if(0 == strncmp(temp_buf,"@LT_WIFI_DEVICE@",16))
      {
        //提供本地发现服务
      }else
      {
        return;
      }
      rw_getMacAddr((char*)&respone_easy[36]);
      ret = sendto(app_demo_ctx.easy_sockfd, respone_easy, 42, 0, (SOCKADDR_IN *)&servAddr, sizeof(servAddr));
      if (ret <= 0 )
      {
        DPRINTF("sendto data error code =%d\n\r",ret);
      }else
      {
        DPRINTF("local Discovery Response\n\r");
      }
    }

  return ;
}

