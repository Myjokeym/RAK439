#ifndef __BSP_H
#define __BSP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#define USE_WIFI_POWER_FET

#if defined (USE_WIFI_POWER_FET)
#define WIFI_FET_GPIO_PORT             			 GPIOB
#define WIFI_FET_PIN                   			 GPIO_Pin_10
#define WIFI_FET_GPIO_CLK              			 RCC_AHB1Periph_GPIOB
#endif

#define WIFI_PWD_GPIO_PORT             			 GPIOB
#define WIFI_PWD_PIN                   			 GPIO_Pin_1
#define WIFI_PWD_GPIO_CLK              			 RCC_AHB1Periph_GPIOB

#define WIFI_INT_GPIO_PORT             			 GPIOB
#define WIFI_INT_PIN                   			 GPIO_Pin_0
#define WIFI_INT_GPIO_EXTI_PORT        			 EXTI_PortSourceGPIOB
#define WIFI_INT_EXTI_PIN_SOURCE       			 EXTI_PinSource0
#define WIFI_INT_EXTI_LINE             			 EXTI_Line0
#define WIFI_INT_EXTI_IRQN             			 EXTI0_IRQn
	
/* WIFI SPI Interface pins  */  
#define WIFI_SPI                             SPI1
#define WIFI_SPI_CLK                         RCC_APB2Periph_SPI1
#define WIFI_SPI_CLK_INIT                    RCC_APB2PeriphClockCmd

#define WIFI_SPI_SCK_PIN                     GPIO_Pin_5
#define WIFI_SPI_SCK_GPIO_PORT               GPIOA
#define WIFI_SPI_SCK_GPIO_CLK                RCC_AHB1Periph_GPIOA
#define WIFI_SPI_SCK_SOURCE                  GPIO_PinSource5
#define WIFI_SPI_SCK_AF                      GPIO_AF_SPI1

#define WIFI_SPI_MISO_PIN                    GPIO_Pin_6
#define WIFI_SPI_MISO_GPIO_PORT              GPIOA
#define WIFI_SPI_MISO_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define WIFI_SPI_MISO_SOURCE                 GPIO_PinSource6
#define WIFI_SPI_MISO_AF                     GPIO_AF_SPI1

#define WIFI_SPI_MOSI_PIN                    GPIO_Pin_7
#define WIFI_SPI_MOSI_GPIO_PORT              GPIOA
#define WIFI_SPI_MOSI_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define WIFI_SPI_MOSI_SOURCE                 GPIO_PinSource7
#define WIFI_SPI_MOSI_AF                   	 GPIO_AF_SPI1

#define WIFI_CS_PIN                          GPIO_Pin_4
#define WIFI_CS_GPIO_PORT                    GPIOA
#define WIFI_CS_GPIO_CLK                     RCC_AHB1Periph_GPIOA



#define PRINT_USART                          USART1 
#define PRINT_USART_CLK                      RCC_APB2Periph_USART1
#define PRINT_USART_INIT                     RCC_APB2PeriphClockCmd

#define PRINT_USART_TX_PIN                   GPIO_Pin_9
#define PRINT_USART_TX_GPIO_PORT             GPIOA
#define PRINT_USART_TX_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define PRINT_USART_TX_SOURCE                GPIO_PinSource9
#define PRINT_USART_TX_AF                    GPIO_AF_USART1

#define PRINT_USART_RX_PIN                   GPIO_Pin_10
#define PRINT_USART_RX_GPIO_PORT             GPIOA
#define PRINT_USART_RX_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define PRINT_USART_RX_SOURCE                GPIO_PinSource10
#define PRINT_USART_RX_AF                    GPIO_AF_USART1

void host_platformInit(void);
void WIFI_GPIO_Init(void);
void WIFI_SPI_Init(void);
void WIFI_INT_Init(void);
void WIFI_SPI_Deinit(void);
void HAL_GPIO_EXTI_Callback(void);

void Print_USART_Init(void);
void delay_ms(int count);
uint32_t HAL_GetTick(void);

#define   DEBUG 

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

#ifdef DEBUG
//#define DPRINTF(fmt, args...) printf(fmt, ##args)
#define DPRINTF(fmt, args...) do { printf("%d  ""%s" ":%u  ", HAL_GetTick(), clean_filename(__FILE__), __LINE__); printf(fmt, ##args); } while(0)
#else
#define DPRINTF(fmt, args...)
#endif

#endif //__BSP_H
