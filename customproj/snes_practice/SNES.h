#ifndef __SNES_h__
#define  __SNES_h__

#include <avr/io.h>

// PORTA# just resolves to 0-7; 
#define SNES_CLOCK  PORTA0 
#define SNES_LATCH  PORTA1
#define SNES_DATA   PORTA2
#define SNES_PORT   PORTA
#define SNES_PIN    PINA

#define SNES_NONE        0
#define SNES_R          16
#define SNES_L         	32 
#define SNES_X          64 
#define SNES_A         128 
#define SNES_RIGHT     256   
#define SNES_LEFT      512  
#define SNES_DOWN     1024 
#define SNES_UP       2048 
#define SNES_START    4096 
#define SNES_SELECT   8192   
#define SNES_Y       16384  
#define SNES_B       32768  

//this will not be used but keeping it in case of future need. Output is instead PORTB
#define OUTPUT_PORT PORTC
//SNES 13-16 not used, so bits 0-3 not used.

void SNES_init(){
    SNES_PORT |= (0x01 << SNES_CLOCK);
    SNES_PORT |= (0x01 << SNES_LATCH);
}

unsigned short SNES_Read(){
    unsigned short snes_pressed = 0x0000;
      
    // Turn latch on and off. Send signal to SNES controller 
    SNES_PORT |= (0x01  << SNES_LATCH);
	SNES_PORT |= (0x01 << SNES_CLOCK);
    SNES_PORT &= ~(0x01 << SNES_LATCH);
    
    snes_pressed = (((~SNES_PIN) & (0x01 << SNES_DATA)) >> SNES_DATA);

    for(int i = 0; i < 16; i++){
        SNES_PORT &= ~(0x01 << SNES_CLOCK);
        snes_pressed <<= 1;
        snes_pressed |= ( ( (~SNES_PIN) & (0x01  << SNES_DATA) ) >> SNES_DATA);      
		SNES_PORT |= (0x01 << SNES_CLOCK);
    }
    return snes_pressed;
}

#endif