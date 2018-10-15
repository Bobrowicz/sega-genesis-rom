/*
 * setup.h
 *
 * Created: 4/21/2018 6:12:57 PM
 *  Author: peter
 */ 


#ifndef SETUP_H_
#define SETUP_H_

#include <pins.h>
#include <avr/io.h>
//#include <TC_0.h>
#include <USART_0.h>

void system_init(void);
void device_init(void);
void i_o_init(void);


#endif /* SETUP_H_ */