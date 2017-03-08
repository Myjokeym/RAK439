/**
  ******************************************************************************
  * @file    rw_error.h
  * @author  RAK439 module Design Team
  * @version V1.0.4
  * @date    13-Jun-2015
  * @brief   RAK439 module error code list Header File.
  *
  *          This file contains:
  *           - Macros to define the return error code from the driver lib apis
  * 
  ******************************************************************************
**/
#ifndef _RW_ERROR_H_
#define _RW_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RW_OK                           (0)    //DONE
#define RW_ERR                          (-1)

/* define all error */
#define RW_ERR_PARAM_INVAILD            (-2)
#define RW_ERR_INIT_DRIVER_FAILD        (-3)
#define RW_ERR_DEINIT_DRIVER_FAILD      (-4)
#define RW_ERR_NO_DRIVER                (-5)
#define RW_ERR_NO_MEMORY                (-6)

#define RW_ERR_INVAILD_SSID             (-7)
#define RW_ERR_INVAILD_PSK              (-8)
#define RW_ERR_INVAILD_CHANNEL          (-9)
#define RW_ERR_INVAILD_SEC_MODE         (-10)    
#define RW_ERR_INVAILD_AUTH_MODE        (-11) 
#define RW_ERR_INVAILD_ROLE_MODE        (-12)
#define RW_ERR_INVAILD_CONFIG_MODE      (-13)  
    
#define RW_ERR_CMD_PENDING              (-14)
#define RW_ERR_NO_CONNECT               (-15)
#define RW_ERR_NOT_FIND_SSID            (-16)
#define RW_ERR_IP_DHCP                  (-17)

#define RW_ERR_SOCKET_INVAILD           (-18)
#define RW_ERR_SEND_BUFFER_FULL         (-19)
#define RW_ERR_TIME_OUT                 (-20)

#endif

