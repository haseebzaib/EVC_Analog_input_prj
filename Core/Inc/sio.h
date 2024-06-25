/*
 * sio.h
 *
 *  Created on: Feb 14, 2024
 *      Author: admin
 */

#ifndef INC_SIO_H_
#define INC_SIO_H_

/************************************************************************/
/*Author: Haseeb Zaib         Program: Analog Inputs for EVC            */
/*Ver: 1                      Date: Feb 14, 2024                        */
/*   Uart interface file with EVC(Electronic Volume Corrector)          */
/************************************************************************/


#include "main.h"

#define tx_buf 500
#define rx_buf 500

/*** ASCII-Codes ***/
#define _LF  0x0A
#define _SOH 0x01

struct SioStruct {

	uint8_t *rx;  //received interrupt message
	uint8_t *tx;   //transmit interrupt message
	uint32_t txSize; //size of transmit data
    uint8_t Echar; //end character to end the transmition
    uint8_t Echar_rx; //received character to compare with Echar
};

extern uint8_t S0TxBuf[];
extern uint8_t S0RxBuf[];

#define Analog_I_Uart huart1

extern __IO struct SioStruct uart_control_block;


extern void sio_task();
extern void sio_init();


#endif /* INC_SIO_H_ */
