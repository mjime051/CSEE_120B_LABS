/*
 * Lab11Part3.c
 *
 * Created: 5/16/2019 7:12:53 AM
 * Author : 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "io.h"
#include "bit.h"
#include "keypad.h"
#include "scheduler.h"
#define F_CPU 8000000UL
#include <util/delay.h>


#define LCD_8BIT_H

#include <bit.h>

// Define LCD port assignments here so easier to change than if hardcoded below
#define LCD_DATA PORTD	// LCD 8-bit data bus
#define LCD_CTRL PORTA	// LCD needs 2-bits for control
#define LCD_RS   3 		// LCD Reset pin
#define LCD_E    4		// LCD Enable pin

unsigned char tempB = 0x00;


// Set by LCD interface synchSM, ready to display new string
unsigned char LCD_rdy_g = 0;
// Set by user synchSM wishing to display string in LCD_string_g
unsigned char LCD_go_g = 0;
// Filled by user synchSM, 16 chars plus end-of-string char
unsigned char LCD_string_g[16];
// Determine if the LCD will write a char (0) or a string (1)
unsigned char LCD_write_str = 1;
// Position to write a single character to the LCD Position (0~15)
unsigned char LCD_char_pos = 0;

void LCD_WriteCmdStart(unsigned char cmd) {
	LCD_CTRL = SetBit(LCD_CTRL,LCD_RS, 0);
	LCD_DATA = cmd;
	LCD_CTRL = SetBit(LCD_CTRL,LCD_E, 1);
}
void LCD_WriteCmdEnd() {
	LCD_CTRL = SetBit(LCD_CTRL,LCD_E, 0);
}
void LCD_WriteDataStart(unsigned char Data) {
	LCD_CTRL = SetBit(LCD_CTRL,LCD_RS,1);
	LCD_DATA = Data;
	LCD_CTRL = SetBit(LCD_CTRL,LCD_E, 1);
}
void LCD_WriteDataEnd() {
	LCD_CTRL = SetBit(LCD_CTRL,LCD_E, 0);
}



enum LI_States { LI_Init1, LI_Init2, LI_Init3, LI_Init4, LI_Init5,
LI_WaitDisplayString, LI_PositionCursor, LI_DisplayChar, LI_WaitGo0 };

int LCDI_SMTick(int state) {
	static unsigned char i;
	switch(state) { // Transitions
		case -1:
		state = LI_Init1;
		break;
		case LI_Init1:
		state = LI_Init2;
		i=0;
		break;
		case LI_Init2:
		if (i<10) { // Wait 100 ms after power up
			state = LI_Init2;
		}
		else {
			state = LI_Init3;
		}
		break;
		case LI_Init3:
		state = LI_Init4;
		LCD_WriteCmdEnd();
		break;
		case LI_Init4:
		state = LI_Init5;
		LCD_WriteCmdEnd();
		break;
		case LI_Init5:
		state = LI_WaitDisplayString;
		LCD_WriteCmdEnd();
		break;
		case LI_WaitDisplayString:
		if (!LCD_go_g) {
			state = LI_WaitDisplayString;
		}
		else if (LCD_go_g) {
			LCD_rdy_g = 0;
			state = LI_PositionCursor;
			i=0;
		}
		break;
		case LI_PositionCursor:
		state = LI_DisplayChar;
		LCD_WriteCmdEnd();
		break;
		case LI_DisplayChar:
		if (i<16 && LCD_write_str) {
			state = LI_PositionCursor;
			LCD_WriteDataEnd();
			i++;
		}
		else {
			state = LI_WaitGo0;
			LCD_WriteDataEnd();
		}
		break;
		case LI_WaitGo0:
		if (!LCD_go_g) {
			state = LI_WaitDisplayString;
		}
		else if (LCD_go_g) {
			state = LI_WaitGo0;
		}
		break;
		default:
		state = LI_Init1;
	} // Transitions

	switch(state) { // State actions
		case LI_Init1:
		LCD_rdy_g = 0;
		break;
		case LI_Init2:
		i++; // Waiting after power up
		break;
		case LI_Init3:
		LCD_WriteCmdStart(0x38);
		break;
		case LI_Init4:
		LCD_WriteCmdStart(0x0F);
		break;
		case LI_Init5:
		LCD_WriteCmdStart(0x01); // Clear
		break;
		case LI_WaitDisplayString:
		LCD_rdy_g = 1;
		break;
		case LI_PositionCursor:
		if ( LCD_write_str ) {
			LCD_Cursor(i);
			} else {
			LCD_Cursor(LCD_char_pos);
		}
		break;
		case LI_DisplayChar:
		if ( LCD_write_str ) {
			LCD_WriteDataStart(LCD_string_g[i]);
			} else {
			LCD_WriteDataStart(LCD_string_g[0]);
		}
		break;
		case LI_WaitGo0:
		break;
		default:
		break;
	} // State actions
	return state;
}



unsigned char button;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 |= 1<< OCIE1A; //timer mask

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	//SREG |= 0x80; // 0x80: 1000000
	sei();// enables the global interupts
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum SM1_States{SM1_disp};

int SMTick1(int state){
	unsigned char keypressed;
	keypressed = GetKeypadKey();
	
	switch(state){
		case SM1_disp:
		switch (keypressed) {
			case '\0': break; // All 5 LEDs on
			case '1': tempB = 0x01;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '2': tempB = 0x02;
			LCD_Cursor(1); LCD_WriteData(tempB + '0');
			break;
			case '3': tempB = 0x03;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '4': tempB = 0x04;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '5': tempB = 0x05;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '6': tempB = 0x06;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '7': tempB = 0x07;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '8': tempB = 0x08;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '9': tempB = 0x09;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case 'A': tempB = 0x0A;
			LCD_Cursor(1); LCD_WriteData(tempB + 0x37); 
			break;
			case 'B': tempB = 0x0B;
			LCD_Cursor(1); LCD_WriteData(tempB + 0x37); 
			break;
			case 'C': tempB = 0x0C;
			LCD_Cursor(1); LCD_WriteData(tempB + 0x37); 
			break;
			case 'D': tempB = 0x0D;
			LCD_Cursor(1); LCD_WriteData(tempB + 0x37); 
			break;
			case '*': tempB = 0x0E;
			LCD_Cursor(1); LCD_WriteData(tempB + 0x1C); 
			break;
			case '0': tempB = 0x00;
			LCD_Cursor(1); LCD_WriteData(tempB + '0'); 
			break;
			case '#': tempB = 0x0F;
			LCD_Cursor(1); LCD_WriteData(tempB + 0x14); 
			break;
			default: tempB = 0x1B; 
			break; // Should never occur. Middle LED off.
		}
		state = SM1_disp;
		PORTB=tempB;
		break;
	}
	return state;
}


int main()
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F; // PC7-4 outputs, PC3-0 inputs
	DDRD = 0xFF; PORTD = 0x00;
	
	// Period for the tasks
	unsigned long int SMTick1_calc = 50;

	//Calculating GCD
	unsigned long int tmpGCD = 1;

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;//maybe take out /GCD

	//Declare an array of tasks
	static task task1;
	task *tasks[] = { &task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = 0;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &SMTick1;//Function pointer for the tick.


	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	//initialize LCD
	LCD_init();

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}

	// Error: Program should not exit!
	return 0;
}
