/*
 * pins.h
 *
 * Created: 4/21/2018 2:04:28 PM
 *  Author: peter
 */ 


#ifndef PINS_H_
#define PINS_H_


#define DATA_D0_D7_DDR DDRC
#define DATA_D0_D7_PIN PINC
#define DATA_D0_D7_PORT PORTC
#define DATA_D8_D15_DDR DDRA
#define DATA_D8_D15_PIN PINA
#define DATA_D8_D15_PORT PORTA
#define ADDR_A0_A7_DDR DDRF
#define ADDR_A0_A7_PORT PORTF
#define ADDR_A8_A15_DDR DDRK
#define ADDR_A8_A15_PORT PORTK
#define ADDR_A16_A18_DDR DDRL
#define ADDR_A16_A18_PORT PORTL
#define ADDR_A16_A18_MASK 0b00000111
#define FLASH_CTRL_DDR DDRL
#define FLASH_CTRL_PORT PORTL
#define FLASH_CTRL_MASK 0b00111000
#define CE_PIN PL3
#define OE_PIN PL4
#define WE_PIN PL5


#endif /* PINS_H_ */