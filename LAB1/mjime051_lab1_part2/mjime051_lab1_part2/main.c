/*	Partner 1 Name & E-mail: Matthew Jimenez
 *	Partner 2 Name & E-mail:
 *	Lab Section: 21
 *	Assignment: Lab 1  Exercise 2
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char cntavail = 0x00;
	unsigned char carSpace = 0x00;
    /* Replace with your application code */
    while (1) 
    {	
		carSpace = PINA;
		cntavail = 0x00;
		for (unsigned char i = 0;i < 4; i++)
		{
			if ((carSpace & (0x01 << i)) == 0)
			{
				cntavail++;
			}
		}
		PORTC = cntavail;
    }
}

