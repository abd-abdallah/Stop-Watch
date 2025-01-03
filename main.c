#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char SEC1=0;
unsigned char SEC2=0;
unsigned char MIN1=0;
unsigned char MIN2=0;
unsigned char HOUR1=0;
unsigned char HOUR2=0;

unsigned int g_tick_MIN=0;
unsigned int g_tick_HOUR=0;

//=======================TimerA1 for One SEC To Increment SEC and MIN and HOUR===============
ISR(TIMER1_COMPA_vect)
{

	if(SEC1==9)
	{
		SEC1=0;
		if(SEC2==5) SEC2=0;
		else SEC2++;
	}
	else SEC1++;


	if(g_tick_MIN==59)
	{
		if(MIN1==9)
		{
			MIN1=0;
			if(MIN2==5) MIN2=0;
			else MIN2++;
		}
		else MIN1++;
		g_tick_MIN=0;
	}
	else g_tick_MIN++;


	if(g_tick_HOUR==3599)
	{
		if(HOUR1==9)
		{
			HOUR1=0;
			if(HOUR2==5) HOUR2=0;
			else HOUR2++;
		}
		else HOUR1++;
		g_tick_HOUR=0;
	}
	else g_tick_HOUR++;
}

void TimerA1_CTC(void)
{
	DDRC |= 0x0F;
	DDRA |= 0x3F;

	TCNT1=0;
	OCR1A=15625;
	TCCR1A=(1<<FOC1A);
	TCCR1B= (1<<WGM12)|(1<<CS10)|(1<<CS11);
	TIMSK |= (1<<OCIE1A);
	SREG |=(1<<7);
}
//=================================Interrupt0 for Reset====================================

ISR(INT0_vect)
{
	SEC1=0;
	SEC2=0;
	MIN1=0;
	MIN2=0;
	HOUR1=0;
	HOUR2=0;
}

void INT0_Reset(void)
{
	DDRD  &= ~(1<<PD2);
	PORTD |= (1<<PD2);

	MCUCR |=(1<<ISC01);
	GICR |= (1<<INT0);
	SREG |= (1<<7);
}

//================================Interrupt1 for Paused======================

ISR(INT1_vect)
{
	TIMSK &= ~(1<<OCIE1A);
}


void INT1_paused(void)
{
	DDRB &= (1<<PD3);

	MCUCR |= (1<<ISC11) |(1<<ISC10);
	GICR |= (1<<INT1);
	SREG |= (1<<7);


}
//=================================Interrupt2 for Resumed================================

ISR(INT2_vect)
{
	TIMSK |= (1<<OCIE1A);
}

void INT2_resumed(void)
{
	DDRB &= (1<<PB2);
	PORTB |= (1<<PB2);

	MCUCSR &= ~(1<<ISC2);
	GICR |=(1<<INT2);
	SREG |= (1<<7);
}
//=================================main to present Digits ========================================
int main(void)
{

	TimerA1_CTC();
	INT0_Reset();
	INT1_paused();
	INT2_resumed();

	while(1)
	{

		PORTA |= (1<<PA0);
		PORTC =(PORTC & 0xF0) | SEC1;
		_delay_ms(5);
		PORTA &= ~(1<<PA0);

		PORTA |= (1<<PA1);
		PORTC =(PORTC & 0xF0) | SEC2;
		_delay_ms(5);
		PORTA &= ~(1<<PA1);

		PORTA |= (1<<PA2);
		PORTC =(PORTC & 0xF0) | MIN1;
		_delay_ms(5);
		PORTA &= ~(1<<PA2);

		PORTA |= (1<<PA3);
		PORTC =(PORTC & 0xF0) | MIN2;
		_delay_ms(5);
		PORTA &= ~(1<<PA3);

		PORTA |= (1<<PA4);
		PORTC =(PORTC & 0xF0) | HOUR1;
		_delay_ms(5);
		PORTA &= ~(1<<PA4);

		PORTA |= (1<<PA5);
		PORTC =(PORTC & 0xF0) | HOUR2;
		_delay_ms(5);
		PORTA &= ~(1<<PA5);


	}
}

