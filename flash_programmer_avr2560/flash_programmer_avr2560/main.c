/*
 * flash_programmer_avr2560.c
 *
 * Created: 10/3/2018 8:17:41 PM
 * Author : peter
 */ 


#include "main.h"

int main(void)
{
	_delay_ms(500);
	system_init();
	
	//uint8_t in_char = 0;
	uint32_t base_address = 0x00;
	uint8_t offset = 0x00;
	
	uint16_t data = 0;
	uint16_t data_read = 0;
	//uint8_t first_byte = 0xDE;
	//uint8_t secon_byte = 0xAD;
	
	uint8_t control_pins_status = 0;
	const uint8_t control_mask = 0b01110000;
	
	control_pins_status = PINB & control_mask;
	PORTB &= ~(1 << PB7);
	_delay_ms(2000);
	if (control_pins_status == 0x70)
	{
		flash_erase_entire_chip();
	}
	
	if (control_pins_status == 0x20)
	{
		for (uint32_t address = 0x00; address < 0x40000; address++)
		{
			data = USART_0_read();
			//PORTB |= (1 << PB7);
			data |= (USART_0_read() << 8);
			//PORTB &= ~(1 << PB7);
			flash_program_one_word(&address, &data);
		}
	}
	DDRB |= (1 << PB7);
	if (control_pins_status == 0x00)
	{
		
		for (uint32_t address = base_address; address < 0x40000/*(base_address + offset)*/; address++)
		{
			
			flash_read_one_word(&address, &data_read);
			
			echo(data_read);
			echo(data_read >> 8);
			//_delay_ms(1);
			
		}
	}
	
	
    while (1) 
    {
		_delay_ms(1000);

		PORTB ^= (1 << PB7);
	}
}

void echo(uint8_t ch)
{
	USART_0_write(ch);
}

void serial_prints(char* str)
{
	while (*str)
	{
		USART_0_write(*str);
		str++;
	}
}

void memory_good(void)
{
	if (flash_check_id_SST_39SF040())
	{
		serial_prints("Memory OK");
	}
	else
	{
		serial_prints("Memory Error");
	}
}