/*
 * Mylib.cpp
 *
 *  Created on: 14 mai 2014
 *      Author: theo
 */


#include <avr/io.h>  // Pour les définitions des registres
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<avr/delay.h>
#include <avr/interrupt.h>  /* Pour les interuptions*/
#include<stdarg.h>// POur fonction variadique
#include <inttypes.h>


/// prescaleur 'P' 8  / 64 / 256 / 1024    \3n
/// Mode 'A'   ---> Output 0C0A
/// \n        0 :  0C0A disable
/// \n        1 :  Normal Port Operation, 0C0A Disconnected
/// \n        2 :  Toggle 0C0A on Compare Match
/// \n        3 :  None-inverted mode (HIGH at bottom, LOW on Match)
/// \n        4 :  Inverted mode (LOW at bottom, HIGH on Match)
/// \n
/// \n Mode 'B'  ---> Output 0C0B
/// \n        0 :  0C0B disable
/// \n        1 :   None-inverted mode (HIGH at bottom, LOW on Match)
/// \n        2 :  Inverted mode (HIGH at bottom, LOW on Match)
/// \n
/// \n  Mode 'M'  --->TOP   |    Description
/// \n        0 :   /   |        Normal
/// \n        1 : 0xFF  |	 PWM Phase Corrected
/// \n        2 : 0CRA  |        CTC
/// \n        3 : 0xFF  |	 Fast PWM
/// \n        5 : OCR0A |	 PWM Phase Corrected
/// \n        7 : OCR0A |	 Fast PWM
/// \n
/// \n    Mode 4/5 reserved
/// \n DutyCycle 'd'       for  Both :  OC0A & OC0B
/// \n      0<DutyCycle<255         Then if it's 255 it's a 100% Duty Cycle,   If it's 128 it's 50% duty cycle
/// \n DutyCycle 'a'       For  OCRA
/// \n      0<DutyCycle<255
/// \n DutyCycle 'b'       For  OCRB
/// \n      0<DutyCycle<255
/// The frequency being chosen according to the mode chosen and the prescal
///\n   'E'  1  Enable using of interrupt vector with the function ISR(TIMER0_OVF_vect)
///\n        0 Disable
void

set_timer0 (char *format, ...)
{
int i = 0;
uint8_t MASK = 0x00;

TCNT0 = 0; //Init timer 1 a 0

// Creation pointeur -> liste des arguments
va_list args;
va_start(args, format);

for (i = 0; format[i]; i++)
  {
    switch (format[i])
      {

      case 'd':
	OCR0B = OCR0A = va_arg(args, uint16_t);

	break;
      case 'a':
	OCR0A = va_arg(args, uint16_t);
	break;
      case 'b':
	OCR0B = va_arg(args, uint16_t);
	break;
      case 'A':
	switch (va_arg(args, int))
	  {
	  MASK = 0b11000000;
	  MASK = ~MASK;
	  TCCR0A = TCCR0A & MASK;
	case 0:
	  TCCR0A = TCCR0A & MASK;
	  break;
	case 1:
	  TCCR0A |= (1 << COM0A0);
	  TCCR0B = (TCCR0B & 0b11110111);

	  break;
	case 2:
	  TCCR0A |= (1 << COM0A0);
	  TCCR0B = (TCCR0B & 0b11110111);
	  TCCR0B |= (1 << WGM02);
	  break;
	case 3:
	  TCCR0A |= (1 << COM0A1);
	  break;
	case 4:
	  TCCR0A |= (1 << COM0A1) | (1 << COM0A0);
	  break;
	default:
	  break;
	}
      break;
    case 'B':
      switch (va_arg(args, int))
	{
	MASK = 0b00110000;
	MASK = ~MASK;
	TCCR0A = TCCR0A & MASK;
      case 0:
	TCCR0A = TCCR0A & MASK;
	break;
      case 1:
	TCCR0A |= (1 << COM0B1);

	break;
      case 2:
	TCCR0A |= TCCR0A |= (1 << COM0B1) | (1 << COM0B0);

	break;
      default:
	break;
      }
    break;
  case 'M':
    switch (va_arg(args, int))
      {
      MASK = 0b00000011; // MASK of the TCCR0A register
      MASK = ~MASK;
      TCCR0A = TCCR0A & MASK;
      MASK = 0b11110111;
      TCCR0B = TCCR0B & MASK;
    case 1:
      TCCR0A |= (1 << WGM00);
      break;
    case 2:
      TCCR0A |= (1 << WGM01);
      break;
    case 3:
      TCCR0A |= (1 << WGM01) | (1 << WGM00);
      break;
    case 5:
      TCCR0A |= (1 << WGM00);
      TCCR0B |= (1 << WGM02);
      break;
    case 7:
      TCCR0A |= (1 << WGM01) | (1 << WGM00);
      TCCR0B |= (1 << WGM02);
      break;
    }
  break;
case 'P':
  switch (va_arg(args, int))
    {
    MASK = 0b00000111;
    MASK = ~MASK;
    TCCR0B = TCCR0B & MASK;
  case 1:
    TCCR0B |= (1 << CS00); // initialisation du timer sans prescaleur
    break;
  case 64: //prescaleur 64
// printf ("\n");
    TCCR0B |= (1 << CS01) | (1 << CS00);
    break;
  case 8:	      //prescaleur 8
    TCCR0B |= (1 << CS01);
    break;
  case 256:	      //prescaleur 256
    TCCR0B |= (1 << CS02);
    break;
  case 1024:	      //prescaleur 1024
    TCCR0B |= (1 << CS02) | (1 << CS00);
    break;
  default:
    break;

  }

break;
case 'E':
switch (va_arg(args, int))
  {
  case 1:
    TIMSK0 |= (1 << TOIE0);
    break;
  case 0:
    TIMSK0 = TIMSK0 & 0b11111110;
    break;
  default:
    break;
  }
break;
default:
break;

}
}
}


///\n Variadique function
///\n OUTPUT A --> Pin 9
///\n OUTPUT B --> Pin 10
///\n Prescaling 'P'  0 / 1 / 8 / 64 / 256 / 1024
///\n \n
///\n MODE 'M'  | Descripion                         |   TOP
///\n           |                                    |
///\n	0	| Normal 	                     |  0xFFFF
///\n   1	| PWM, Phase Corrected, 8bit	     |  0x00FF
///\n   2	| PWM, Phase Corrected, 9bit	     |  0x01FF
///\n   3	| PWM, Phase Corrected, 10bit        |  0x03FF
///\n   4	| Fast PWM, 8bit 	             |  0x00FF
///\n   5	| Fast PWM, 9bit 	             |  0x01FF
///\n   6	| Fast PWM, 10bit 	             |  0x03FF
///\n   7	| PWM, Phase and Frequency Corrected |	ICR1
///\n   8	| PWM, Phase and Frequency Corrected |	OCR1A
///\n   9	| PWM, Phase Correct 	             |  ICR1
///\n   10	| PWM, Phase Correct 	             |  OCR1A
///\n   11	| Fast PWM 	                     |  ICR1
///\n   12	| Fast PWM	                     | OCR1A
///\n
///\n Frequency 'f'It's the value you affect to ICR1 : Fixing frequency  |  0 to 0xFFF
///\n\n Mode 'A'   ---> Output 0C1A    &    Mode 'B'  ---> Output 0C1B
///\n        0 :  Normal port operation, OC1A/OC1B disconnected.
///\n        1 :  Mode  ( in mode 8,10,12)   only: Enable OCR1A only (OC1B disconnected)
///\n        2 :  	 None-inverted mode (HIGH at bottom, LOW on Match)
///\n        3 :  Inverted mode (LOW at bottom, HIGH on Match)
///\n\n
///\n DutyCycle 'd'       for  Both :  OCR1A & OCR1B
///\n      0<DutyCycle<65535
///\n DutyCycle 'a'       For  OCRA
///\n      0<DutyCycle<65535
///\n DutyCycle 'b'       For  OCRB
///\n      0<DutyCycle<65535
///\n
///\n   'E'  1  Enable using of interrupt vector with the function ISR(TIMER1_OVF_vect)
///\n        0 Disable

void
set_timer1 (char *format, ...)
{
  uint8_t MASK = 0x00;
  int i = 0;
  TCNT1 = 0; //Init timer 1 a 0

  // Creation pointeur -> liste des arguments
  va_list args;
  va_start(args, format);

  for (i = 0; format[i]; i++)
    {
      switch (format[i])
	{
	case 'f':
	  ICR1 = va_arg(args, uint16_t);

	  break;
	case 'd':
	  OCR1B = OCR1A = va_arg(args, uint16_t);

	  break;
	case 'a':
	  OCR1A = va_arg(args, uint16_t);
	  break;
	case 'b':
	  OCR1B = va_arg(args, uint16_t);
	  break;
	case 'A':
	  MASK = 0b00111111;
	  TCCR1A = TCCR1A & MASK;
	  switch (va_arg(args, int))
	    {
	    case 0:
	      TCCR1A = TCCR1A & MASK;
	      break;
	    case 1:
	      TCCR1A |= (1 << COM1A0);
	      break;
	    case 2:
	      TCCR1A |= (1 << COM1A1);
	      break;
	    case 3:
	      TCCR1A |= (1 << COM1A0) | (1 << COM1A1);
	      break;

	    default:
	      break;

	    }
	  break;

	case 'B':
	  MASK = 0b11001111;
	  TCCR1A = TCCR1A & MASK;
	  switch (va_arg(args, int))
	    {
	    case 0:
	      TCCR1A = TCCR1A & MASK;
	      break;
	    case 1:
	      TCCR1A |= (1 << COM1B0);
	      break;
	    case 2:
	      TCCR1A |= (1 << COM1B1);
	      break;
	    case 3:
	      TCCR1A |= (1 << COM1B0) | (1 << COM1B1);
	      break;

	    default:
	      break;

	    }
	  break;
	case 'M':
	  switch (va_arg(args, int))
	    {
	    MASK = 0b11100111;
	    TCCR1B = TCCR1B & MASK;
	    MASK = 0b11111100;
	    TCCR1A = TCCR1A & MASK;
	  case 0:
	    break;
	  case 1:
	    TCCR1A |= (1 << WGM10);

	    break;
	  case 2:
	    TCCR1A |= (1 << WGM11);
	    break;
	  case 3:
	    TCCR1A |= (1 << WGM11) | (1 << WGM10);
	    break;
	  case 4:
	    TCCR1A |= (1 << WGM10);
	    TCCR1B |= (1 << WGM12);
	    break;

	  case 5:
	    TCCR1A |= (1 << WGM11);
	    TCCR1B |= (1 << WGM12);
	    break;
	  case 6:
	    TCCR1A |= (1 << WGM11) | (1 << WGM10);
	    TCCR1B |= (1 << WGM12);
	    break;
	  case 7:
	    TCCR1B |= (1 << WGM13);
	    break;
	  case 8:
	    TCCR1A |= (1 << WGM10);
	    TCCR1B |= (1 << WGM13);
	    break;
	  case 9:
	    TCCR1A |= (1 << WGM11);
	    TCCR1B |= (1 << WGM13);
	    break;
	  case 10:
	    TCCR1A |= (1 << WGM11) | (1 << WGM10);
	    TCCR1B |= (1 << WGM13);
	    break;
	  case 11:
	    TCCR1A |= (1 << WGM11);
	    TCCR1B |= (1 << WGM13) | (1 << WGM12);
	    break;
	  case 12:
	    TCCR1A |= (1 << WGM11) | (1 << WGM10);
	    TCCR1B |= (1 << WGM13) | (1 << WGM12);
	    break;

	  default:
	    break;
	  }
	break;
      case 'P':
	MASK = 0b11111000;
	TCCR1B = TCCR1B & MASK;

	switch (va_arg(args, int))
	  {
	  case 0:
	    TCCR1B = TCCR1B & MASK;
	    break;
	  case 1:
	    TCCR1B |= (1 << CS10);
	    break;
	  case 64:

	    TCCR1B |= (1 << CS11) | (1 << CS10);
	    break;
	  case 8:
	    TCCR1B |= (1 << CS11);
	    break;
	  case 256:
	    TCCR1B |= (1 << CS12);
	    break;
	  case 1024:
	    TCCR1B |= (1 << CS12) | (1 << CS10);
	    break;
	  default:
	    break;

	  }
	break;
      case 'E':
	switch (va_arg(args, int))
	  {
	  case 1:
	    TIMSK1 |= (1 << TOIE1);
	    break;
	  case 0:
	    TIMSK1 = TIMSK1 & 0b11111110;
	    break;
	  default:
	    break;
	  }

	break;
      default:
	break;

      }
  }

va_end(args);

}/// OUTPUT B --> Pin 3
/// \n OUTPUT A Pin 11
/// prescaleur 'P' 8  / 35 / 64 / 128 / 256 / 1024    \n
/// Mode 'A'   ---> Output 0C2A
/// \n        0 :  0C2A disable
/// \n        1 :  Normal Port Operation, 0C2A Disconnected
/// \n        2 :  Toggle 0C2A on Compare Match
/// \n        3 :  None-inverted mode (HIGH at bottom, LOW on Match)
/// \n        4 :  Inverted mode (LOW at bottom, HIGH on Match)
/// \n
/// \n Mode 'B'  ---> Output 0C2B
/// \n        0 :  0C2B disable
/// \n        1 :   None-inverted mode (HIGH at bottom, LOW on Match)
/// \n        2 :  Inverted mode (HIGH at bottom, LOW on Match)
/// \n
/// \n Mode 'M'  --->TOP   |    Description
/// \n        0 :   /   |        Normal
/// \n        1 : 0xFF  |	 PWM Phase Corrected
/// \n        2 : 0CRA  |        CTC
/// \n        3 : 0xFF  |	 Fast PWM
/// \n        5 : OCR2A |	 PWM Phase Corrected
/// \n        7 : OCR2A |	 Fast PWM
/// \n    Mode 4/5 reserved
/// \n
/// \n DutyCycle 'd'       for  Both :  OC2A & OC2B
/// \n      0<DutyCycle<255         Then if it's 255 it's a 100% Duty Cycle,   If it's 128 it's 50% duty cycle
/// \n DutyCycle 'a'       For  OCRA
/// \n      0<DutyCycle<255
/// \n DutyCycle 'b'       For  OCRB
/// \n      0<DutyCycle<255
/// The frequency being chosen according to the mode chosen and the prescal
///\n   'E'  1  Enable using of interrupt vector with the function ISR(TIMER2_OVF_vect)
///\n        0 Disable

void
set_timer2 (char *format, ...)


{
int i = 0;
uint8_t MASK = 0x00;

TCNT2 = 0; //Init timer 1 a 0

// Creation pointeur -> liste des arguments
va_list args;
va_start(args, format);

for (i = 0; format[i]; i++)
  {
    switch (format[i])
      {

      case 'd':
	OCR2B = OCR2A = va_arg(args, uint16_t);
	break;
      case 'a':
	OCR2A = va_arg(args, uint16_t);
	break;
      case 'b':
	OCR2B = va_arg(args, uint16_t);
	break;
      case 'A':
	MASK = 0b11000000;
	MASK = ~MASK;
	TCCR2A = TCCR2A & MASK;
	switch (va_arg(args, int))
	  {

	  case 0:
	    TCCR2A = TCCR2A & MASK;
	    break;
	  case 1:
	    TCCR2A |= (1 << COM2A0);
	    TCCR2B = (TCCR2B & 0b11110111);

	    break;
	  case 2:
	    TCCR2A |= (1 << COM2A0);
	    TCCR2B = (TCCR2B & 0b11110111);
	    TCCR2B |= (1 << WGM22);
	    break;
	  case 3:
	    TCCR2A |= (1 << COM2A1);
	    break;
	  case 4:
	    TCCR2A |= (1 << COM2A1) | (1 << COM2A0);
	    break;
	  default:
	    break;
	  }
	break;
      case 'B':
	MASK = 0b00110000;
	MASK = ~MASK;
	TCCR2A = TCCR2A & MASK;
	switch (va_arg(args, int))
	  {

	  case 0:
	    TCCR2A = TCCR2A & MASK;
	    break;
	  case 1:
	    TCCR2A |= (1 << COM2B1);

	    break;
	  case 2:
	    TCCR2A |= (1 << COM2B1) | (1 << COM2B0);

	    break;
	  default:
	    break;
	  }
	break;
      case 'M':
	MASK = 0b00000011; // MASK of the TCCR0A register
	MASK = ~MASK;
	TCCR2A = TCCR2A & MASK;
	MASK = 0b11110111;
	TCCR2B = TCCR2B & MASK;
	switch (va_arg(args, int))
	  {

	  case 1:
	    TCCR2A |= (1 << WGM20);
	    break;
	  case 2:
	    TCCR2A |= (1 << WGM21);
	    break;
	  case 3:
	    TCCR2A |= (1 << WGM21) | (1 << WGM20);
	    break;
	  case 5:
	    TCCR2A |= (1 << WGM20);
	    TCCR2B |= (1 << WGM22);
	    break;
	  case 7:
	    TCCR2A |= (1 << WGM21) | (1 << WGM20);
	    TCCR2B |= (1 << WGM22);
	    break;
	  }
	break;
      case 'P':
	MASK = 0b00000111;
	MASK = ~MASK;
	TCCR2B = TCCR2B & MASK;
	switch (va_arg(args, int))
	  {

	  case 1:
	    TCCR2B |= (1 << CS20); // initialisation du timer sans prescaleur
	    break;
	  case 8:	      //prescaleur 8
	    TCCR2B |= (1 << CS21);
	    break;
	  case 32:
	    TCCR2B |= (1 << CS21) | (1 << CS20);

	    break;
	  case 64: //prescaleur 64
		   // printf ("\n");
	    TCCR2B |= (1 << CS22);
	    break;
	  case 128:
	    TCCR2B |= (1 << CS22) | (1 << CS20);

	    break;
	  case 256:	      //prescaleur 256
	    TCCR2B |= (1 << CS22) | (1 << CS21);
	    break;
	  case 1024:	      //prescaleur 1024
	    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	    break;
	  default:
	    break;

	  }
	break;
      case 'E':
	switch (va_arg(args, int))
	  {
	  case 1:
	    TIMSK2 |= (1 << TOIE2);
	    break;
	  case 0:
	    TIMSK2 = TIMSK2 & 0b11111110;
	    break;
	  default:
	    break;
	  }
	break;
      default:
	break;

      }
  }
}
