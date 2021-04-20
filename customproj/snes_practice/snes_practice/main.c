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

#define OUTPUT_PORT PORTB
//SNES 13-16 not used, so bits 0-3 not used.

void SNES_init(){
    SNES_PORT |= (0x01 << SNES_CLOCK);
    SNES_PORT |= (0x01 << SNES_LATCH);
}

unsigned short SNES_Read(){
    unsigned short snes_pressed = 0x0000;
      
    // Turn latch on and off. Send signal to SNES controller 
      
    // FROM JChristy, Part 5:   
    /* 
        Every 16.67ms (or about 60Hz), the SNES CPU sends out a 12us wide, positive
        going data latch pulse on pin 3. This instructs the ICs in the controller
        to latch the state of all buttons internally.  
    */
	//SNES_PORT &= ~(0x01 << SNES_LATCH);
    SNES_PORT |= (0x01  << SNES_LATCH);
	SNES_PORT |= (0x01 << SNES_CLOCK);
    SNES_PORT &= ~(0x01 << SNES_LATCH);
    
    // FROM JChristy, Part 5 and 6:
    /*
        Each button on the controller is assigned a specific id which corresponds
        to the clock cycle during which that button's state will be reported.
        The table in section 6.0 lists the ids for all buttons. Note that
        multiple buttons may be depressed at any given moment. Also note
        that a logic "high" on the serial data line means the button is NOT
        depressed.
    */
    
    snes_pressed = (((~SNES_PIN) & (0x01 << SNES_DATA)) >> SNES_DATA);
    
    // For 16 clock cycles the controller outputs the keys pressed, 
	// but first one is a bit different and some not used.
	// See JChristy Part 6. 
    for(int i = 0; i < 16; i++){
        SNES_PORT &= ~(0x01 << SNES_CLOCK);
        snes_pressed <<= 1;
        snes_pressed |= ( ( (~SNES_PIN) & (0x01  << SNES_DATA) ) >> SNES_DATA);      
		SNES_PORT |= (0x01 << SNES_CLOCK);
    }
    return snes_pressed;
}

int main(void){
	DDRA = 0x03; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned char temp;
	unsigned short button_pressed = 0x0000;
	SNES_init();
	while (1)
	{
		temp = 0x00;
		button_pressed = (SNES_Read());
		if ((button_pressed & 0) == 0)
		{
			temp = 0x00;
		}
		if ((button_pressed & 16) == 16)//SNES_R
		{
			temp = 0x01;
		}
		if ((button_pressed & 32) == 32)//SNES_L
		{
			temp = 0x02;
		}
		if ((button_pressed & 64) == 64)//SNES_X
		{
			temp = 0x03;
		}
		if ((button_pressed & 128) == 128)//SNES_A
		{
			temp = 0x04;
		}
		if ((button_pressed & 256) == 256)//SNES_RIGHT
		{
			temp = 0x05;
		}
		if ((button_pressed & 512) == 512)//SNES_LEFT
		{
			temp = 0x06;
		}
		if ((button_pressed & 1024) == 1024)//SNES_DOWN
		{
			temp = 0x07;
		}
		if ((button_pressed & 2048) == 2048)//SNES_UP
		{
			temp = 0x08;
		}
		if ((button_pressed & 4096) == 4096)//SNES_START
		{
			temp = 0x09;
		}
		if ((button_pressed & 8192) == 8192)//SNES_SELECT
		{
			temp = 0x0A;
		}
		if ((button_pressed & 16384) == 16384)//SNES_Y
		{
			temp = 0x0B;
		}
		if ((button_pressed & 32768) == 32768)//SNES_B
		{
			temp = 0x0C;
		}
		PORTB = temp;
	}
}