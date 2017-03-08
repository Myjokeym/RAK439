/**
  ******************************************************************************
  * @file    rw_os.h
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module FreeRtos OS Entity filling Header File.
  *
  *          This file contains:
  *           - FreeRtos OS Entity filling include task,mutex,sem operations
  * 
  ******************************************************************************
**/
#ifndef _RW_OS_H
#define _RW_OS_H

#include <stdint.h>

#define RW_OS_OK             0
#define RW_OS_ERROR         -1
#define RW_OS_TIME_OUT      -2

typedef  void (*RW_OS_TASK_PTR)(void *p_arg);

void* rw_creat_task(RW_OS_TASK_PTR p_task);
int rw_del_task(void* p_tcb);

void* rw_creat_mutex(void);
int rw_del_mutex(void* p_mutex);
int rw_lock_mutex(void* p_mutex, uint32_t timeout);
int rw_unlock_mutex(void* p_mutex);

void* rw_creat_sem(void);
int rw_del_sem(void* p_sem);
int rw_post_sem(void* p_sem);
int rw_pend_sem(void* p_sem, uint32_t timeout);
#endif
