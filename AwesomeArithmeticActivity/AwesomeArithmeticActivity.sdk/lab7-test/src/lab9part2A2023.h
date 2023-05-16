/*File Name: LEDcounter_scutimer_interrupt.h */
/*
//ECE530 Spring 2022-2023
//Author: Jianjian Song
// A count-up counter is incremented by the Cortex A9 SCU Private Timer interrupt
// This is revision based on xscutimer_intr_example.c
//from C:\Xilinx\SDK\2016.2\data\embeddedsw\XilinxProcessorIPLib\drivers\scutimer_v2_1\examples
*/
#include <xparameters.h>
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include <xgpio.h>
#include "stdio.h"
#include <xiicps.h>

/************************** Constant Definitions *****************************/
#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define GIC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR
//#define LEDS_DEVICE_ID		XPAR_AXI_GPIO_0_DEVICE_ID

#define ONE_TENTH 32500000
#define TIMER_LOAD_VALUE		5*ONE_TENTH

// I2C Definitions
#define PS_I2C_DEVICE_ID XPAR_PS7_I2C_0_DEVICE_ID
#define PS_DATA_RATE 100000 // 100KHz
#define PS_TMP101_ADDRESS 0b01001010
#define PS_TMP101_RESOLUTION 0b01100000

//#define PL_I2C_DEVICE_ID XPAR_PS7_I2C_1_DEVICE_ID
#define PL_DATA_RATE 200000 // 200KHz
#define PL_TMP101_ADDRESS 0b01001000	//groun
#define PL_TMP101_RESOLUTION 0b00100000
#define write 0x80
#define read 0x7F

//#define UARTDATA_GPIO XPAR_AXI_GPIO_1_DEVICE_ID
//#define BUFFERFULL_GPIO XPAR_AXI_GPIO_2_DEVICE_ID
/************************** Variable Definitions *****************************/
XScuTimer TimerInstance;	/* Cortex A9 Scu Private Timer Instance */
XScuGic GICinstance;		/* GIC Interrupt Controller Instance */
XGpio LEDInst, uart1, uart_tx_full;	//LED instance
XIicPs i2c_ps, i2c_pl;	//device instances
float ps_tmp, pl_tmp;

//Function prototypes
int SetupScuTimer(XScuTimer *TimerInstance, u16 timer_ID, unsigned int load_value, u16 timer_interrupt_id);
int SetupGIC(XScuGic *IntcInstancePtr, u16 gic_id, XScuTimer *TimerInstancePtr, u16 TimerIntrId);
void TimerIntrHandler(void *CallBackRef);
int SetupLEDs(XGpio *LEDdevice, int DeviceID);
int ReadTemperature(XIicPs * I2Cinstance, float * temperature, int I2C_address);
void Print_message(char* Message);
