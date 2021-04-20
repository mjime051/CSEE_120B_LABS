/*
 * mjime051_lab9_part1.c
 *
 * Created: 5/3/2019 12:19:06 PM
 * Author : matt_
 */ 

#include <avr/io.h>

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM0A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum SM_STATES{INIT, WAIT, A0_PRESS, A1_PRESS, A2_PRESS} state;

void Tick(unsigned char button) {
	switch(state) {
		case INIT:
			state = WAIT;
			break;
		case WAIT:
			if (button == 0x01)
			{
				PWM_on();
				state = A0_PRESS;
			}
			else if (button == 0x02)
			{
				PWM_on();
				state = A1_PRESS;
			}
			else if (button == 0x04)
			{
				PWM_on();
				state = A2_PRESS;
			}
			else
			{
				state = WAIT;
			}
			break;
		case A0_PRESS:
			state = (button == 0x01) ? A0_PRESS : WAIT;
			break;
		case A1_PRESS:
			state = (button == 0x02) ? A1_PRESS : WAIT;
			break;
		case A2_PRESS:
			state = (button == 0x04) ? A2_PRESS : WAIT;
			break;
		default:
			state = WAIT;
			break;
	}
	
	switch(state){
		case INIT:
			break;
		case WAIT:
			set_PWM(0);
			break;
		case A0_PRESS:
			set_PWM(261.63);
			break;
		case A1_PRESS:
			set_PWM(293.66);
			break;
		case A2_PRESS:
			set_PWM(329.63);
			break;
		default:
			break;
	}
}
int main(void)
{
    /* Replace with your application code */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	unsigned char button = 0x00;
	state = INIT;
	//PWM_on();
	
    while (1) 
    {
		button = ~PINA & 0x07;
		Tick(button);
		//set_PWM((300));
		//PORTB = 0x08;
    }
}

