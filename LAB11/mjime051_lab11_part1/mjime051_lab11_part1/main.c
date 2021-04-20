/*
 * mjime051_lab11_part1.c
 *
 * Created: 5/13/2019 3:01:11 PM
 * Author : matt_
 */ 

#include <avr/io.h>
#include "io.c"
//#include <ucr/bit.h>
#include "bit.h"
//#include "keypad.h"

// Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
// If multiple keys pressed, returns leftmost-topmost one
// Keypad must be connected to port C
/* Keypad arrangement
        PC4 PC5 PC6 PC7
   col  1   2   3   4
row
PC0 1   1 | 2 | 3 | A
PC1 2   4 | 5 | 6 | B
PC2 3   7 | 8 | 9 | C
PC3 4   * | 0 | # | D
*/
unsigned char GetKeypadKey() {

	PORTC = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }

	// Check keys in col 2
	PORTC = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }

	// Check keys in col 3
	PORTC = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }

	// Check keys in col 4	
	PORTC = 0x7F; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }

	return('\0'); // default value
}

enum SM_STATES {INIT, WAIT, LED_DISPLAY}state;
	
void Tick()
{
	unsigned char x = GetKeypadKey();
	switch(state){
		case INIT:
			state = WAIT;
			break;
		case WAIT:
			state = LED_DISPLAY;
			break;
		case LED_DISPLAY:
			state = WAIT;
			break;
		default:
			state = INIT;
			break;
	}
	switch(state){
		case INIT:
			break;
		case WAIT:
			break;
		case LED_DISPLAY:
			//LCD_WriteData(x);
			PORTB = x;
			break;
		default:
			break;
			
	}
}

int main(void)
{
	unsigned char x;

	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRD = 0xFF; PORTD = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	//unsigned char a[] = "Hello"; //initialize a char array with string literal. Special case, OK
	LCD_init();
	   //LCD_DisplayString(1, a);

	while(1) {
		Tick();
	}
}

