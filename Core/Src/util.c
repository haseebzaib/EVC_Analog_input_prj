/*
 * util.c
 *
 *  Created on: Feb 14, 2024
 *      Author: Admin
 */



/************************************************************************/
/*Author: Haseeb Zaib         Program: Analog Inputs for EVC            */
/*Ver: 1                      Date: Feb 14, 2024                        */
/*  Utility File, system independent                                    */
/************************************************************************/

#include "util.h"
#include "main.h"

GLOBAL __IO uint8_t uart_event = 0;
LOCAL uint32_t prev_time = 0;






/*
 *Checks for uart event for set time
 *time is in ms
 */
GLOBAL void util_check_UartEvent(uint32_t time_ms)
{

	switch(time_ms)
	{

	case 0:
	{
		 while(util_UartEvent_Nequal)  //just wait for event dont perform anything else
		 {

		 }
			 break;
	}

	default:
	 {
		 prev_time = HAL_GetTick(); //we get new time before going into while
		 while(util_UartEvent_Nequal && (HAL_GetTick() - prev_time < time_ms)); //wait here until get event or time passes
	     break;
	 }


	}

}
