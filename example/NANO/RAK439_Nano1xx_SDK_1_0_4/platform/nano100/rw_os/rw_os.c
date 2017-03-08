/**
  ******************************************************************************
  * @file    rw_os.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module ucosIII OS Entity filling C File.
  *
  *          This file contains:
  *           - ucosIII OS Entity filling include task,mutex,sem operations
  * 
  ******************************************************************************
**/
#include "os.h"
#include "rw_os.h"
#include "lib_mem.h"

#define RW_DRIVER_TASK_PRIO  4
#define RW_DRIVER_TASK_STACK_SIZE (800>>2)


void* rw_creat_task(RW_OS_TASK_PTR p_task)
{
    OS_TCB* p_tcb;
    OS_ERR oserr;
    CPU_STK* p_stack;
    
    p_tcb = pvPortMalloc(sizeof(OS_TCB));
    if (p_tcb == NULL) {
        while(1);
    }
    
    p_stack = pvPortMalloc(RW_DRIVER_TASK_STACK_SIZE * 4);
    if (p_stack == NULL) {
        while(1);
    }    
    
    OSTaskCreate(p_tcb,                                        // TCB
                 "task",                                        // name
                 p_task,                                         // func
                 NULL,                                            // func(arg)
                 RW_DRIVER_TASK_PRIO,                         // priority
                 p_stack,                            // stack
                 RW_DRIVER_TASK_STACK_SIZE/10,                   // stack lim
                 RW_DRIVER_TASK_STACK_SIZE,                     // stack sz
                 0,                                               // msq Q
                 1000,                                               // time quanta
                 NULL,                                            // ext ptr
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),     // opt(s)
                 &oserr); 
    
    if (oserr != OS_ERR_NONE) {
        return NULL;  
    } 
    return (void*)p_tcb;
    
}

int rw_del_task(void* p_tcb)
{
    OS_ERR oserr;
    void* stack;
    
    stack = ((OS_TCB*)p_tcb)->StkBasePtr;
    OSTaskDel(p_tcb, &oserr);
    vPortFree(p_tcb);
    vPortFree(stack);
    return RW_OS_OK;

}

void* rw_creat_mutex(void)
{
  OS_ERR oserr;
  OS_MUTEX* p_mutex;
  
  p_mutex = pvPortMalloc(sizeof(OS_MUTEX));
  if (p_mutex == NULL) {
      while(1);
  }
  
  OSMutexCreate(p_mutex, "mutex", &oserr);
  if(oserr != OS_ERR_NONE){
     return NULL;
  }
  return (void*)p_mutex;
}

int rw_del_mutex(void* p_mutex)
{
  OS_ERR oserr;
  
  OSMutexDel(p_mutex, OS_OPT_DEL_ALWAYS, &oserr);
  vPortFree(p_mutex);
  return RW_OS_OK;
}

int rw_lock_mutex(void* p_mutex, uint32_t timeout)
{
  OS_ERR oserr;
  
  OSMutexPend(p_mutex, timeout, OS_OPT_PEND_BLOCKING, NULL, &oserr);
  return RW_OS_OK;
}

int rw_unlock_mutex(void* p_mutex)
{
  OS_ERR oserr;  
  OSMutexPost(p_mutex, OS_OPT_POST_NONE, &oserr);
  return RW_OS_OK;
}

void* rw_creat_sem(void)
{
    OS_ERR oserr;
    OS_SEM* p_sem;
    
    p_sem = pvPortMalloc(sizeof(OS_SEM));
    if (p_sem == NULL) {
        while(1);
    }
    OSSemCreate(p_sem, "task", 0, &oserr);
    return p_sem;
}

int rw_del_sem(void* p_sem)
{
    OS_ERR oserr;
    OSSemDel(p_sem, OS_OPT_DEL_ALWAYS,&oserr);
    vPortFree(p_sem);
    return RW_OS_OK;
}

int rw_post_sem(void* p_sem)
{
    OS_ERR oserr;
    OS_SemPost(p_sem, OS_OPT_POST_1, 0, &oserr);
    return RW_OS_OK;
}

int rw_pend_sem(void* p_sem, uint32_t timeout)
{
    OS_ERR oserr;
    OSSemPend(p_sem, timeout, OS_OPT_PEND_BLOCKING, NULL, &oserr);
    if (oserr == OS_ERR_NONE) {
        return RW_OS_OK;     
    } else if (oserr == OS_ERR_TIMEOUT) {
        return RW_OS_TIME_OUT;
    }
    return RW_OS_ERROR;
}

void delay_ms(int count)
{
    OS_ERR oserr;
    OSTimeDlyHMSM(0,0,0,count, OS_OPT_TIME_HMSM_NON_STRICT, &oserr);    
}






