/*
 * ads8344.c
 *
 *  Created on: Jan 3, 2024
 *      Author: Admin
 */


/************************************************************************/
/*Author: Haseeb Zaib         Program: Analog Inputs for EVC            */
/*Ver: 1                      Date: jan 3, 2024                        */
/*  ads8344 interface file, capable of reading 4-20mA                   */
/************************************************************************/

#include "ads8344.h"
#include "main.h"
#include "cmsis_gcc.h"


/* Communications Register Values */
#define ANALOG_CTRL  0x83	// Start DIF noPD
#define ANALOG_AOFS  4		// Shift-Offset zur Kanal-Adresse

#ifdef use_ads_spi
extern SPI_HandleTypeDef hspi1;
#endif

static uint16_t ADCmeasured_values[ANALOG_INP];	// The 4 measured values ​​in digits (measured value minus zero value)
uint16_t ADCmeasured_values_pos[ANALOG_INP];	// The 4 measured values ​​in digits (measured value at +)
uint16_t ADCmeasured_values_neg[ANALOG_INP];		// The 4 measured values ​​in digits (measured value at -)
uint16_t ADCmeasured_values_dig[ANALOG_INP];	             // The 4 measured values ​​in digits (3rd measured value)


static void ads_NopH (void)
{
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
}

static void ads_NopL (void)
{
	__asm__("nop");
	__asm__("nop");
}

#ifdef use_ads_bitbang
static void B_DOUT (GPIO_PinState ibPinState)
{
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, ibPinState);
}

static void B_SCK (GPIO_PinState ibPinState)
{
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, ibPinState);
}

static GPIO_PinState B_DIN (void)
{
 return(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6));
}
#endif


static void ads_enable (uint32_t Enable)
{
	  HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, !Enable);
}

static GPIO_PinState ads_busy (void)
{
  return(HAL_GPIO_ReadPin(ADS_BUSY1_GPIO_Port, ADS_BUSY1_Pin));
}

static uint32_t ads_busyTest(void)
{

#ifdef use_ads_bitbang
	ads_NopL();
	if (ads_busy() != 1)
		{
		return(1);
		}


	B_SCK(1);
	ads_NopH();
	B_SCK(0);


	ads_NopL();
	if (ads_busy() != 0)
		{
		return(2);
		}
	else
		{
		return(0);
		}


#elif use_ads_spi
	 uint8_t cnt =0;


	// mes_WriteByte(CR_CR, CR_DR | CR_READ);



	   while((ads_busy() != 0) && (cnt < 20)) //Waiting until the result is available or the runtime is transferred
	    {                        //wait for 2seconds if interrupt does not come move forward anyways

		   HAL_Delay(10);
	      cnt++;

	    }

	   if(cnt < 20)
		{
		   return(0);
		}
	   else
	   {
		   return(2);
	   }
#endif

}

static void ads_WriteByte (uint8_t iValue)
{
#ifdef use_ads_bitbang
	uint8_t iLoop;

		// Comm-Reg
		for (iLoop = 0x80 ; iLoop != 0; iLoop /= 2)		//shift bit for masking
		{
			if (iLoop & iValue)	B_DOUT(1);	//masking value with i, write to ADC
			else				B_DOUT(0);
			ads_NopL();
			B_SCK(1);
			ads_NopH();
			B_SCK(0);
		}
		B_DOUT(0);
#elif use_ads_spi
 HAL_SPI_Transmit(&hspi1, &iValue, 1, 1000);
#endif
}

static uint16_t ads_Readdata (void)
{
	uint16_t iValue;
	iValue = 0;

#ifdef use_ads_bitbang
	  uint16_t iLoop;
	  for (iLoop = 0x8000; iLoop != 0; iLoop /= 2)//shift bit for masking
	  {
		if (B_DIN()) iValue |= iLoop;	//read bit
		B_SCK(1);
		ads_NopH();
		B_SCK(0);
		ads_NopL();
	  }
#elif use_ads_spi
	  uint8_t rd_buf[3];
	  uint8_t wr_dummy[3] = {0,0,0};
	  HAL_SPI_TransmitReceive(&hspi1, wr_dummy, rd_buf, 2, 1000);
	  iValue = rd_buf[0] << 8 | rd_buf[1];
#endif

	  return(iValue);
}




static uint32_t ads_MeasDigit (uint32_t Idx, uint16_t *pErg)
{
uint32_t iStat;

ads_start();
    ads_WriteByte(ANALOG_CTRL | (uint8_t)(Idx << ANALOG_AOFS));	// Befehl senden
#ifdef use_ads_bitbang
	iStat = ads_busyTest();
#elif use_ads_spi
	iStat = 0;
#endif
	if (iStat == 0)
		*pErg = ads_Readdata();		// Ausgabe

	ads_stop();
	return(iStat);
}

/**
 * Measure the value
 * Channel index 0..3
 * Output: Status, 0 if ok, >0 if area violated
 */
 uint32_t ani_Mess (uint32_t Idx, float *pStrom)
{
uint16_t iErg;
uint32_t iErgSum;
uint32_t iStat;
float iU;


	iStat = ads_MeasDigit(Idx, &iErg);	// 1.Discard the measurement
	iStat = ads_MeasDigit(Idx, &iErg);	// 2.Discard the measurement
	if (iStat)
		return(iStat);

	iStat = ads_MeasDigit(Idx, &iErg);	// 3.Use measurement
	if (iStat)
		return(iStat);
	iErgSum = (uint32_t) iErg;

	iStat = ads_MeasDigit(Idx, &iErg);	// 4.Use measurement
	if (iStat)
		return(iStat);
	iErgSum += (uint32_t) iErg;

	ADCmeasured_values[Idx] = (uint16_t)(iErgSum / 2);

	// Convert digits/voltage into electricity
	iU	= (float)ADCmeasured_values[Idx] * (2.50f / 65536.0f);	// For now, U-value
	*pStrom = iU * (1000.0f / 100.0f);	// I = U / R,   R = 100 Ohm,  I * 1000 in mA/A


	return(iStat);
}

void ads_stop(void)
{
#ifdef use_ads_bitbang
	B_SCK(0);	B_DOUT(0);
	ads_enable(0);
#elif use_ads_spi
	ads_enable(0);
#endif


}

void ads_start(void)
{
#ifdef use_ads_bitbang
	  B_SCK(0);	B_DOUT(0);
	ads_enable(1);
#elif use_ads_spi
	ads_enable(1);

#endif
}
