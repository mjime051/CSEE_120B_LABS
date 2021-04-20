/*	Partner 1 Name & E-mail: Matthew Jimenez, mjime051@ucr.edu
 *	Partner 2 Name & E-mail:
 *	Lab Section: 21
 *	Assignment: Lab 3  Exercise 1 
 *	Exercise Description: Count the number of 1s on ports A and B and output that number on port C. 
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
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char cntOnes = 0x00; 
	unsigned char button1 = 0x00;
	unsigned char button2 = 0x00;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		button1 = PORTA;
		button2 = PORTB;
		cntOnes = 0x00;
		// 2) Perform Computation
		for (unsigned char i = 0; i < 8; i++)
		{
			if (GetBit(button1, i) == 0x01)
			{
				cntOnes++;
			}
			if (GetBit(button2, i) == 0x01)
			{
				cntOnes++;
			}
		}
		PORTC = cntOnes;
	}
}

