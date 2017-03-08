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
    rw_IpConfig_t      ipinfo;
    
    platform_init(); 
    rw_appdemo_context_init();

    DPRINTF("rw_network_startConfig ...\r\n");
    app_demo_ctx.easywps_mode = CONFIG_EASY;   
    rw_network_startConfig(app_demo_ctx.easywps_mode);
    
    while(1) {
			
        if (app_demo_ctx.rw_easywps_status == STATUS_OK) {
           app_demo_ctx.rw_easywps_status = STATUS_INIT;
           rw_network_init(&conn, DHCP_CLIENT, &ipinfo);
        }else if (app_demo_ctx.rw_easywps_status == STATUS_FAIL) {
           app_demo_ctx.rw_easywps_status = STATUS_INIT;
        }
				
        if (app_demo_ctx.rw_connect_status == STATUS_OK && app_demo_ctx.rw_ipquery_status == STATUS_OK) {
          rw_easy_responseToAPP(); 
					
        }else if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
          DPRINTF("reconnect and ipquery...\r\n");
          rw_appdemo_context_init();  
          rw_sysDriverReset();
          rw_network_init(&conn, DHCP_CLIENT, NULL);
        }
        rw_sysSleep(10);
    }

}


int main(void)
{	
  host_platformInit();	
  DPRINTF("Host platform init...success\r\n");
	
	xTaskCreate(rw_app_task, "app_task", configMINIMAL_STACK_SIZE * 6, NULL, START_TASK_PRIO, NULL); 

  /* Start the scheduler */
  vTaskStartScheduler();
    
  /* We should never get here as control is now taken by the scheduler */
  for( ;; );	
	
}
