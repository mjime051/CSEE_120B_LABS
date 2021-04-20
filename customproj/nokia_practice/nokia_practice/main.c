/*
 * nokia_practice.c
 *
 * Created: 6/3/2019 1:43:49 PM
 * Author : matt_
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "nokia5110.h"

int main(void)
{
	nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_write_string("IT'S WORKING!",1);
	nokia_lcd_set_cursor(42, 42);
	nokia_lcd_write_char('F', 1);
	nokia_lcd_render();

	for (;;) {
		_delay_ms(1000);
	}
}

