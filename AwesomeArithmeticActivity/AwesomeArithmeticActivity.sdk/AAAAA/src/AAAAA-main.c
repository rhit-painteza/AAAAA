/*
 * AAAAA-main.c

 *
 *  Created on: May 15, 2023
 *      Author: wilsonic
 */

#include "AAAAA-header.h"
size_t timer_seconds;
size_t starting_time;
int DRAW_TIMER_FLAG = 0;

int scutimer_stuff(){
	int Status;

	Status = SetupScuTimer(&TimerInstance, TIMER_DEVICE_ID, ONE_SECOND, TIMER_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		xil_printf("SCU Timer Enable Failed\r\n");
		return XST_FAILURE;     }
	XScuTimer_EnableAutoReload(&TimerInstance);

	Status = SetupGIC(&GICinstance, GIC_DEVICE_ID,
		      &TimerInstance, TIMER_IRPT_INTR);
	if (Status != XST_SUCCESS) {
		xil_printf("SCU Timer Interrupt Example Test Failed\r\n");
		return XST_FAILURE;     }
	XScuTimer_LoadTimer(&TimerInstance, ONE_SECOND);
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

void gameStart(size_t* secondsval, int* numplayers){
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

int randint(int n) {
    XTime  *t=0;
    XTime_GetTime(t);
    srand(*t);
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    //assert (n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
   // assert (end > 0);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

int getanswer(int param1, int param2, char op){
	float f =0;
	switch(op){
		case '+':
			return param1+param2;
		case '-':
			return param1-param2;
		case '/':
			if(param2 == 0) param2 = 1;
			f= param1/param2;
			return (int)rintf(f);
		case '*':
			return param1*param2;
	}
}

const char ops[] = {'+', '-', '/', '*'};
void generateQuestion(int qcount, char* qbuf, int* answer){
	int max_param;
	int op_index;
	if(qcount < EASY){
		max_param = 6;
		op_index = 2;
	}else if(qcount < MEDIUM){
		max_param = 13;
		op_index = 4;
	}else if(qcount < HARD){
		max_param = 21;
		op_index = 4;
	}else if(qcount < EXPERT){
		max_param = 101;
		op_index = 4;
	}else{ //EXPERT+
		max_param = RAND_MAX;
		op_index = 4;
	}
	int param1 = randint(max_param);
	int param2 = randint(max_param);
	char op = ops[randint(op_index)];
	char tmp[40];
	char *divmsg = tmp;
	divmsg = (op == '/') ? "(always round down)" : "";
	*answer = getanswer(param1, param2, op);
	sprintf(qbuf, "QUESTION %d: What is %d %c %d? %s\r\n", qcount, param1, op, param2, divmsg);
}

int playGame(int player){
	timer_seconds = starting_time;
	printf("                     \r\n");
	printf("                     \r\n");
	printf(" -- Player %d, are you ready? (Press enter to start)\r\n", player+1);
	char buf[5];
	getline(buf, 5);
	int score = 0;

	//math game
	int q_count = 1;
	char question[100];
	int answer, numincorrect=0;
	while(numincorrect < 3){
		int index = 0;
		char inbuf[10];
		timer_seconds = starting_time;
		bzero(question, 100);
		bzero(inbuf, 10);
		generateQuestion(q_count, question, &answer);
		printf("                     \r\n");
		printf("       "); //tab
		printf(question);
		//TODO: print question to screen
		XScuTimer_Start(&TimerInstance);
		printf("PROMPT> ");
		while(timer_seconds > 0){
			if(DRAW_TIMER_FLAG){
				//TODO: draw update timer here :)
				DRAW_TIMER_FLAG = 0;
			}
			//super cool non-blocking input
			char userInput = XUartPs_ReadReg(STDIN_BASEADDRESS, XUARTPS_FIFO_OFFSET);
			printf("%c", userInput);
			if(userInput != 0){
				inbuf[index] = userInput;
				index++;
			}
			if(userInput == '\r' || userInput == '\n' || index > 9) break;
		}
		XScuTimer_Stop(&TimerInstance);
		if(timer_seconds == 0 ){
			numincorrect++;
			printf("\r       OUT OF TIME! Please try again.\r\n");
			//TODO: display time ran out screen
		} else if(atoi(inbuf) != answer){
			numincorrect++;
			printf("\r       Your Answer: %d -- INCORRECT! Please try again.\r\n", atoi(inbuf));
			//TODO: update screen
		} else {
			score++;
			printf("\r       Your Answer: %d -- CORRECT! Good work.\r\n", atoi(inbuf));
			//TODO: update screen
		}
		q_count++;
	}
	return score;
}

int main(){
	int status;
	time_t t;
	//set up timer & global interrupts
	status = scutimer_stuff();
	if (status != XST_SUCCESS) {
		xil_printf("Timer/Interrupt Setup Failed\r\n");
		return XST_FAILURE;     }

	//set up buttons
	SetupGPIO(&BTNInst, BTNS_DEVICE_ID);
	SetupGPIO(&SWInst, SW_DEVICE_ID);

	while(1){ //game start
		int numplayers;
reset:
		timer_seconds = 0; numplayers = 0, starting_time = 0;;
		gameStart(&starting_time, &numplayers);
		int i, score[numplayers], max = 0, max_i = 0;
		for(i = 0; i < numplayers; i++){
			score[i] = playGame(i);
			printf("       Player %d scored %d points!\r\n", i+1, score[i]);
			if(score[i] > max){
				max = score[i];
				max_i = i;
			}
		}
		printf("\r\n -- Player %d wins with score %d!!! -- \r\n", max_i+1, max);
		//TODO: win/game over/play again screen
		if(gameEnd()) break;

	}

	return 0;
}
