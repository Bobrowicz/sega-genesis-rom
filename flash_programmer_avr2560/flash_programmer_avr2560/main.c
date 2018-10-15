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
	uint8_t write_flag = 0;
	uint8_t read_flag = 0;
	uint32_t base = 0x00;
	uint32_t base_address = base;
	uint16_t data = 0;
	uint16_t data_read = 0;
	uint8_t first_byte = 0xDE;
	uint8_t secon_byte = 0xAD;
	
	write_flag = 0;
	read_flag = 1;
	//flash_erase_entire_chip();
	//memory_good();
	PORTB &= (1 << PB7);
    while (1) 
    {
		//usart_printstr(ready_prompt);
		data = first_byte;
		data |= (secon_byte << 8);
		
		if (write_flag)
		{
			for (uint8_t i = 0; i < 16; i++)
			{
				
				flash_program_one_word(&base_address, &data);
				
				base_address++;
				
			}
			
			write_flag = 0;
			read_flag = 1;
		}
		_delay_ms(4000);
		base_address = 0x00;
		while (read_flag)
		{
			
			flash_read_one_word(&base_address, &data_read);
			echo(data_read);
			echo(data_read >> 8);
			base_address++;
			_delay_ms(100);
		}
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