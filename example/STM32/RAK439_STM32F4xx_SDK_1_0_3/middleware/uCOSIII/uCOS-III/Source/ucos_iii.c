/*
*********************************************************************************************************
*                                                uC/OS-III
*                                          The Real-Time Kernel
*
*                              (c) Copyright 1992-2010, Micrium, Weston, FL
*                                           All Rights Reserved
*
* File    : uCOS_II.C
* By      : Jean J. Labrosse
* Version : V2.92
*
* LICENSING TERMS:
* ---------------
*   uC/OS-II is provided in source form for FREE evaluation, for educational use or for peaceful research.  
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micriµm to properly license 
* its use in your product. We provide ALL the source code for your convenience and to help you experience 
* uC/OS-II.   The fact that the  source is provided does  NOT  mean that you can use it without  paying a 
* licensing fee.
*********************************************************************************************************
*/

#define  OS_GLOBALS                           /* Declare GLOBAL variables                              */
#include <os.h>


#define  OS_MASTER_FILE                       /* Prevent the following files from including includes.h */
#include <os_core.c>
#include <os_dbg.c>
#include <os_flag.c>
#include <os_int.c>
#include <os_mem.c>
#include <os_msg.c>
#include <os_mutex.c>
#include <os_pend_multi.c>
#include <os_prio.c>
#include <os_q.c>
#include <os_sem.c>
#include <os_stat.c>
#include <os_task.c>
#include <os_tick.c>
#include <os_time.c>
#include <os_tmr.c>
#include <os_var.c>
#include <os_cfg_app.c>
