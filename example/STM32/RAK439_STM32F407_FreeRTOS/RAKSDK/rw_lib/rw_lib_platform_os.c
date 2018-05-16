/**
  ******************************************************************************
  * @file    rw_lib_platform_os.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module OS Demo rw_lib_platform_os Application C File.
  *
  *          This file contains:
  *           -customer callback,hardware interface init,system parameters setting
  * 
  ******************************************************************************
**/
#include "rw_app.h"


static uint32_t _init_interface(void);
static uint32_t _deinit_interface(void);
static void _power_up_down(uint8_t status);
static void _spi_io_buffer(uint8_t* write, uint8_t* read, uint16_t len);
static void _ext_interrupt(uint8_t enable);
//void delay_ms(int count);
rw_stamp_t get_stamp(void);

rw_WlanConnect_t   conn;
rw_WlanEasyConfigWpsResponse_t       easywpsinfo;

/***********rak module function callback****************/
static void scan_callback(rw_WlanNetworkInfoList_t* scan_info) 
{
   DPRINTF("scan num = 0x%x\r\n", scan_info->num);
   vPortFree(scan_info->WlanNetworkInfo);
}

static void connect_callback(uint8_t event ,  rw_WlanConnect_t* wlan_info, RW_DISCONNECT_REASON dis_code)
{
    DPRINTF("connect_callback event = 0x%x\r\n", event);
    switch(event) {
        case CONN_STATUS_STA_CONNECTED:
          {
            if (wlan_info != NULL) {
              DPRINTF("---------connected AP info list---------\r\n");
              DPRINTF("bssid = %02X:%02X:%02X:%02X:%02X:%02X\r\n", wlan_info->bssid[0]
                                                                 , wlan_info->bssid[1] 
                                                                 , wlan_info->bssid[2]  
                                                                 , wlan_info->bssid[3]  
                                                                 , wlan_info->bssid[4]  
                                                                 , wlan_info->bssid[5] );
              DPRINTF("channel =%d\r\n", wlan_info->channel);
              DPRINTF("ssid =%s\r\n", wlan_info->ssid);
              DPRINTF("psk =%s\r\n", wlan_info->psk);
              DPRINTF("sec_mode =%d\r\n", wlan_info->sec_mode);
              DPRINTF("auth_mode =%d\r\n", wlan_info->auth_mode);             
            }
            app_demo_ctx.rw_connect_status =STATUS_OK;
            DPRINTF("---------CONN_STATUS_STA_CONNECTED--------\r\n");
            break;
		      }
        case CONN_STATUS_STA_DISCONNECT:
            app_demo_ctx.rw_connect_status =STATUS_FAIL;
            DPRINTF("---------CONN_STATUS_STA_DISCONNECT Code =%d---\r\n" ,dis_code);
            break;
        case CONN_STATUS_AP_ESTABLISH:
	    app_demo_ctx.rw_connect_status = STATUS_OK;
            DPRINTF("---------CONN_STATUS_AP_ESTABLISH--------\r\n");
            break;
        case CONN_STATUS_AP_CLT_CONNECTED:
            DPRINTF("---------CONN_STATUS_AP_CLT_CONNECTED--------\r\n");
            break;
        case CONN_STATUS_AP_CLT_DISCONNECT:
            DPRINTF("---------CONN_STATUS_AP_CLT_DISCONNECT Code =%d--\r\n" ,dis_code);
            break;
        default:
            DPRINTF("error value\r\n");
            break;
    }
}

static void wps_easy_callback(rw_WlanEasyConfigWpsResponse_t* info , int status)
{
    if (status == 0) {      
      DPRINTF("bssid = %02X:%02X:%02X:%02X:%02X:%02X\r\n", info->bssid[0]
                                                        , info->bssid[1] 
                                                        , info->bssid[2]  
                                                        , info->bssid[3]  
                                                        , info->bssid[4]  
                                                        , info->bssid[5] );
      DPRINTF("channel =%d\r\n", info->channel);
      DPRINTF("ssid =%s\r\n", info->ssid);
      DPRINTF("psk =%s\r\n", info->psk); 
       
      app_demo_ctx.rw_easywps_status = STATUS_OK;
			memcpy(&easywpsinfo,info,sizeof(easywpsinfo));
      memset(&conn, 0, sizeof(conn));
      if(app_demo_ctx.easywps_mode ==CONFIG_WPS){
        conn.bssid = NULL;
        conn.channel = 0;
      }else{
        conn.bssid = easywpsinfo.bssid; 
        conn.channel = easywpsinfo.channel;
      }         
      conn.ssid = (char*)easywpsinfo.ssid; 
      conn.psk = (char*)easywpsinfo.psk;
      conn.pmk = NULL;
      if(info->psk[0] != 0){
        conn.sec_mode = RW_SEC_TYPE_SEC;
      }else{
        conn.sec_mode = RW_SEC_TYPE_OPEN;
      }    
      conn.auth_mode = RW_AUTH_TYPE_AUTO;
      conn.role_mode = ROLE_STA;     
    }else
    {    
      app_demo_ctx.rw_easywps_status = STATUS_FAIL;
      DPRINTF("wps_easy config fail \r\n");
    }
}

static void customer_assert(const char* file, int line)
{
    DPRINTF("---------Assert rw_lib: %s:%d---------\r\n", file, line);
    while(1);
}
/*****************************END*******************************/


// void HAL_GPIO_EXTI_Callback(void)
// {
// 		DRIVER_INT_HANDLE();
// }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_4)
  {
		DRIVER_INT_HANDLE();
  }
}



void wifi_init_params(rw_DriverParams_t* params)
{
    params->driver_cb.hw_init = _init_interface;    
    params->driver_cb.hw_deinit = _deinit_interface;
    params->driver_cb.hw_power = _power_up_down;
    
    params->driver_cb.spi_io_buffer = _spi_io_buffer;    
    params->driver_cb.time_delay = delay_ms;
    params->driver_cb.Stamp_get = get_stamp;   
    params->driver_cb.toggle_irq = _ext_interrupt;   
    params->driver_cb.driver_free = vPortFree;
    params->driver_cb.driver_malloc = pvPortMalloc;
    params->driver_cb.driver_assert = customer_assert;
    
    
    params->app_cb.conn_cb = connect_callback; 
    params->app_cb.scan_cb = scan_callback;
    params->app_cb.easy_wps_cb = wps_easy_callback;

    params->spi_int_enable =true;

    params->rx_queue_num = 5;
    params->tx_queue_num = 5;
		
    params->tx_max_len = MAX_SEND_PACKET_LEN;
    
    params->scan_max_num = 10;
    
    params->tcp_retry_num = 5;
    
    params->socket_max_num = 8;
    
    params->country_code = "CN";
    
    params->host_name = "rakmodule";

}

static uint32_t _init_interface(void)
{
//     WIFI_GPIO_Init();
//     WIFI_SPI_Init();
//     WIFI_INT_Init();
 	  MX_GPIO_Init();
 		MX_SPI1_Init();
 		MX_USART1_UART_Init();
//		DPRINTF("_init_interface\r\n");
	  return 0;
}

static uint32_t _deinit_interface(void)
{
  //WIFI_SPI_Deinit();
	//DPRINTF("_deinit_interface\r\n");

	HAL_SPI_DeInit(&hspi1);
	return 0;
}

static void _power_up_down(uint8_t status)
{
	//DPRINTF("_power_up_down\r\n");
  if (status) {
#if defined (USE_WIFI_POWER_FET)
    GPIO_WriteBit(WIFI_FET_GPIO_PORT, WIFI_FET_PIN, Bit_RESET);
#endif
    delay_ms(1);
		BSP_WIFIPWD_Up();
  } else {
#if defined (USE_WIFI_POWER_FET)
    GPIO_WriteBit(WIFI_FET_GPIO_PORT, WIFI_FET_PIN, Bit_SET);
#endif
		BSP_WIFIPWD_Down();

	}
}


/*µ÷ÓÃHAL¿âµÄSPI½Ó¿ÚÊµÏÖµÄ_spi_io_buffer*/

static void _spi_io_buffer(uint8_t* write, uint8_t* read, uint16_t len)
{
  uint32_t 						i;
  uint8_t 						dummy;
  uint8_t 						recv;
  HAL_StatusTypeDef status = HAL_OK;

	BSP_WIFICS_Down();
	
	if (write != NULL)
	{
			for (int i=0;i<len;++i)
			{
				printf("%x ",*(write+i)); 
			}
			printf("\r\n");
	}


	
	if(read == NULL) {
			for(i=0;i<len;i++) {
				if(write == NULL) {
					status = HAL_SPI_TransmitReceive(&hspi1,&dummy,&recv,1,5000);
					assert_param(status == HAL_OK);
				}else {
					status = HAL_SPI_TransmitReceive(&hspi1,&write[i],&recv,1,5000);
					assert_param(status == HAL_OK);
				}
			}
		} 
		else {
			for(i=0;i<len;i++) {
				if(write == NULL) {
					status = HAL_SPI_TransmitReceive(&hspi1,&dummy,&read[i],1,5000);
					assert_param(status == HAL_OK);				
				}else {
					status = HAL_SPI_TransmitReceive(&hspi1,&write[i],&read[i],1,5000);		
					assert_param(status == HAL_OK);
				}
			}
		}	

	BSP_WIFICS_Up();
}


#if 0
static void _spi_io_buffer(uint8_t* write, uint8_t* read, uint16_t len)
{
  uint32_t 						i;
  uint8_t 						dummy;
  uint8_t 						recv;
  
	DPRINTF("_spi_io_buffer\r\n");
  //GPIO_WriteBit(WIFI_CS_GPIO_PORT, WIFI_CS_PIN, Bit_RESET);
	BSP_WIFICS_Down();
#if defined SPI_DMA	
  
#else
  if(read == NULL) {
    for(i=0;i<len;i++) {
      while((WIFI_SPI->SR&SPI_FLAG_TXE)==RESET)	;
      if(write == NULL) {
        WIFI_SPI->DR = dummy;
      }else {
        WIFI_SPI->DR = write[i];
      }
      	  printf("send=%x      ",write[i]); 
      while((WIFI_SPI->SR&SPI_FLAG_RXNE)==RESET);
      recv = WIFI_SPI->DR;
      	  printf("recv dummy=%x\r\n",dummy);
    }
  } 
  else {
    for(i=0;i<len;i++) {
      while((WIFI_SPI->SR&SPI_FLAG_TXE)==RESET);
      if(write == NULL) {
        WIFI_SPI->DR = dummy;
        	  printf("send dummy=%x  ",dummy); 
      }else {
        WIFI_SPI->DR = write[i];
        	   printf("send=%x  ",write[i]);
      }
      while((WIFI_SPI->SR&SPI_FLAG_RXNE)==RESET);
      read[i] = WIFI_SPI->DR;
      	  printf("recv=%x\r\n",read[i]);
    }
  }
#endif
  //GPIO_WriteBit(WIFI_CS_GPIO_PORT, WIFI_CS_PIN, Bit_SET);
	BSP_WIFICS_Up();
}
#endif




static void _ext_interrupt(uint8_t enable)
{
	//DPRINTF("_ext_interrupt\r\n");
  if (enable){
		NVIC_EnableIRQ(WIFI_INT_EXTI_IRQN);
	}
  else{
    NVIC_DisableIRQ(WIFI_INT_EXTI_IRQN);
  }   
}

void delay_ms(int count)
{
	osDelay(count);
}

rw_stamp_t get_stamp(void)
{
	return osKernelSysTick();
}
