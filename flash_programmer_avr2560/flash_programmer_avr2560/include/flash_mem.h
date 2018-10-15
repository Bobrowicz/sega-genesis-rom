/*
 * flash_mem.h
 *
 * Created: 10/11/2018 8:04:02 PM
 *  Author: peter
 */ 

#ifndef FLASH_MEM_H_
#define FLASH_MEM_H_

#include <avr/io.h>
#include <pins.h>
#include <avr/cpufunc.h>
#include <clock_config.h>
#include <util/delay.h>

#define SST_ID			0xBF    // SST Manufacturer's ID code   
#define SST_39SF040     0xB7    // SST 39SF040 device code     
#define TRUE			1
#define FALSE			0

// struct for Software Data Protection byte load sequence
struct SDP_command {
	const uint32_t *address;
	const uint8_t *data;
	uint8_t byte_count;
	};

uint8_t flash_check_id_SST_39SF040(void);

void flash_exit_id_SST_39SF040(void);
void flash_erase_entire_chip(void);
void flash_program_one_word(uint32_t *address, uint16_t *word);
void flash_read_one_word(uint32_t *address, uint16_t *data);


#endif /* FLASH_MEM_H_ */