//lab9part1ZAP_ICW2023.c
/*
This is a recommended template for Lab #9 Part 1
//You should change the file name to lab9part1JJS_JJS2023,
// where JJS are your name initials of two students on your team
//ECE530 Spring 2022-2023
//Lab #9 Part 1 to read two tmp101 breakout boards from both PL and PS sides with ARM I2C modules
//Date: 4-2-2023
 * Zackery Painter, Isabel Wilson
 */
#include "lab9part2A2023.h"

#define DELAYLOOPCOUNT 80000000
//function prototype

/*************************************************************
* This function ConfigureTMP101() configures an I2C TMP101 instance.
*
* @param	I2Cinstance is a pointer to an I2C instance.
* @param	I2CAddress is the address of I2C TMP101 chip.
* @param	TEMPresolution is .the resolution of TMP101 temperature as 0b0RR00000
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if failed.
*
*************************************************************/

int ConfigureTMP101(XIicPs *I2Cinstance, u8 I2CAddress, u8 TEMPresolution) {
	u8 SetPointertoZero = 0b00000000;
	u8 SetResolution[] = {0b00000001,0b00000000};

	SetResolution[1] = TEMPresolution;

	//Set resolution
	int status;
	I2CAddress = write | I2CAddress;
	status=XIicPs_MasterSendPolled(I2Cinstance, SetResolution, 2,I2CAddress);

	if( status == XST_FAILURE ) {
		printf("\r\n-- Set Resolution Failed");  	return XST_FAILURE;
	}

	//set pointer back to 0x00 to point to the temperature value
	status=XIicPs_MasterSendPolled(I2Cinstance, &SetPointertoZero, 1,I2CAddress);

	if( status == XST_FAILURE ) {
		printf("\r\n-- Set Pointer 0 Failed");  	return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*************************************************************
* This function ConfigureI2Cinstance() configures an I2C instance.
*
* @param	I2Cinstance is a pointer to an I2C instance.
* @param	I2CdeviceID is the ID of I2C device defined in xparamenters.h.
* @param	I2C_ClockRate is clock frequency of I2C TMP101 in Hz.
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if failed.
*
*************************************************************/
int ConfigureI2Cinstance(XIicPs *I2Cinstance, int I2CdeviceID, int I2C_ClockRate) {

	int status;
	XIicPs_Config *I2C_config;

//Call lookup table to find I2C hardware module for I2CdeviceID
	I2C_config = XIicPs_LookupConfig(I2CdeviceID);

	if (NULL == I2C_config) {
		printf("\r\n-- Failed to find I2CdeviceID %d", I2CdeviceID);  	return XST_FAILURE;
	}

//Initialize I2C instance. Return XST_FAILTURE if failed.

	status = XIicPs_CfgInitialize(I2Cinstance, I2C_config, I2C_config->BaseAddress);
	if (status != XST_SUCCESS) {
		printf("\r\n-- Configuration Failed");  	return XST_FAILURE;
	}

//Self test. Return XST_FAILTURE if failed
	status = XIicPs_SelfTest(I2Cinstance);
	if (status != XST_SUCCESS) {
		printf("\r\n-- Self Test Failed");  	return XST_FAILURE;
	}

//Set I2C clock frequency. Return XST_FAILTURE if failed

	XIicPs_SetSClk(I2Cinstance, I2C_ClockRate);
	if (status != XST_SUCCESS) {
			printf("\r\n-- Self Clock Failed");  	return XST_FAILURE;
		}

//Wait when I2Cdevice is busy, until bus is idle to start another transfer
//	printf("going into while \r\n");
	s32 status1 = (s32)TRUE;
	while( status1 ){
		status1 = XIicPs_BusIsBusy(I2Cinstance);
//		printf("%d \r\n",status1);
	}
//	printf("out of while \r\n");

	return XST_SUCCESS;

} // end ConfigureI2Ctmp101()

void scutimer_stuff(){
	int Status;

	// Initialize LEDs
//	  Status = SetupLEDs(&LEDInst, LEDS_DEVICE_ID);
//	  if(Status != XST_SUCCESS) return XST_FAILURE;

	Status = SetupScuTimer(&TimerInstance, TIMER_DEVICE_ID, TIMER_LOAD_VALUE, TIMER_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		xil_printf("SCU Timer Enable Failed\r\n");
		return XST_FAILURE;     }

	Status = SetupGIC(&GICinstance, GIC_DEVICE_ID,
		      &TimerInstance, TIMER_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		xil_printf("SCU Timer Interrupt Example Test Failed\r\n");
		return XST_FAILURE;     }
}

int main(void){

    float temp_ps, temp_pl;
    unsigned int loopcount;
    int status;
    printf("\r\n\n -- Start Lab 9 Part 1 --\r\n");

//    XGpio_Initialize(&uart1, UARTDATA_GPIO);
//    XGpio_Initialize(&uart_tx_full, BUFFERFULL_GPIO);
//    XGpio_SetDataDirection(&uart1, 1, 0x00000000);
//    XGpio_SetDataDirection(&uart1, 2, 0x00000000);
//    XGpio_SetDataDirection(&uart_tx_full, 1, 0xFFFFFFFF);

    //configure I2C instance and tmp101 board on PS port
//
//    status = ConfigureI2Cinstance(&i2c_ps,PS_I2C_DEVICE_ID, PS_DATA_RATE);
//    if(status == XST_FAILURE) printf("\r\n Failed to configure I2C instance on PS.");
//
//
////set resolution
//    //ConfigureTMP101(XIicPs *I2Cinstance, u8 I2CAddress, u8 TEMPresolution)
//    status=ConfigureTMP101(&i2c_ps, PS_TMP101_ADDRESS, PS_TMP101_RESOLUTION);
//    if(status == XST_FAILURE) printf("\r\n Failed to configure TMP101 on PS.");


 //configure I2C instance and tmp101 board on PL port

  //  status = ConfigureI2Cinstance(&i2c_pl,PL_I2C_DEVICE_ID, PL_DATA_RATE);

//    if(status == XST_FAILURE) printf("\r\n Failed to configure I2C instance on PL.");
////set resolution
//
//    status=ConfigureTMP101(&i2c_pl, PL_TMP101_ADDRESS, PL_TMP101_RESOLUTION);
//
//    if(status == XST_FAILURE) printf("\r\n Failed to configure TMP101 on PL.");
//
    scutimer_stuff();
//	while(1) {
////		printf("in loop \n\r");
////Read tmp101 board and calculate temperature value on PS port
//
////	if(status == XST_FAILURE) printf("\r\n Failed to read TMP101 from PS on bottom row of Connector JF.");
////
//////Read tmp101 board and calculate temperature value on PL port
////
////	if(status == XST_FAILURE) printf("\r\n Failed to read TMP101 from PL on top row of Connector JB.");
////
/////*
//////printing floating numbers is not supported by xil_printf().
//////Use printf() from <stdio.h> to print floating points.
//////Mixing Xil_printf() with printf() may cause some printf() statements to be dropped.
//////Display temperature in floating point number with 4 digits after decimal point
//// */
//	float ps_tmpF;
//////	printf("Reading from PS\r\n");
//
//	//ReadTemperature(&i2c_ps, &ps_tmp,PS_TMP101_ADDRESS);
//	ps_tmpF = (ps_tmp*1.8)+32;
//	float pl_tmpF;
////	printf("Reading from PL\r\n");
//	//ReadTemperature(&i2c_pl, &pl_tmp,PL_TMP101_ADDRESS);
//	pl_tmpF = (pl_tmp*1.8)+32;
//	char buffer[100];
//	//char plbuffer[100];
//	printf("TMP101 on PS %f deg. C   %f deg. F\r\n",ps_tmp,ps_tmpF);
//	printf("TMP101 on PL %f deg. C   %f deg. F\r\n",pl_tmp,pl_tmpF);
////	printf("hi\n");
//	//printf(plbuffer);
//	sprintf(buffer,"TMP101 on PS %f deg. C   %f deg. F\r\n",ps_tmp,ps_tmpF);
//	Print_message(buffer);
//	sprintf(buffer,"TMP101 on PL %f deg. C   %f deg. F\r\n",pl_tmp,pl_tmpF);
//	Print_message(buffer);
//	//Print_message("hi pl\n");
//
//		//delay loop to pause for a while
//		for(loopcount=0; loopcount<DELAYLOOPCOUNT; loopcount++);	//delay time
//
//	} //while(1)

    while(1);	// hold just in case

}	//end main()


/************************************************************
* This function ReadTemperature() reads temperature of an TMP101.
*
* @param	I2Cinstance is a pointer to an I2C instance.
* @param	temperature is the returned floating point temperature value.
* @param	I2C_address is the address of I2C TMP101 chip.
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if failed.
*
************************************************************/
int ReadTemperature(XIicPs * I2Cinstance, float * temperature, int I2C_address) {

	int status;
	u8 temp[2];	//2 byte temperature
	u8 SetPointertoZero=0b00000000;

	//set pointer back to 0x00 to point to the temperature value
	I2C_address = I2C_address | write;

	status = XIicPs_MasterSendPolled(I2Cinstance, &SetPointertoZero, 1, I2C_address);
	if( status == XST_FAILURE )
		return status;

	//Read temperature. Return XST_FAILURE if failed
	/* Wait until bus is idle to start another transfer. */
		while (XIicPs_BusIsBusy(I2Cinstance)) {
			/* NOP */
//			printf("WAIT");
		}
		I2C_address = I2C_address & read;

		u8 from_tmp[2];
		status = XIicPs_MasterRecvPolled(I2Cinstance, from_tmp,
				  2, I2C_address);
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	//Convert temperature to floating number

	    *temperature = (float) from_tmp[0]; // Get the whole number
		*temperature = *temperature +((float)(from_tmp[1]>>4))/16; // get decimal place
//		*temperature = *temperature;
	return XST_SUCCESS;
}	//end of ReadTemperature()

