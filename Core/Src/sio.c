/*
 * sio.c
 *
 *  Created on: Feb 14, 2024
 *      Author: Admin
 */

/************************************************************************/
/*Author: Haseeb Zaib         Program: Analog Inputs for EVC            */
/*Ver: 1                      Date: Feb 14, 2024                        */
/*   Uart interface file with EVC(Electronic Volume Corrector)          */
/************************************************************************/

#include "sio.h"
#include "main.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "util.h"
#include "ads8344.h"
#include "stdlib.h"

#define N_DECIMAL_POINTS_PRECISION (100000)

//char* floatToString(float f, char* S, size_t n, int digitsAfterDP) {
//  if (digitsAfterDP == 0)
//    snprintf(S, n, "%d", (int) (f + (f < 0 ? -0.5 : 0.5)));
//  else if (digitsAfterDP < 0) {
//    int i;
//    for (i = 0; i < -digitsAfterDP && abs(f) >= 10; i++) f /= 10;
//    char fmt[20]; // "%d%02d"
//    sprintf(fmt, "%%d%%0%dd", i);
//    snprintf(S, n, fmt, (int) (f + (f < 0 ? -0.5 : 0.5)), 0);
//  } else {
//    int M = (int) f;
//    f = abs(f - (float) M);
//    for (int i = digitsAfterDP; i > 0; i--) f *= 10;
//    int E = (int) (f + 0.5);
//    char fmt[20]; // "%d.%05d"
//    sprintf(fmt, "%%d.%%0%dd", digitsAfterDP);
//    snprintf(S, n, fmt, M, E);
//  }
//  return(S);
//}
/*Variables*/
LOCAL uint8_t *pRx; //Pointer to examine RX buffer
GLOBAL uint8_t S0TxBuf[120]; /* Send buffer to EVC*/
GLOBAL uint8_t S0RxBuf[50]; /* Receieve buffer from EVC */
uint8_t opRq_buf[] = "senddata\n";
float ad_val[ANALOG_INP];
uint8_t ad_buf[ANALOG_INP][25];
char Tx_buffer[120];
uint8_t Err;

__IO struct SioStruct uart_control_block;

/*Function Prototypes*/
LOCAL void sio_RxIntS0(UART_HandleTypeDef *huart);
//LOCAL void sio_TxIntS0 (UART_HandleTypeDef *huart);
LOCAL void sio_RxBlockStart();
LOCAL void sio_TxBlockStart(char *buf);

LOCAL uint16_t sio_uart_rxRec(UART_HandleTypeDef *huart) {
	uint16_t uhErr = (uint16_t) huart->ErrorCode;
	uint16_t uhData;

	if (huart->RxState == HAL_UART_STATE_BUSY_RX) {
		/* uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
		 if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
		 {
		 tmp = (uint16_t*) huart->pRxBuffPtr ;
		 *tmp = (uint16_t)(uhdata & uhMask);
		 // huart->pRxBuffPtr +=2;
		 }
		 else
		 {
		 // *huart->pRxBuffPtr++ = (uint8_t)(uhdata & (uint8_t)uhMask);
		 *huart->pRxBuffPtr = (uint8_t)(uhdata & (uint8_t)uhMask);
		 }*/
		uhData = ((uint16_t) READ_REG(huart->Instance->RDR)) & huart->Mask;

		if (uhErr != HAL_UART_ERROR_NONE)
			uhData |= (uhErr << 8);

		return (uhData);	// HAL_OK
	} else {
		/* Clear RXNE interrupt flag */
		__HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

		return (0);	// HAL_BUSY
	}
}

/*
 * Create Receiving block for uart communication
 */
LOCAL void sio_RxBlockStart() {
	util_resUartEvent();
	memset(S0RxBuf, 0, sizeof(S0RxBuf));

	uart_control_block.rx = S0RxBuf;
	uart_control_block.Echar = _LF;

	HAL_UART_Receive_IT(&Analog_I_Uart, uart_control_block.rx, 1);

	Analog_I_Uart.RxISR = &sio_RxIntS0;

}

/*
 * Create transmittion  block for uart communication
 */
LOCAL void sio_TxBlockStart(char *buf) {
	util_resUartEvent();
	memset(S0TxBuf, 0, sizeof(S0TxBuf));

	sprintf((char*) S0TxBuf, "%s", buf);

	uart_control_block.tx = S0TxBuf;
	uart_control_block.txSize = strlen((const char*) S0TxBuf);

	HAL_UART_Transmit_IT(&Analog_I_Uart, uart_control_block.tx,
			uart_control_block.txSize);

}

/*
 * Opening request prompt from EVC
 * we make request buffer here
 */
LOCAL void sio_opRq() {
//	if (*pRx++ != '?')		// after / must ? come
//			{
//		return;
//	}

//	*pRx++;
//	*pRx++;
//
//	if (memcmp(pRx, opRq_buf, strlen((const char*) opRq_buf)) == 0) {

		for (int i = 0; i < ANALOG_INP; i++) {

			//for (int j = 0; j < 2; j++) {
				Err = ani_Mess(i, &ad_val[i]);

				if (Err) {
					sprintf((char*) ad_buf[i], "%s", "ERR"); //error
				} else {
//				int integerPart = (int)ad_val[i];
//				int decimalPart = ((int)(ad_val[i]*N_DECIMAL_POINTS_PRECISION)%N_DECIMAL_POINTS_PRECISION);
//				sprintf((char *)ad_buf[i], "%d.%04d",integerPart,decimalPart);//cant use float due to memory constrains
					sprintf((char*) ad_buf[i], "%.5f", ad_val[i]);
					//floatToString(ad_val[i],(char *)ad_buf[i],25,6);
				}

			//}

		}

		sprintf(Tx_buffer, "\x01IN1:%s,IN2:%s,IN3:%s,IN4:%s!\x0A",
				(char*) ad_buf[0], (char*) ad_buf[1], (char*) ad_buf[2],
				(char*) ad_buf[3]);

		sio_TxBlockStart(Tx_buffer);
		util_check_UartEvent(2000);

//	} else {
//		return;
//	}

}

/*
 * Main task for handling all the communication related to uart
 */
GLOBAL void sio_task() {
	sio_init();
	sio_RxBlockStart();
	util_check_UartEvent(50000);   //we wait here until we receive something
//	sio_TxBlockStart((char *)S0RxBuf);
//	util_check_UartEvent(2000);

//	pRx = S0RxBuf;
//
//	switch (*pRx++) //for future more cases can be added if needed
//	{
//	case '/': //opening request
//	{
//		sio_opRq();
//		break;
//	}
//	default: {
//		//sio_opRq();
//		HAL_UART_DeInit(&huart1);
//		huart1.Instance = USART1;
//		huart1.Init.BaudRate = 9600;
//		huart1.Init.WordLength = UART_WORDLENGTH_8B;
//		huart1.Init.StopBits = UART_STOPBITS_1;
//		huart1.Init.Parity = UART_PARITY_NONE;
//		huart1.Init.Mode = UART_MODE_TX_RX;
//		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//		huart1.Init.OverSampling = UART_OVERSAMPLING_16;
//		huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//		huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
//		huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//		if (HAL_UART_Init(&huart1) != HAL_OK) {
//			Error_Handler();
//		}
//		if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8)
//				!= HAL_OK) {
//			Error_Handler();
//		}
//		if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8)
//				!= HAL_OK) {
//			Error_Handler();
//		}
//		if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK) {
//			Error_Handler();
//		}
//		break;
//	}
//	}



	if(strstr("/?senddata",(const char *)S0RxBuf))
	{
		sio_opRq();
	}
	else
	{
		HAL_UART_DeInit(&huart1);
		huart1.Instance = USART1;
		huart1.Init.BaudRate = 9600;
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		huart1.Init.StopBits = UART_STOPBITS_1;
		huart1.Init.Parity = UART_PARITY_NONE;
		huart1.Init.Mode = UART_MODE_TX_RX;
		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart1.Init.OverSampling = UART_OVERSAMPLING_16;
		huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
		huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart1) != HAL_OK) {
			Error_Handler();
		}
		if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8)
				!= HAL_OK) {
			Error_Handler();
		}
		if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8)
				!= HAL_OK) {
			Error_Handler();
		}
		if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK) {
			Error_Handler();
		}
	}

}

/*Initializes Uart interfaces
 *Also sets interrupt callback handler
 */
GLOBAL void sio_init() {

	memset((void*) &uart_control_block, 0, sizeof(uart_control_block)); // reset control block

}

/***Interrupt Routines****/

/*
 * Interrupt handling for Received Data
 */
LOCAL void sio_RxIntS0(UART_HandleTypeDef *huart) {

	//  HAL_UART_Receive_IT(&Analog_I_Uart,   uart_control_block.rx, 1); //we enable it again to reive another interrupt
	uart_control_block.Echar_rx = (uint8_t) sio_uart_rxRec(huart);
	*uart_control_block.rx++ = (uint8_t) sio_uart_rxRec(huart);

	if (uart_control_block.Echar == uart_control_block.Echar_rx) //we received end char so string is finish
			{
		huart->RxISR = NULL;
		util_setUartEvent();
	}



}

/*
 * Interrupt handling for sent Data
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	util_setUartEvent();
}
