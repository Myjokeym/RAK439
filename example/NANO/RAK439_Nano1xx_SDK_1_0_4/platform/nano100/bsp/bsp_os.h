#ifndef  _BSP_H_
#define  _BSP_H_

#include "nano1xx.h"
#include "nano1xx_sys.h"
#include "nano1xx_uart.h"
#include "nano1xx_spi.h"
#include "nano1xx_gpio.h"
#include "nano1xx_pdma.h"
#include "nano1xx_fmc.h"
#include "nano1xx_timer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "os.h"
#include "os_cfg_app.h"

#define   DEBUG   

extern uint32_t __IO u32Timer0Cnt;

#define DEFAULT_VALUE         {\
                                  115200,\
                                  DRVUART_DATABITS_8,\
                                  DRVUART_STOPBITS_1,\
                                  DRVUART_PARITY_NONE,\
                                  DRVUART_FIFO_1BYTES,\
                                  DISABLE\
                              }

#define USE_WIFI_POWER_FET


#define WIFI_PORT_INT                           GPIOB
#define WIFI_PIN_INT                            15

#if defined (USE_WIFI_POWER_FET)
#define WIFI_PORT_FET                           GPIOA
#define WIFI_PIN_FET                            0
#endif

#define WIFI_PORT_PWD                           GPIOE
#define WIFI_PIN_PWD                            5


#define PRINT_UART                              UART0
/* WIFI SPI Interface pins  */  
#define WIFI_SPI                                SPI0


static __INLINE void sys_soft_reset(void)
{
    NVIC_SystemReset();
}

static __INLINE void sys_hard_reset(void)
{
    GCR->IPRST_CTL1 |= GCR_IPRSTCTL1_CHIP;
    while(1);
}

#ifdef DEBUG
static const char* clean_filename(const char* path)
{
  const char* filename = path + strlen(path); 
  while(filename > path)
  {
    if(*filename == '/' || *filename == '\\')
    {
      return filename + 1;
    }
    filename--;
  }
  return path;
}
#endif

extern OS_MUTEX   buf_mutex;

static inline void p_lock_mutex(OS_MUTEX *mutex)
{
  OS_ERR oserr;  
  OSMutexPend(mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &oserr);
}


static inline void p_unlock_mutex(OS_MUTEX *mutex)
{
  OS_ERR oserr;  
  OSMutexPost(mutex, OS_OPT_POST_NONE, &oserr);
}

#ifdef DEBUG
//#define DPRINTF(fmt, args...) printf(fmt, ##args)
#define DPRINTF(fmt, args...) do { \
                                   p_lock_mutex(&buf_mutex); \
                                   printf("%d  ""%s" ":%u  ", get_stamp(), clean_filename(__FILE__), __LINE__); \
                                   p_unlock_mutex(&buf_mutex); printf(fmt, ##args); \
                                 } while(0)
#else
#define DPRINTF(fmt, args...)
#endif

void host_platformInit(void);
void WIFI_GPIO_Init(void);
void WIFI_SPI_Init(void);
void WIFI_INT_Init(void);
void WIFI_SPI_Deinit(void);

void SYS_Config(void);
void UART_TxByte(UART_TypeDef  *tUART, uint8_t data);
int  Print_UART_TxBuf(uint8_t *buffer, int nbytes);
void Print_UART_Init(void);
int  UART0_RxByte(void);

#endif
