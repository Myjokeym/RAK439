/**
******************************************************************************
* File Name          : freertos.c
* Description        : Code for freertos applications
******************************************************************************
*
* COPYRIGHT(c) 2016 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "rw_app.h"
#include "tim.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId appTaskHandle;
osThreadId clientTaskHandle;
osThreadId mallocTaskHandle;
osMutexId buf_mutexHandle;
#define SIZE 53

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartAppTask(void const * argument);
void StartClientTask(void const * argument);
void StartMallocTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/************************************
// Method:    platform_init
// Date:  	  2016/06/27
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: void
// Function:  配置RAK439参数和初始化驱动 获取RAK439库版本与物理地址 
************************************/
int platform_init(void)
{
    rw_DriverParams_t     params;
    int                   ret =0;
    char                  libVersion[20]="";
    char                  module_mac[6] ="";

    /*RAK439用户参数配置*/
    wifi_init_params(&params);

    /*RAK439驱动初始化*/
    ret =rw_sysDriverInit(&params);
    DPRINTF("rw_sysDriverInit ret:%d\r\n",ret);
    if(ret != RW_OK)
    {
        DPRINTF("RAK module platform init...failed\r\n");
        while(1);
    }

    /*获取RAK439库版本*/
    rw_getLibVersion(libVersion); 
    DPRINTF("rak wifi LibVersion:%s\r\n", libVersion);

    /*获取RAK439物理地址*/
    rw_getMacAddr(module_mac);
    DPRINTF("rak wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n", module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);

    return RW_OK;
}

/************************************
// Method:    get_xtask_state
// Date:  	  2016/06/27
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
// Function:  获取每个任务的状态信息
************************************/
void get_xtask_state(void)
{
    char tmp[128] = "";
    int res_getstate = 0;
    const char task_state[]={'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, index;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;  
    TaskStatus_t *pxTaskStatusArray = NULL;

    /*获取任务数目，至少有一个任务（该任务为freertos的IDLE空闲任务）*/
    uxArraySize = uxTaskGetNumberOfTasks(); 
    assert_param(uxArraySize>0);

    /*为任务信息变量分配内存*/
    pxTaskStatusArray = pvPortMalloc(sizeof(TaskStatus_t)*uxArraySize);
    assert_param(pxTaskStatusArray);

    /*获取任务信息*/
    res_getstate = uxTaskGetSystemState(pxTaskStatusArray,uxArraySize,&ulTotalRunTime);
    assert_param(res_getstate);

    //DPRINTF("********************任务信息统计表*********************\r\n\r\n");
    //DPRINTF("任务名    状态    ID  优先级   堆栈  CPU使用率\r\n");
    if (ulTotalRunTime>0)
    {
        for (index=0; index<uxArraySize; index++)
        {
            ulStatsAsPercentage = (uint64_t)(pxTaskStatusArray[index].ulRunTimeCounter)*100 / ulTotalRunTime;
            if (ulStatsAsPercentage > 0UL)
            {
                sprintf(tmp,"%-12s%-6c%-6ld%-8ld%-8d%d%%",
                    pxTaskStatusArray[index].pcTaskName,
                    task_state[pxTaskStatusArray[index].eCurrentState],  
                    pxTaskStatusArray[index].xTaskNumber,
                    pxTaskStatusArray[index].uxCurrentPriority,  
                    pxTaskStatusArray[index].usStackHighWaterMark,
                    ulStatsAsPercentage);
            }
            else
            {
                sprintf(tmp,"%-12s%-6c%-6ld%-8ld%-8dt<1%%",
                    pxTaskStatusArray[index].pcTaskName,
                    task_state[pxTaskStatusArray[index].eCurrentState],  
                    pxTaskStatusArray[index].xTaskNumber,
                    pxTaskStatusArray[index].uxCurrentPriority,  
                    pxTaskStatusArray[index].usStackHighWaterMark); 
            }
            //DPRINTF("%s\r\n",tmp);
        }
    }
    //DPRINTF("任务状态:   r-运行  R-就绪  B-阻塞  S-挂起  D-删除\r\n"); 
    DPRINTF("内存剩余量:%d Byte\r\n\r\n",xPortGetFreeHeapSize());

    /*释放内存*/
    vPortFree(pxTaskStatusArray);
}



/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
void configureTimerForRunTimeStats(void)
{
    HAL_TIM_Base_Start_IT(&htim3);
}

unsigned long getRunTimeCounterValue(void)
{
    /*返回定时器3中断处理函数的变量*/
    return Tim3Counter;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
    DPRINTF("%s StackOverFlow!\r\n",pcTaskName);
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created. It is also called by various parts of the
    demo application. If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    DPRINTF("vApplicationMallocFailedHook!\r\n");
}
/* USER CODE END 5 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */
    /* USER CODE END Init */
	
    /* Create the mutex(es) */
    /* definition and creation of buf_mutex */
    osMutexDef(buf_mutex);
    buf_mutexHandle = osMutexCreate(osMutex(buf_mutex));

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* definition and creation of appTask */
    osThreadDef(appTask, StartAppTask, osPriorityAboveNormal, 0, 256);
    appTaskHandle = osThreadCreate(osThread(appTask), NULL);

    /* definition and creation of clientTask */
    //osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 256);
    //clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);


    /* definition and creation of mallocTask */
    //osThreadDef(mallocTask, StartMallocTask, osPriorityNormal, 0, 128);
    //mallocTaskHandle = osThreadCreate(osThread(mallocTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */

    /*每隔5秒获取一次每个任务的状态信息*/
    for(;;)
    {
        get_xtask_state();
        osDelay(5000);
    }
    /* USER CODE END StartDefaultTask */
}

/* StartAppTask function */
/************************************
// Method:    StartAppTask
// Date:  	  2016/07/04
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void const * argument
// Function:  此任务用于检测RAK439连接路由器的状态 从而来控制clientTask任务
************************************/
void StartAppTask(void const * argument)
{
    /* USER CODE BEGIN StartAppTask */

    int ret = RW_OK;
    /*RAK439驱动初始化*/
    platform_init();

    /*初始化app变量*/
    rw_appdemo_context_init();

    /*连接路由器*/ 
    //此功能函数会阻塞一段时间
    ret = rw_network_startSTA();
    osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 2048);
    clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

    /* Infinite loop */
    for(;;)
    {
        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
            DPRINTF("reconnect and ipquery...\r\n");
            /*删除clientTask*/
            //osThreadTerminate(clientTaskHandle);
            //rw_appdemo_context_init();   
            //rw_sysDriverReset();
            //也可以直接调用rw_network_startSTA()实现路由器重连功能
            //rw_network_startSTA();
            //osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 256);
            //clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

        }
        rw_sysSleep(100);

    }
    /* USER CODE END StartAppTask */
}

/* StartClientTask function */

void StartClientTask(void const * argument)
{
    /* USER CODE BEGIN StartClientTask */
    int      ret = 0;
    char str[SIZE];
		memset(str, 0, sizeof(str));
		int tmpIndex = 0;
		for (int i = 0; i < SIZE; ++i)
		{
			str[i] = ('0' + tmpIndex);
			++tmpIndex;
			if (tmpIndex == 10)
			{
				tmpIndex = 0;
			}
		}
		str[SIZE - 3] = '\r';
		str[SIZE - 2] = '\n';
		str[SIZE - 1] = '\0';
	  DPRINTF("str:%s\r\n",str);
		char revBuff[SIZE];
		memset(revBuff,0,SIZE);
    /* Infinite loop */
    for(;;)
    {      
reconnect: 
        //
        if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
        {
            DPRINTF("未连接服务器\r\n");
            rw_sysSleep(1000);
            goto reconnect;      
        }


        if(app_demo_ctx.rw_connect_status == STATUS_OK && app_demo_ctx.rw_ipquery_status == STATUS_OK) 
        {
            if (app_demo_ctx.tcp_cloud_sockfd == INVAILD_SOCK_FD)
            {
                if((ret =RAK_TcpClient(8000, 0xC0A80469)) >= 0)
                {
                    app_demo_ctx.tcp_cloud_sockfd = ret;
                    app_demo_ctx.tcp_cloud_status = STATUS_OK;
                    DPRINTF("RAK_TcpClient sockfd = %u creat\r\n",app_demo_ctx.tcp_cloud_sockfd);
										DPRINTF("SEND START***************************************\r\n\r\n");
                    ret = send(app_demo_ctx.tcp_cloud_sockfd,str,strlen(str),0);
										DPRINTF("SEND End***************************************\r\n\r\n");
										DPRINTF("send ret1:%d\r\n",ret);
									
										ret = send(app_demo_ctx.tcp_cloud_sockfd,str,strlen(str),0);
										DPRINTF("send ret2:%d\r\n",ret);
										
									  recv(app_demo_ctx.tcp_cloud_sockfd,revBuff,sizeof(revBuff),0);
										DPRINTF("revFromServer:%s\r\n",revBuff);
									  
									  close(app_demo_ctx.tcp_cloud_sockfd);
                    app_demo_ctx.tcp_cloud_status = STATUS_INIT;
                    app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
										break;

                }
                else
                {
                    if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
                    { 
                        DPRINTF("RAK_TcpClient creat failed code=%d\r\n", ret);
                        rw_sysSleep(100);
                        goto reconnect;
                    }
                }    
            }
        }
        rw_sysSleep(1000);
    }
    /* USER CODE END StartClientTask */
}

/* StartMallocTask function */
void StartMallocTask(void const * argument)
{
    /* USER CODE BEGIN StartMallocTask */
    uint8_t *test_mallocbuff = NULL;
    uint32_t MallocNum = 1024;
    uint8_t test_value = 1;
    uint32_t index = 0,Sum = 0;

    /*输出任务名字*/
    DPRINTF("%s is Run\r\n",pcTaskGetTaskName(NULL));

    /* Infinite loop */
    for(;;)
    {
        /*分配内存并赋初值*/
        test_mallocbuff = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*MallocNum);
        memset(test_mallocbuff,test_value,MallocNum);

        /*求和*/
        Sum = 0;
        for (index = 0; index < MallocNum; index++)
        {
            Sum += *(test_mallocbuff+index);
        }
        assert_param(Sum == (test_value*MallocNum));
        vPortFree(test_mallocbuff);
        osDelay(50);
    }
    /* USER CODE END StartMallocTask */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
