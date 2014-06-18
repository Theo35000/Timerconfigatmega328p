/*
 * Mylib.h
 *
 *  Created on: 14 mai 2014
 *      Author: theo
 */

#ifndef MYLIB_H_
#define MYLIB_H_


#include <avr/io.h>  // Pour les définitions des registres
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<avr/delay.h>
#include <avr/interrupt.h>  /* Pour les interuptions*/
#include<stdarg.h>// POur fonction variadique
#include <inttypes.h>

/// UART SERIAL DEFINES
#define USART_BAUDRATE 9600
/// UART SERIAL DEFINES
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
//Prototype function
#define FOSC 16000000                       // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD -1
void
set_timer1 (char *format, ...);
void
set_timer0 (char *format, ...);
void
set_timer2 (char *format, ...);

#endif /* MYLIB_H_ */
