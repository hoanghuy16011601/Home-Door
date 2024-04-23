#include "external_interrupt.h"
#include <avr/interrupt.h>

void external_interrupt(char mask, char type)
{
	EIMSK |= (1<<mask);
	if (mask == INT0)
	{
		EICRA |= (type/2 << ISC01)|(type%2 << ISC00);
	}
	else
	{
		EICRA |= (type/2 << ISC11)|(type%2 << ISC10);
	}
	sei();
}

void external_interrupt_stop(char mask)
{
	EIMSK &= ~(1<<mask);
}

void pinchange_interrupt(char PORT, char PIN)
{
	PCICR |= (1<<PORT);
	if (PORT ==0)
	{
		PCMSK0 |= (1<<PIN);
	}
	if (PORT ==1)
	{
		PCMSK1 |= (1<<PIN);
	}
	if (PORT == 2)
	{
		PCMSK2 |= (1<<PIN);
	}
	sei();
}

void pinchange_interrupt_stop(char PORT, char PIN)
{
	if (PORT ==0)
	{
		PCMSK0 &= ~(1<<PIN);
	}
	if (PORT ==1)
	{
		PCMSK1 &= ~(1<<PIN);
	}
	if (PORT == 2)
	{
		PCMSK2 &= ~(1<<PIN);
	}
}