/*
 * AAAAA-main.c

 *
 *  Created on: May 15, 2023
 *      Author: wilsonic
 */

#include "AAAAA-header.h"

int scutimer_stuff(){
	int Status;

	Status = SetupScuTimer(&TimerInstance, TIMER_DEVICE_ID, ONE_SECOND, TIMER_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		xil_printf("SCU Timer Enable Failed\r\n");
		return XST_FAILURE;     }

	Status = SetupGIC(&GICinstance, GIC_DEVICE_ID,
		      &TimerInstance, TIMER_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		xil_printf("SCU Timer Interrupt Example Test Failed\r\n");
		return XST_FAILURE;     }
	return XST_SUCCESS;
}

int SetupGPIO(XGpio *GPIOdevice, int DeviceID) {
	int status;
	// Initialize LEDs
	status = XGpio_Initialize(GPIOdevice, DeviceID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	// Set LEDs direction to outputs
	XGpio_SetDataDirection(GPIOdevice, 1, 0xFF);
	return XST_SUCCESS;
} //end SetupLEDs()

void gameStart(int* secondsval, int* numplayers){
	printf("                         \r\n");
	printf("                         \r\n");
	printf(" -- An Absolutely Awesome Arithmetic Activity --\r\n");
	printf(" -- By Zack Painter and Isabel Wilson --\r\n");
	printf(" -- Welcome! --\r\n");
	//TODO: Draw welcome screen

	char buffer[50];
	while(strncmp(buffer, "Y", 1) != 0 && strncmp(buffer, "y", 1)!=0){
		printf("        Please use the slide switches to set the timer (increments of 5 seconds)\r\n");
		printf("        Please use the buttons to select the number of players\r\n");
		char btn_val = 0;
		char sw_val = 0;

		while(btn_val == 0){
			btn_val = XGpio_DiscreteRead(&BTNInst, 1);
			sw_val = XGpio_DiscreteRead(&SWInst, 1);
		}

		//convert button press to player num
		int numplyrs = 0;
		while(btn_val > 0){
			btn_val = (btn_val << 1) & 0x0F;
			numplyrs++;
		}

		//convert switch val to timer value
		*secondsval = sw_val == 0 ? 1 : sw_val*5;
		*numplayers = numplyrs;

		printf("\tAre the following parameters okay? (Y/n)\r\n\t\tTimer: %ds\r\n\t\tNumber of Players: %d\r\n",
				*secondsval, *numplayers);
		getline(buffer, 50);
	}

}

int gameEnd(){
	//TODO: draw game over screen somewhere in here.
	printf(" -- GAME OVER --\r\n");
	printf(" -- Play again? (Y/n) -- \r\n");
	char buffer[5];
	getline(buffer, 5);
	if(strncmp(buffer, "Y", 1) == 0 || strncmp(buffer, "y", 1) == 0){
		return 0;
	}
	printf(" -- Goodbye! -- \r\n");
	return 1;
}
void getline(char* buffer, int size){
	int index = 0;
	char userInput = 0;
	printf("PROMPT> ");

	while(userInput != '\n' && userInput != '\r' && index < size-3){
		userInput = XUartPs_ReadReg(STDIN_BASEADDRESS, XUARTPS_FIFO_OFFSET);
		printf("%c", userInput);
		if(userInput != 0){
			buffer[index] = userInput;
			index++;
		}
	}
	buffer[index] = '\r';
	buffer[index+1] = '\n';
	buffer[index+2]='\0';
}

int playGame(int player){
	return 0;
}

int main(){
	int status;

	//set up timer & global interrupts
	status = scutimer_stuff();
	if (status != XST_SUCCESS) {
		xil_printf("Timer/Interrupt Setup Failed\r\n");
		return XST_FAILURE;     }

	//set up buttons
	SetupGPIO(&BTNInst, BTNS_DEVICE_ID);
	SetupGPIO(&SWInst, SW_DEVICE_ID);

	while(1){ //game start
		int timerload, numplayers;
reset:
		timerload = 0; numplayers = 0;
		gameStart(&timerload, &numplayers);
		int i, score[numplayers];
		for(i = 0; i < numplayers; i++){
			score[i] = playGame(i);
		}
		if(gameEnd()) break;

	}

	return 0;
}
