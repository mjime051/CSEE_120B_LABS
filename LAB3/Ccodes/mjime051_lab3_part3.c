/*	Partner 1 Name & E-mail: Matthew Jimenez, mjime051@ucr.edu
 *	Partner 2 Name & E-mail:
 *	Lab Section: 21
 *	Assignment: Lab 3  Exercise 3
 *	Exercise Description: In addition to part2, PA4 is 1 if a key is in the ignition, PA5 is one if a driver is seated, and PA6 is 1 if the driver's seatbelt is fastened. 
 *  PC7 should light a "Fasten seatbelt" icon if a key is in the ignition, the driver is seated, but the belt is not fastened.
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
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	// initialize to 0s
	unsigned char fuelLevel = 0x00;
	unsigned char carInfo = 0x00;
	unsigned char lightBits = 0x00;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		fuelLevel = PINA & 0x0F;
		carInfo = PINA & 0xF0;
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
		
		if ((GetBit(carInfo, 4) == 1) && (GetBit(carInfo, 5) == 1) && (GetBit(carInfo, 6) == 0))
		{
			lightBits = SetBit(lightBits, 7, 1);
		}
		
		PORTC = lightBits;
	}
}

