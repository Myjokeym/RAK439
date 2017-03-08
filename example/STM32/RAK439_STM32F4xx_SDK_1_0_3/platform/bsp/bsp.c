#include "bsp.h"
#include "stdio.h"


#if   defined ( __CC_ARM )
  #ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #endif /* __GNUC__ */
  /**
    * @brief  Retargets the C library printf function to the USART.
    * @param  None
    * @retval None
    */
  PUTCHAR_PROTOTYPE
  {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    USART_SendData(PRINT_USART, (uint16_t) ch);
          
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(PRINT_USART, USART_FLAG_TC) == RESET)
    {};

    return ch;
  }
#endif
	
void WIFI_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/* GPIOA GPIOB Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(WIFI_PWD_GPIO_CLK|WIFI_CS_GPIO_CLK, ENABLE);
	
#if defined (USE_WIFI_POWER_FET)
	RCC_AHB1PeriphClockCmd(WIFI_FET_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = WIFI_FET_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(WIFI_FET_GPIO_PORT, &GPIO_InitStructure);	
	GPIO_WriteBit(WIFI_FET_GPIO_PORT, WIFI_FET_PIN, Bit_SET);
#endif

	GPIO_InitStructure.GPIO_Pin = WIFI_PWD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(WIFI_PWD_GPIO_PORT, &GPIO_InitStructure);	
	GPIO_WriteBit(WIFI_PWD_GPIO_PORT, WIFI_PWD_PIN, Bit_RESET);
	
  GPIO_InitStructure.GPIO_Pin = WIFI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(WIFI_CS_GPIO_PORT, &GPIO_InitStructure);	
	GPIO_WriteBit(WIFI_CS_GPIO_PORT, WIFI_CS_PIN, Bit_SET);
	
}

void Print_USART_Init(void) 
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
  PRINT_USART_INIT(PRINT_USART_CLK, ENABLE);
	
  RCC_AHB1PeriphClockCmd(PRINT_USART_TX_GPIO_CLK|PRINT_USART_RX_GPIO_CLK, ENABLE);
	
  /*!< Connect SPI pins to AF */  
  GPIO_PinAFConfig(PRINT_USART_TX_GPIO_PORT, PRINT_USART_TX_SOURCE, PRINT_USART_TX_AF);
  GPIO_PinAFConfig(PRINT_USART_RX_GPIO_PORT, PRINT_USART_RX_SOURCE, PRINT_USART_RX_AF);	
 
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
  GPIO_InitStructure.GPIO_Pin = PRINT_USART_TX_PIN;
  GPIO_Init(PRINT_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = PRINT_USART_RX_PIN;
  GPIO_Init(PRINT_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
 USART_InitStructure.USART_BaudRate = 115200;
 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 USART_InitStructure.USART_StopBits = USART_StopBits_1;
 USART_InitStructure.USART_Parity = USART_Parity_No;
 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
 USART_Init(PRINT_USART, &USART_InitStructure);
 USART_Cmd(PRINT_USART, ENABLE);
 
}

void host_platformInit(void)
{
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock/1000);	
  Print_USART_Init();
}

void WIFI_SPI_Init(void) 
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

  /*!< Enable the SPI clock */
  WIFI_SPI_CLK_INIT(WIFI_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(WIFI_SPI_SCK_GPIO_CLK | WIFI_SPI_MISO_GPIO_CLK | 
                         WIFI_SPI_MOSI_GPIO_CLK , ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF */  
  GPIO_PinAFConfig(WIFI_SPI_SCK_GPIO_PORT, WIFI_SPI_SCK_SOURCE, WIFI_SPI_SCK_AF);
  GPIO_PinAFConfig(WIFI_SPI_MISO_GPIO_PORT, WIFI_SPI_MISO_SOURCE, WIFI_SPI_MISO_AF);
  GPIO_PinAFConfig(WIFI_SPI_MOSI_GPIO_PORT, WIFI_SPI_MOSI_SOURCE, WIFI_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
        
  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = WIFI_SPI_SCK_PIN;
  GPIO_Init(WIFI_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  WIFI_SPI_MOSI_PIN;
  GPIO_Init(WIFI_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  WIFI_SPI_MISO_PIN;
  GPIO_Init(WIFI_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	
	
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(WIFI_SPI, &SPI_InitStructure);

  /*!< Enable the WIFI_SPI  */
  SPI_Cmd(WIFI_SPI, ENABLE);
	
}

void WIFI_INT_Init(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOB clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* Configure PB0 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = WIFI_INT_PIN;
  GPIO_Init(WIFI_INT_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable SYSCFG clock */
  /* Connect EXTI Line0 to PB0 pin */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(WIFI_INT_GPIO_EXTI_PORT, WIFI_INT_EXTI_PIN_SOURCE);

  /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = WIFI_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = WIFI_INT_EXTI_IRQN;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

void WIFI_SPI_Deinit(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the WIFI_SPI  ************************************************/
  SPI_Cmd(WIFI_SPI, DISABLE);
  
  /*!< DeInitializes the WIFI_SPI *******************************************/
  SPI_I2S_DeInit(WIFI_SPI);
  
  /*!< WIFI_SPI Periph clock disable ****************************************/
  WIFI_SPI_CLK_INIT(WIFI_SPI_CLK, DISABLE);
      
  /*!< Configure all pins used by the SPI as input floating *******************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = WIFI_SPI_SCK_PIN;
  GPIO_Init(WIFI_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = WIFI_SPI_MISO_PIN;
  GPIO_Init(WIFI_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = WIFI_SPI_MOSI_PIN;
  GPIO_Init(WIFI_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = WIFI_CS_PIN;
  GPIO_Init(WIFI_CS_GPIO_PORT, &GPIO_InitStructure);
  
  EXTI_DeInit();
}

