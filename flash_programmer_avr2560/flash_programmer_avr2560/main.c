/*
 * flash_programmer_avr2560.c
 *
 * Created: 10/3/2018 8:17:41 PM
 * Author : peter
 */ 


#include "main.h"

int main(void)
{
	_delay_ms(1000);
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRB |= (1 << PB7);
	
    while (1) 
    {
		
		ODD_BYTE ^= 0xBB;
		EVEN_BYTE ^= 0xBB;
		PORTB ^= (1 << PB7);
		_delay_ms(200);
	}
		
}

