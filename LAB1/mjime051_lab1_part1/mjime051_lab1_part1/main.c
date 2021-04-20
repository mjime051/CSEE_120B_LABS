/*	Partner 1 Name & E-mail: Matthew Jimenez
 *	Partner 2 Name & E-mail:
 *	Lab Section: 21
 *	Assignment: Lab 1  Exercise 1
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned char lightSensor = 0x00;
	unsigned char garageSensor = 0x00;
	unsigned char LED = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		//the next line reads PA0
		garageSensor = PINA & 0x01;
		//the next line reads PA1
		lightSensor = PINA & 0x02;
		if (garageSensor == 0x01 && lightSensor == 0x00)
		{
			LED = (LED & 0xFC) | 0x01;
		}
		else {
			LED = (LED & 0xFC) | 0x02;
		}
		PORTB = LED;
    }
	return 0;
}

