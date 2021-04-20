/*	Partner 1 Name & E-mail: Matthew Jimenez, mjime051@ucr.edu
 *	Partner 2 Name & E-mail:
 *	Lab Section: 21
 *	Assignment: Lab 3  Exercise 4
 *	Exercise Description: Read an 8-bit value on PA7..PA0 and write that value on PB3..PB0PC7..PC4. 
 *  That is to say,  take the upper nibble of PINA and map it to the lower nibble of PORTB, 
 *  likewise take the lower nibble of PINA and map it to the upper nibble of PORTC (PA7 -> PB3, PA6 -> PB2, … PA1 -> PC5, PA0 -> PC4).
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
 

#include <avr/io.h>


// Bit-access function
unsigned char SetBit(unsigned char changedBin, unsigned char position, unsigned char newBit) {
	return (newBit ? changedBin | (0x01 << position) : changedBin & ~(0x01 << position));
}
unsigned char GetBit(unsigned char position, unsigned char k) {
	return ((position & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; //Configure port B's 8 pins as outputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpB = 0x00;
	unsigned char tmpC = 0x00;
	while(1)
	{
		tmpB = 0x00;
		tmpC = 0x00;
		for (unsigned char i = 0x00; i < 4; i++)
		{
			tmpB = SetBit(tmpB, i, GetBit(PINA, 4 + i));
			tmpC = SetBit(tmpC, 4+i, GetBit(PINA, i));
		}
		PORTB = tmpB;
		PORTC = tmpC;
	}
}

