/*
 * eeprom_practice.c
 *
 * Created: 5/28/2019 10:05:32 AM
 * Author : matt_
 */ 

#include <avr/eeprom.h>
unsigned char EEMEM NonVolatileChar = 0x02;
uint16_t EEMEM NonVolatileInt;
uint8_t EEMEM NonVolatileString[10];
int main(void)
{
	DDRD = 0xFF; PORTD = 0x00;
	unsigned char SRAMchar;
	uint16_t SRAMint;
	uint8_t SRAMstring;
	unsigned char hiScore = eeprom_read_byte(&NonVolatileChar);
	if (hiScore == 0xFF)
	{
		hiScore = 5;
		eeprom_write_byte(&NonVolatileChar,hiScore);
	}
	//eeprom_write_byte(&NonVolatileChar, 0x08);
	SRAMchar = eeprom_read_byte(&NonVolatileChar);
	SRAMint = eeprom_read_word(&NonVolatileInt);
	eeprom_read_block((void*)&SRAMstring, (const void*)&NonVolatileString, 10);
	while(1){
		PORTD = SRAMchar;
	}
}
