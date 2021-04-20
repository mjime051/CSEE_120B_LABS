/*
 * mjime051_lab8_part1.c
 *
 * Created: 5/1/2019 10:28:19 AM
 * Author : matt_
 */ 

#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
    /* Replace with your application code */
	//DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	//DDRD = 0x00; PORTD = 0x00;
	ADC_init();
	unsigned short x = 0x00;
	unsigned char max = 127;
    while (1) 
    {
		x = ADC & 0x00FF;
		if(x >= (max/2)){
			PORTB = 0x01;	
		}
		else {
			PORTB = 0x00;	
		}
    }
}