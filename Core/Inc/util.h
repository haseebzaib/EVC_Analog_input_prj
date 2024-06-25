/*
 * util.h
 *
 *  Created on: Feb 14, 2024
 *      Author: Admin
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_


/************************************************************************/
/*Author: Haseeb Zaib         Program: Analog Inputs for EVC            */
/*Ver: 1                      Date: Feb 14, 2024                        */
/*  Utility File, system independent                                    */
/************************************************************************/

#include "main.h"


extern __IO uint8_t uart_event;

#define util_UartEvent_Nequal uart_event != 1
#define util_resUartEvent()  uart_event = 0
#define util_setUartEvent()  uart_event = 1




extern void util_check_UartEvent(uint32_t time_ms);





#endif /* INC_UTIL_H_ */
