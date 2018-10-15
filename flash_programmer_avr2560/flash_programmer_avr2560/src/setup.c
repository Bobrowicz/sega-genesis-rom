/*
 * setup.c
 *
 * Created: 4/21/2018 6:13:09 PM
 *  Author: peter
 */ 

#include <setup.h>

void system_init()
{
	device_init();
	i_o_init();
}


void device_init()
{
	//TIMER_0_init();
	USART_0_init();
}

void i_o_init()
{
	// Disable internal pull up resistors
	MCUCR |= (1 << PUD);
	
	// Set flash memory control lines to output
	FLASH_CTRL_DDR |= (1 << CE_PIN) | (1 << OE_PIN) | (1 << WE_PIN);
	
	// Flash memory control lines are active low, start disabled
	FLASH_CTRL_PORT |= (1 << CE_PIN) | (1 << OE_PIN) | (1 << WE_PIN);	
	
	// Set address bus pins A16 - A18 to output
	ADDR_A16_A18_DDR |= ADDR_A16_A18_MASK;
	
	// Set address bus pins A8 - A15 to output
	ADDR_A8_A15_DDR = 0xFF;
	
	// Set address bus pins A0 - A7 to output
	ADDR_A0_A7_DDR = 0xFF;
	
	// Set data bus pins D8 - D15 to output
	//DATA_D8_D15_DDR = 0xFF;
	
	// Set data bus pins D0 - D7 to output
	//DATA_D0_D7_DDR = 0xFF;
	
	DDRB |= (1 << PB7);
}