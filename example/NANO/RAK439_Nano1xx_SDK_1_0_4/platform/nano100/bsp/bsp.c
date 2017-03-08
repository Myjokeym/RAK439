#include "bsp.h"

uint32_t __IO u32Timer0Cnt=0;
static void set_sys_clock(void);
extern void PDMA_DRIVER_HANDLE(void* cxt);

#if   defined ( __CC_ARM )

  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

  /**
    * @brief  Retargets the C library printf function to the USART.
    * @param  None
    * @retval None
    */
  PUTCHAR_PROTOTYPE
  {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    UART_TxByte(PRINT_UART, ch);

    return ch;
  }
#endif
	
void TMR0_IRQHandler(void)
{
	TIMER0->ISR = 3;
	u32Timer0Cnt++;
}

void SYS_Config(void)
{
    UNLOCKREG();
    set_sys_clock();
    LOCKREG();
}

void Print_UART_Init(void)
{
    STR_UART_T sParam = DEFAULT_VALUE;
    /* Set UART0 Pin */
    MFP_UART0_TO_PORTB();
    UART_Init(PRINT_UART, &sParam);
}

void TIMER_Tick_Config(void)
{
  TIMER_Init(TIMER0, 11, 1000, TIMER_CTL_MODESEL_PERIODIC);
  /* Enable TIMER0 Intettupt */
  TIMER_EnableInt(TIMER0, TIMER_IER_TMRIE);
  /* Start counting */
  u32Timer0Cnt = 0;
  TIMER_Start(TIMER0);
  
}

void host_platformInit(void)
{
   SYS_Config();
   Print_UART_Init();	
   TIMER_Tick_Config();
}

void WIFI_SPI_Init(void)
{
    SPI_DATA_T conf;

    uint32_t clksel = CLK->CLKSEL0 & CLK_CLKSEL0_HCLK_MASK;;

    SPI_Init(WIFI_SPI);

    GCR->PC_L_MFP = GCR->PC_L_MFP & ~(PC0_MFP_MASK|PC1_MFP_MASK|PC2_MFP_MASK|PC3_MFP_MASK) |
        PC0_MFP_SPI0_SS0 | PC1_MFP_SPI0_SCLK | PC2_MFP_SPI0_MISO0 | PC3_MFP_SPI0_MOSI0;

    /* Configure SPI0 as a master, 8-bit transaction*/
    conf.u32Mode = SPI_MODE_MASTER;
    /*Clock high idle, sample on rising edge.*/
    conf.u32Type = SPI_TYPE2;
    conf.i32BitLength = 8;	
    SPI_Open(WIFI_SPI, &conf);

    /* Diable AutoSS */
    SPI_DisableAutoSS(WIFI_SPI);
    /* Set the active level of slave select. */
    SPI_SetSlaveSelectActiveLow(WIFI_SPI);
    /* SPI clock rate 12MHz */

    if (clksel == CLK_CLKSEL0_HCLK_HIRC)
        SPI_SetClockFreq(WIFI_SPI, 4000000, 0);
    else
        SPI_SetClockFreq(WIFI_SPI, 16000000, 0);

    /* Enable FIFO mode */
    SPI_SetFIFOMode(WIFI_SPI, TRUE, 2);

    /* Clear Tx register of WIFI_SPI to avoid send non-zero data to Master. Just for safe. */
    SPI_ClearTxFIFO(WIFI_SPI);
    SPI_ClearRxFIFO(WIFI_SPI);
    while(WIFI_SPI->CTL & SPI_CTL_GO_BUSY);
}

void WIFI_INT_Init(void)
{
    /* Configure external interrupt */
    MFP_EXT_INT1_TO_PB15();
    /* Set falling edge interrupt and clear/enable it */
    GPIO_EnableEINT1(WIFI_PORT_INT, WIFI_PIN_INT, GPIO_IER_IF_EN_15, GPIO_IMD_EDGE_15);
}

void WIFI_GPIO_Init(void)
{
    GPIO_SetBit(WIFI_PORT_FET, WIFI_PIN_FET);
    GPIO_Open(WIFI_PORT_FET, GPIO_PMD_PMD0_OUTPUT, GPIO_PMD_PMD0_MASK);
    GPIO_ClrBit(WIFI_PORT_PWD, WIFI_PIN_PWD);
    GPIO_Open(WIFI_PORT_PWD, GPIO_PMD_PMD5_OUTPUT, GPIO_PMD_PMD5_MASK);
}

void WIFI_SPI_Deinit(void)
{
    SPI_Close(WIFI_SPI);
    SPI_DeInit(WIFI_SPI); 
}

static void set_sys_clock(void)
{
    S_SYS_CHIP_CLKCFG sChipClkCfg;

    uint32_t clksel = CLK->CLKSEL0 & CLK_CLKSEL0_HCLK_MASK;

     /* Init Chip clock source and IP clocks */
     memset(&sChipClkCfg, 0, sizeof(sChipClkCfg));
     sChipClkCfg.u32ChipClkEn = CLK_PWRCTL_HXT_EN;
     sChipClkCfg.u32PLLClkSrc = CLK_PLLCTL_PLLSRC_HXT;
     sChipClkCfg.u8PLLEnable  = ENABLE;   /* enable PLL */
     sChipClkCfg.ePLLInFreq   = E_SYS_PLLIN_12M;
     sChipClkCfg.ePLLOutFreq  = E_SYS_PLLOUT_120M;
     sChipClkCfg.u32HClkDiv   = HCLK_CLK_DIVIDER(2);    /* HCLK = 120/(2+1) = 40 MHz */
     sChipClkCfg.u32HClkSrc = CLK_CLKSEL0_HCLK_PLL;
      /* IPs clock setting */
     SYS_InitChipClock(&sChipClkCfg);

    /* Update CPU Clock Frequency */
    SystemCoreClockUpdate();

}

int Print_UART_TxBuf(uint8_t *buffer, int nbytes)
{
    for (int i = 0; i < nbytes; i++)
      UART_TxByte(PRINT_UART, *buffer++);
    
    return nbytes;
}

void UART_TxByte(UART_TypeDef  *tUART, uint8_t data)
{
    while (!(tUART->FSR & UART_FSR_TX_EMPTY_F));
    tUART->THR = data;
}


void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while(1);

}





