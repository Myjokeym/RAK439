#include "rw_app.h"
#include "os.h"
#include "os_cfg_app.h"

#define   RW_MAIN_TASK_PRIO            5
#define   RW_MAIN_TASK_STACK_SIZE      200U

static    CPU_STK g_rw_task_stack[RW_MAIN_TASK_STACK_SIZE]; 
static    OS_TCB g_rw_task_tcb;
volatile  uint8_t g_connected;
static void rw_task_main(void *p_arg);

int main(void)
{
    OS_ERR oserr;

    /* Disable all interrupts until we are ready to accept
    * them.                                                */
    CPU_IntDis();
    /* Initialize "uC/OS-II, The Real-Time Kernel" */
    OSInit(&oserr);

    if(oserr != OS_ERR_NONE){
        while(1){};
    }

    /* Create the task                         */
    OSTaskCreate(&g_rw_task_tcb,                                        // TCB
                 "MASTER",                                        // name
                 rw_task_main,                                         // func
                 NULL,                                            // func(arg)
                 RW_MAIN_TASK_PRIO,                         // priority
                 &g_rw_task_stack[0],                            // stack
                 RW_MAIN_TASK_STACK_SIZE/10,                   // stack lim
                 RW_MAIN_TASK_STACK_SIZE,                     // stack sz
                 0,                                               // msq Q
                 1000,                                               // time quanta
                 NULL,                                            // ext ptr
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),     // opt(s)
                 &oserr);                                         // error
    if(oserr != OS_ERR_NONE){
        while(1){};
    }

    CPU_IntEn();
    /* Start multitasking (i.e. give control to uC/OS-II).  */
    OSStart(&oserr);

    if(oserr != OS_ERR_NONE){
        while(1){};
    }

    /* OSStart() never returns, serious error had occured if
    * code execution reached this point                    */
    while(1) ;
}

static int platform_init(void)
{
  rw_DriverParams_t     params;
  int                   ret =0;
  char                  libVersion[20]="";
  char                  module_mac[6] ="";
  
  // host platform init
  host_platformInit();
  DPRINTF("Host platform init...success\r\n");
  
  //rak module driver init
  wifi_init_params(&params);
  ret =rw_sysDriverInit(&params);
  if(ret != RW_OK)
  {
    DPRINTF("RAK module platform init...failed code=%d\r\n", ret);
    while(1); 
  }
  rw_getLibVersion(libVersion); 
  DPRINTF("rak wifi LibVersion:%s\r\n", libVersion);
  rw_getMacAddr(module_mac);
  DPRINTF("rak wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n", module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);
  
  return RW_OK;
}


static void rw_task_main(void *p_arg)
{
    platform_init();
    rw_appdemo_context_init();
    
//    rw_WlanNetworkInfoList_t scan_info;     
//    rw_wlanNetworkScan(NULL, 0);    
//    rw_wlanGetScanInfo(&scan_info);   
//    printf("scan num = %d\r\n", scan_info.num);  
//    vPortFree(scan_info.WlanNetworkInfo);
       
    rw_network_startSTA();
    
#ifdef  TCPS_TEST
    creat_tcpsTask();
#endif  
#ifdef  TCPC_TEST
    creat_tcpcTask();
#endif
#ifdef  UDPS_TEST
     creat_udpsTask();
#endif
#ifdef  UDPC_TEST
     creat_udpcTask();
#endif  
       
    while(1){ 
       if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
          DPRINTF("reconnect and ipquery...\r\n");          
          rw_appdemo_context_init();
          rw_sysDriverReset();          
          rw_network_startSTA();
        }
        rw_sysSleep(100);
    }

}