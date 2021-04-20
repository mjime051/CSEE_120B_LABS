/*
 * mjime051_lab10_part1.c
 *
 * Created: 5/8/2019 2:26:27 PM
 * Author : matt_
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

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
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
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

unsigned char threeLED = 0x00;
unsigned char blinkLED = 0x00;

enum threeStates {THREE_INIT, LIGHT_ONE, LIGHT_TWO, LIGHT_THREE} threeState;
enum blinkStates {BLINK_INIT, BLINK_ON, BLINK_OFF} blinkState;
enum combineStates {COMBINE_INIT, COMBINED} combineState;

void TickThree(){
	static char three_cnt;
	switch(threeState){
		case THREE_INIT:
			threeState = LIGHT_ONE;
			break;
		case LIGHT_ONE:
			if(three_cnt < 3)
			{
				threeState = LIGHT_ONE;
				three_cnt++;
			}
			else if (three_cnt == 3)
			{
				threeState = LIGHT_TWO;
				three_cnt = 0x00;
			}
			break;
		case LIGHT_TWO:
			if(three_cnt < 3)
			{
				threeState = LIGHT_TWO;
				three_cnt++;
			}
			else if (three_cnt == 3)
			{
				threeState = LIGHT_THREE;
				three_cnt = 0x00;
			}
			break;
		case LIGHT_THREE:
			if(three_cnt < 3)
			{
				threeState = LIGHT_THREE;
				three_cnt++;
			}
			else if (three_cnt == 3)
			{
				threeState = LIGHT_ONE;
				three_cnt = 0x00;
			}
			break;
		default:
			threeState = THREE_INIT;
			break;
	}
	switch(threeState){
		case THREE_INIT:
			threeLED = 0x00;
			three_cnt = 0x00;
			break;
		case LIGHT_ONE:
			threeLED = 0x01;
			break;
		case LIGHT_TWO:
			threeLED = 0x02;
			break;
		case LIGHT_THREE:
			threeLED = 0x04;
			break;
		default:
			break;
	}
}

void TickBlink(){
	static char blink_cnt;
	switch(blinkState){
		case BLINK_INIT:
			blinkState = BLINK_OFF;
			break;
		case BLINK_OFF:
			if(blink_cnt < 10){
				blinkState = BLINK_OFF;
			}
			else if (blink_cnt >= 10)
			{
				blinkState = BLINK_ON;
				blink_cnt = 0x00;
			}
			break;
		case BLINK_ON:
			if(blink_cnt < 10){
				blinkState = BLINK_ON;
			}
			else if (blink_cnt >= 10)
			{
				blinkState = BLINK_OFF;
				blink_cnt = 0x00;
			}
			break;
		default:
			blinkState = BLINK_INIT;
			break;
	}
	switch(blinkState){
		case BLINK_INIT:
			blinkLED = 0x00;
			blink_cnt = 0x00;
			break;
		case BLINK_OFF:
			blinkLED = 0x00;
			++blink_cnt;
			break;
		case BLINK_ON:
			blinkLED = 0x08;
			++blink_cnt;
			break;
		default:
			break;
	}
}

void TickCombine(){
	switch(combineState){
		case COMBINE_INIT:
			combineState = COMBINED;
			break;
		case COMBINED:
			combineState = COMBINED;
			break;
		default:
			combineState = COMBINE_INIT;
			break;
	}
	switch(combineState){
		case COMBINE_INIT:
			break;
		case COMBINED:
			PORTB = threeLED + blinkLED;
			break;
		default:
			break;
	}
}
int main(void)
{
    /* Replace with your application code */
	DDRB = 0xFF; PORTB = 0x00;
	threeState = THREE_INIT;
	blinkState = BLINK_INIT;
	combineState = COMBINE_INIT;
	TimerSet(100);
	TimerOn();
    while (1) 
    {
		TickThree();
		TickBlink();
		TickCombine();
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

