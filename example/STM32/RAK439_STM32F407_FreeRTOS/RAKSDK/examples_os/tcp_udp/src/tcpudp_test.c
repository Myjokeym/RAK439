#include "rw_app.h"

#define START_TASK_PRIO   (configMAX_PRIORITIES - 4)     
static void rw_app_task(void *p_arg);

int platform_init(void)
{
  rw_DriverParams_t     params;
  int                   ret =0;
  char                  libVersion[20]="";
  char                  module_mac[6] ="";
  
  //rak module driver init
  wifi_init_params(&params);
  ret =rw_sysDriverInit(&params);
  if(ret != RW_OK)
  {
    DPRINTF("RAK module platform init...failed\r\n");
    while(1); 
  }
  rw_getLibVersion(libVersion); 
  DPRINTF("rak wifi LibVersion:%s\r\n", libVersion);
  rw_getMacAddr(module_mac);
  DPRINTF("rak wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n", module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);
  
  return RW_OK;
}


static void rw_app_task(void *p_arg)
{

    platform_init(); 
    rw_appdemo_context_init();

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
    
    while(1) {
      
        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
          DPRINTF("reconnect and ipquery...\r\n");
          rw_appdemo_context_init();   
          rw_sysDriverReset();
          rw_network_init(&conn, DHCP_CLIENT, NULL);
        }
        rw_sysSleep(100);
    }

}


// int main(void)
// {	
//    //host_platformInit();
//    DPRINTF("Host platform init...success\r\n");
//   
//   xTaskCreate(rw_app_task, "app_task", configMINIMAL_STACK_SIZE * 2, NULL, START_TASK_PRIO, NULL); 
//   
//   /* Start the scheduler */
//   vTaskStartScheduler();
//     
//   /* We should never get here as control is now taken by the scheduler */
//   for( ;; );	
// 	
// }
