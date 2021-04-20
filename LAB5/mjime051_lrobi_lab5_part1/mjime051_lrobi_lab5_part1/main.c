/*	Partner 1 Name & E-mail: Matthew Jimenez, mjime051@ucr.edu
 *	Partner 2 Name & E-mail: Liam Robbins
 *	Lab Section: 21
 *	Assignment: Lab 5  Exercise 1
 *	Exercise Description: A car has a fuel-level sensor that sets PA3..PA0 to a value between 0 (empty) and 15 (full).  
 *	A series of LEDs connected to PB5..PB0 should light to graphically indicate the fuel level. 
 *  If the fuel level is 1 or 2, PB5 lights. If the level is 3 or 4, PB5 and PB4 light.
 *  Level 5-6 lights PB5..PB3. 7-9 lights PB5..PB2. 10-12 lights PB5..PB1. 13-15 lights PB5..PB0. 
 *  Also, PB6 connects to a "Low fuel" icon, which should light if the level is 4 or less.
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
	DDRB = 0xFF; PORTB = 0x00; // Configure port C's 8 pins as outputs,
	// initialize to 0s
	unsigned char fuelLevel = 0x00;
	unsigned char lightBits = 0x00;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		fuelLevel = PINA & 0x0F;
		lightBits = 0x00;
		// 2) Perform Computation
		if (fuelLevel > 0 && fuelLevel < 0x03)
		{
			lightBits = SetBit(lightBits, 5, 1);
		} 
		else if (fuelLevel == 0x03 || fuelLevel == 0x04)
		{
			lightBits = SetBit(lightBits, 5, 1);
			lightBits = SetBit(lightBits, 4, 1);
		} 
		else if (fuelLevel == 0x05 || fuelLevel == 0x06)
		{
			lightBits = lightBits | 0x38;
		} 
		else if (fuelLevel >= 0x07 && fuelLevel <= 0x09)
		{
			lightBits = lightBits | 0x3C;
		} 
		else if (fuelLevel >= 0x0A && fuelLevel <= 0x0C)
		{
			lightBits = lightBits | 0x3E;
		} 
		else if (fuelLevel >= 0x0D && fuelLevel <=0x0F)
		{
			lightBits = lightBits | 0x3F;
		}
		
		if (fuelLevel <= 0x04)
		{
			lightBits = SetBit(lightBits, 6, 1);
		}
		
		PORTB = lightBits;
	}
}