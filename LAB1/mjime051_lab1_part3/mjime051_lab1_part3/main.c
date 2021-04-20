/*	Partner 1 Name & E-mail: Matthew Jimenez
 *	Partner 2 Name & E-mail:
 *	Lab Section: 21
 *	Assignment: Lab 1  Exercise 3
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
	unsigned char tmpc = 0x00;
    while (1)
    {
	    carSpace = PINA;
		tmpc = PINC; 
	    cntavail = 0x00; // clear cntavail
	    for (unsigned char i = 0;i < 4; i++)
	    {
		    if ((carSpace & (0x01 << i)) == 0)
		    {
			    cntavail++; // if PC3 ... PC0 have a 0 then the space is available so we increment cntavail
		    }
	    }
		if (cntavail == 0)
		{
			tmpc = (tmpc & 0x70) + 0x80; // if no spaces are available, set PC7 to 1 and clear the rest of PC
		} 
		else
		{
			tmpc = (tmpc & 0x70) + cntavail; // else just set tmpc to cntavail
		}
	    PORTC = tmpc; // set portc to tmpc
    }
}

