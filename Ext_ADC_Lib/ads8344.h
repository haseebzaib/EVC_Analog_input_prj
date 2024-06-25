/*
 * ads8344.h
 *
 *  Created on: Jan 3, 2024
 *      Author: Admin
 */

#ifndef ADS8344_H_
#define ADS8344_H_

/************************************************************************/
/*Author: Haseeb Zaib         Program: Analog Inputs for EVC            */
/*Ver: 1                      Date: jan 3, 2024                        */
/* ads8344 interface file, capable of reading 4-20mA                  */
/************************************************************************/


#include "main.h"

#define use_ads_bitbang 1
//#define use_ads_spi 1

#define ANALOG_INP		4	//number of analog inputs

extern uint16_t ADCmeasured_values_pos[ANALOG_INP];	// The 4 measured values ​​in digits (measured value at +)
extern uint16_t ADCmeasured_values_neg[ANALOG_INP];		// The 4 measured values ​​in digits (measured value at -)
extern uint16_t ADCmeasured_values_dig[ANALOG_INP];	             // The 4 measured values ​​in digits (3rd measured value)

extern  uint32_t ani_Mess (uint32_t Idx, float *pStrom);
extern void ads_stop(void);
extern void ads_start(void);

#endif /* ADS8344_H_ */
