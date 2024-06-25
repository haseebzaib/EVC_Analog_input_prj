/*
 * i2c_com.c
 *
 *  Created on: May 13, 2024
 *      Author: user
 */


#include "main.h"
#include "i2c_com.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "util.h"
#include "ads8344.h"

union  MIX_UWORD {
                 uint16_t W;
                 struct  { uint8_t Lb;
                          uint8_t Hb; } B;
               };

union MIX_UWORD convert_16bit_to_8bit;




uint8_t aRxBuffer[12];
uint8_t counter;
uint8_t *pRx;
LOCAL uint8_t opRq_buf[] = "senddata";
LOCAL float ad_val[ANALOG_INP];
LOCAL uint8_t ad_buf[ANALOG_INP][50];
#define N_DECIMAL_POINTS_PRECISION (100000)
LOCAL uint8_t Err;
LOCAL char Tx_buffer[500];
/*
 * Opening request prompt from EVC
 * we make request buffer here
 */
LOCAL void sio_opRq() {
	if (*pRx++ != '?')		// after / must ? come
			{
		return;
	}



	if (memcmp(pRx, opRq_buf, strlen((const char*) opRq_buf)) == 0) {

		for (int i = 0; i < ANALOG_INP; i++) {

			Err = ani_Mess(i, &ad_val[i]);


			if(Err)
			{
				sprintf((char *)ad_buf[i], "%s", "ERROR");
			}
			else
			{
				int integerPart = (int)ad_val[i];
				int decimalPart = ((int)(ad_val[i]*N_DECIMAL_POINTS_PRECISION)%N_DECIMAL_POINTS_PRECISION);
				sprintf((char *)ad_buf[i], "%d.%05d",integerPart,decimalPart);//cant use float due to memory constrains
			}

		}


		sprintf(Tx_buffer,"\x01IN1:%s,IN2:%s,IN3:%s,IN4:%s!\x0A",
				(char *)ad_buf[0],
				(char *)ad_buf[1],
				(char *)ad_buf[2],
				(char *)ad_buf[3]
			     );


		convert_16bit_to_8bit.W = strlen(Tx_buffer);


        uint8_t len[2];
        len[0] = convert_16bit_to_8bit.B.Hb;
        len[1] = convert_16bit_to_8bit.B.Lb;

        HAL_I2C_Slave_Transmit_IT(&hi2c1, len, 2); //send length to tell controller how much data it will receive


        while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
        {
        }

        HAL_Delay(1000);


        HAL_I2C_Slave_Transmit_IT(&hi2c1, (uint8_t *)Tx_buffer, strlen(Tx_buffer)); //send data now


           while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
           {
           }


	} else {
		return;
	}

}

void i2c_com_task()
{

counter  = 0;
      hi2c1.State = HAL_I2C_STATE_READY;
	  HAL_I2C_Slave_Receive_IT(&hi2c1, aRxBuffer, 11);
	  while ((HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)  && counter < 20)
	  {
       counter++;
       HAL_Delay(400);
	  }



		pRx = aRxBuffer;

		switch (*pRx++) //for future more cases can be added if needed
		{
		case '/': //opening request
		{
			sio_opRq();
			break;
		}
		}




}
