/**
  ******************************************************************************
  * @file    rw_os.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module FreeRtos OS Entity filling C File.
  *
  *          This file contains:
  *           - FreeRtos OS Entity filling include task,mutex,sem operations
  * 
  ******************************************************************************
**/
#include "rw_os.h"
#include "cmsis_os.h"

#define RW_DRIVER_TASK_PRIO  (configMAX_PRIORITIES - 3) 
#define RW_DRIVER_TASK_STACK_SIZE (800>>2)


void* rw_creat_task(RW_OS_TASK_PTR p_task)
{	
  osThreadId p_tcb;
  osThreadDef(task_wifi, (os_pthread)p_task, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 7);
  p_tcb = osThreadCreate (osThread(task_wifi), NULL);
  return p_tcb;
}

int rw_del_task(void* p_tcb)
{
  osThreadTerminate(p_tcb);
  return RW_OS_OK;
  
}

static osMutexDef(mutex);

void* rw_creat_mutex(void)
{
  osMutexId p_mutex;
  p_mutex = osMutexCreate(osMutex(mutex));
  return (void *)p_mutex;
}

int rw_del_mutex(void* p_mutex)
{
  osMutexDelete(p_mutex);
  return RW_OS_OK;
}

int rw_lock_mutex(void* p_mutex, uint32_t timeout)
{
  if (timeout ==0) {  //wait forever
    timeout = osWaitForever;
  }
  osMutexWait(p_mutex,timeout);
  return RW_OS_OK;
}

int rw_unlock_mutex(void* p_mutex)
{
  osMutexRelease(p_mutex);
  return RW_OS_OK;
}

static osSemaphoreDef(sem);

void* rw_creat_sem(void)
{
  osSemaphoreId p_sem;
  p_sem = osSemaphoreCreate(osSemaphore(sem),1);
  return p_sem;
}

int rw_del_sem(void* p_sem)
{
  osSemaphoreDelete(p_sem);
  return RW_OS_OK;
}

int rw_post_sem(void* p_sem)
{
  osSemaphoreRelease(p_sem);
  return RW_OS_OK;
}

int rw_pend_sem(void* p_sem, uint32_t timeout)
{
   int32_t oserr;
    
   if (timeout ==0) {  //wait forever
      timeout = osWaitForever;
    }
    oserr =osSemaphoreWait(p_sem,timeout);  
    if (oserr == osOK) {
        return RW_OS_OK;     
    } else if (oserr == osErrorOS) {
        return RW_OS_TIME_OUT;
    }
    return RW_OS_ERROR;
}








