/*
 * Bluetooth + motors.c
 *
 * Created: 11/13/2018 10:43:15 AM
 * Author : Alvaro
 */ 


#include <stdio.h>	

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_ATmega1284.h"
#include "timer.h"

#define F_CPU 8000000UL					/* Define CPU Frequency e.g. here its 8 Mhz */
#include <avr/io.h>						/* Include AVR std. library file */
#include <util/delay.h>
#define ipin PINA
#define iport PORTA

void ADC_Init();
int ADC_Read(char);

void ADC_Init()
{
	DDRA = 0x00;		/* Make ADC port as input */
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC);	/* start conversion */
	while((ADCSRA &(1<<ADIF))== 0);	/* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF);	/* clear interrupt flag */
	ADC_value = (int)ADCL;	/* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */

	return ADC_value;		/* return digital value */
} 

/*
int ADC_Value;
int ADC_Value2;

enum States {start, toggle, send} state;

void tickSM()
{
	switch (state)
	{//Transitions
		case start:
		state = toggle;
		break;
		case toggle:
		state = send;
		break;
		case send:
		state = toggle;
		default:
		state = start;
		break;
	}
	switch(state)
	{//Actions
		case start:
		TimerSet(50);
		break;
		case toggle:
		
		
		if (USART_IsSendReady(0))
		{
			ADC_Value = ADC_Read(1);
			ADC_Value2 = ADC_Read(0);
			
			if ( ADC_Value < 400)
			{
				USART_Send(0x06,0);
				PORTB = 0x02;
				USART_Flush(0);
			}
			
			if ( ADC_Value > 1000)
			{
				USART_Send(0x09,0);
				PORTB = 0x02;
				USART_Flush(0);
			}
				
			
			if ( ADC_Value2 < 400) //forward
			{
				USART_Send(0x05,0);
				PORTB = 0x01;
				USART_Flush(0);
			}
			
			else if ( ADC_Value2 > 1000) //backward
			{
				USART_Send(0x0A,0);
				PORTB = 0x01;
				USART_Flush(0);
			}
			
			if ( (~PINC&0x03) == 0x01 )
			{
				USART_Send(0x10,0);
				PORTB = 0x03;
				USART_Flush(0);
			}
			
			if ( (~PINC&0x03) == 0x02 )
			{
				USART_Send(0x20,0);
				PORTB = 0x03;
				USART_Flush(0);
			}
			
			if ( (~PINC&0x03) == 0x03 )
			{
				USART_Send(0x30,0);
				PORTB = 0x03;
				USART_Flush(0);
			}
			
			
		}
		
		
		
		else  
		{
			//USART_Send(0x00,0);
			PORTB = 0x00;
		}
		
		break;
		case send:
		
		if (USART_IsSendReady(0) && !(ADC_Value< 400) && !(ADC_Value > 1000) && !(ADC_Value2< 400) && !(ADC_Value2 > 1000)&& !((~PINC&0x03)== 0x01)&& !((~PINC&0x03)== 0x02)&& !((~PINC&0x03)== 0x03))
		{
			USART_Send(0x00,0);
			PORTB = 0x00;
			USART_Flush(0);
		}
		
		break;
	}
}

int main(void)
{
	initUSART(0);
	
	ADC_Init();

	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0xFF;


	//TimerSet(50);
	TimerOn();
	

	while (1)
	{
		tickSM();
		
		while (!TimerFlag) {}
		TimerFlag = 0;
		
	}
}

*/


enum States { start, recieve, run } state;


void tickSM()
{
	switch (state) //transitions
	{
		case start:
		state = recieve;
		break;
		case recieve:
		state = run;
		break;
		case run:
		state = recieve;
		break;
		default:
		break;
		
	}
	
	switch (state) //actions
	{
		case start:
		TimerSet(3);
		break;
		case recieve:
		if ( USART_HasReceived(0) )
		{
			unsigned char tmp = 0x00;
			unsigned char tmp2 = 0x00;
			tmp = USART_Receive(0);
		
		
		
				if ( (~PINA&0x01) == 0 )
				{
					PORTB = tmp;
				}
				
				if ( (~PINA&0x01) == 1 )
				
				{
					PORTB = 0;
				}	
				
				
				
			if ( tmp == 0x20)
				{
					OCR1A = 165;
					
				}

			
			if ( tmp == 0x10)
			{
				
				OCR1A = 280;
			}
			
			if (tmp == 0x30)
			{
				PORTC = 0x01;
			}
			
			else 
			{
				PORTC = 0x00;
			}
			
		
			USART_Flush(0);
			break;
		
		}
		
		break;
		case run:
		//PORTA = tmp;
		break;
	}
	

	
}


int main(void)
{
	DDRD |= (1<<PD5);	
	TCNT1 = 0;		
	ICR1 = 2499;
	
	
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	
	DDRB = 0xFF; PORTB =0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	//DDRD = 0xFF, PORTD = 0x00;
	
	// DDRD = 0xFF; PORTD = 0x00;
	
	initUSART(0);
	
	//TimerSet(3);
	TimerOn();
	
	unsigned char temp = 0;
	while (1)
	{
		tickSM();
		while(!TimerFlag);
		TimerFlag = 0;
		
		
		
	}
	
	
}

