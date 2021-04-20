/*
 * mjime051_lab3_part5.c
 *
 * Created: 4/10/2019 2:32:20 PM
 * Author : matt_
 */ 

#include <avr/io.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}


int main(void)
{
	DDRB = 0xFE; PORTB = 0x00;
	DDRD = 0x00; PORTD = 0x00;
	unsigned short weight = 0x00;
	unsigned char tmpB = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		tmpB = 0x00;
		weight = 0x00;
		//weight = PIND;
		//weight = PIND << 1;
		//weight = SetBit(weight, 0, PINB & 0x01);
		weight = (PIND << 1) + (PINB & 0x01);
		if (weight >= 70)
		{
			tmpB = SetBit(tmpB, 1, 0x01);
		} 
		else if ((weight > 5) && (weight < 70))
		{
			tmpB = SetBit(tmpB, 2, 0x01);
		}
		PORTB = tmpB;
    }
}

