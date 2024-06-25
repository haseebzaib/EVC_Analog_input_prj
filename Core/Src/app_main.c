/*
 * app_main.c
 *
 *  Created on: Jan 3, 2024
 *      Author: Admin
 */


#include "main.h"
#include "ads8344.h"
#include "util.h"
#include "sio.h"

#include "i2c_com.h"



/**
 * Main app in which all the routines run
 */
void main_app(void)
{


     sio_init();


	while(1)
	{



	sio_task();
	//i2c_com_task();


	}


}
