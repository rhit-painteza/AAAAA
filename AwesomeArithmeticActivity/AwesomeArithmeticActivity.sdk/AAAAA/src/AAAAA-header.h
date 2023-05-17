/*
 * AAAAA-header.h
 *
 *  Created on: May 15, 2023
 *      Author: wilsonic
 */

#ifndef SRC_AAAAA_HEADER_H_
#define SRC_AAAAA_HEADER_H_

#include <xparameters.h>
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include <xgpio.h>
#include "stdio.h"
#include <xiicps.h>
#include "xuartps.h"
#include <stdlib.h>
#include "xtime_l.h"
#include <math.h>
//buttons and switches
#define BTNS_DEVICE_ID		XPAR_BTNS_4BITS_DEVICE_ID
#define SW_DEVICE_ID		XPAR_SWS_4BITS_DEVICE_ID

// Timer hardware definitions
#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define GIC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR

//Timer definitions
#define ONE_TENTH 32500000
#define ONE_SECOND 10*ONE_TENTH
#define FIVE_SECONDS 5*ONE_SECOND;

//game definitions
#define EASY 5
#define MEDIUM 10
#define HARD 20
#define EXPERT 50


/************************** Variable Definitions *****************************/
XScuTimer TimerInstance;	/* Cortex A9 Scu Private Timer Instance */
XScuGic GICinstance;		/* GIC Interrupt Controller Instance */

XGpio SWInst, BTNInst;
extern size_t timer_seconds;
extern size_t starting_time;
extern int DRAW_TIMER_FLAG;

//Function prototypes
int SetupScuTimer(XScuTimer *TimerInstance, u16 timer_ID, unsigned int load_value, u16 timer_interrupt_id);
int SetupGIC(XScuGic *IntcInstancePtr, u16 gic_id, XScuTimer *TimerInstancePtr, u16 TimerIntrId);
void TimerIntrHandler(void *CallBackRef);
int SetupGPIO(XGpio *GPIOdevice, int DeviceID);
void getline(char* buffer, int size);
void gameStart(size_t* timerval, int* numplayers);

#endif /* SRC_AAAAA_HEADER_H_ */
