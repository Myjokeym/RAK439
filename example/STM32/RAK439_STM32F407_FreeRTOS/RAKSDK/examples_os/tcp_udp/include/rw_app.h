/**
  ******************************************************************************
  * @file    rw_app.h
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module OS Demo Application Header File.
  *
  *          This file contains:
  *           - Data structures and network,socket api function
  * 
  ******************************************************************************
**/
#include "bsp.h"
#include "rw_lib.h"
#include "rw_socket.h"
#include "rw_error.h"  

#define   ALLOW_MAX_NUMS       1
#define   INVAILD_SOCK_FD     -1

//#define   TCPS_TEST   
#define   TCPC_TEST  
//#define   UDPS_TEST  
//#define   UDPC_TEST  

typedef enum
{    
  STATUS_FAIL = -1 , 
  STATUS_INIT = 0 ,
  STATUS_OK   = 1
}rw_CmdStatus_t;


typedef struct 
{
  int             tcpc_sockfd;
  int             loopback_count;
  SOCKADDR_IN     tcpc_info;
}TCPC_INFO_T;

typedef struct
{
   int                  rw_connect_status;    //save wifi network connet status
   int                  rw_ipquery_status;    //save wifi network ipquery status
                                              // customer can add needed status here
   rw_ConfigMode_t      easywps_mode;
   int                  rw_easywps_status;    //save easywps cmd excute result success or fail
   int                  easy_sockfd;         //save ludp socket fd for easyconfig to response app
   int                  easy_rsptimeout; 
	
   int                  ltcps_sockfd;        //save local tcp server service socket fd
   int                  tcpc_num;            //current tcp client nums connect to the server 
   TCPC_INFO_T          ltcps_clientinfos[ALLOW_MAX_NUMS];
	
   int                  tcp_cloud_sockfd;    //save cloud tcp client connect socket fd
   int                  tcp_cloud_status;    //save cloud tcp client connect status whether connected
   
   int                  ludps_sockfd;        //save local udp server service socket fd 
   
   int                  udpc_sockfd;    //save cloud tcp client connect socket fd
   int                  udpc_status;
}RW_APP_CTX;

rw_stamp_t get_stamp(void);
void wifi_init_params(rw_DriverParams_t* params);

void rw_appdemo_context_init(void);
int rw_network_init(rw_WlanConnect_t* pconn, rw_IpConfigMode_t ipmode, rw_IpConfig_t* ipconfig);
int rw_network_startAP(void);
int rw_network_startSTA(void);
int  rw_network_startConfig(rw_ConfigMode_t mode);

int RAK_TcpServer(uint16_t lPort);
int RAK_TcpSRecvClients(int sever_fd, RW_APP_CTX* app_ctx);
int RAK_TcpClient(uint16_t destPort, int destIp);
int RAK_UdpServer(uint16_t lPort);
int RAK_UdpClient(uint16_t lPort, uint16_t destPort, int destIp);


void RAK_TcpClient_EventHandle(void *p_arg);
void RAK_TcpServer_EventHandle(void *p_arg);
void RAK_UdpServer_EventHandle(void *p_arg);
void RAK_UdpClient_EventHandle(void *p_arg);

int creat_tcpsTask(void);
int creat_tcpcTask(void);
int creat_udpsTask(void);
int creat_udpcTask(void);

void rw_easy_responseToAPP(void);


extern RW_APP_CTX        app_demo_ctx;
extern rw_WlanConnect_t  conn;

void delay_ms(int count);

