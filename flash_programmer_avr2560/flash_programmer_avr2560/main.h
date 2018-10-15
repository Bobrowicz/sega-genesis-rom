/*
 * main.h
 *
 * Created: 10/3/2018 9:03:28 PM
 *  Author: peter
 */ 


#ifndef MAIN_H_
#define MAIN_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include <clock_config.h>
#include <util/delay.h>
#include <pins.h>
#include <setup.h>
#include <avr/cpufunc.h>
#include <flash_mem.h>

#define TRUE  1
#define FALSE 0

void memory_good(void);
void serial_prints(char* str);
void echo(uint8_t ch);


#endif /* MAIN_H_ */