#include "timer0.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void run_millis()
{
	milisecond = 0;
	TIMSK0 |= (1<<OCIE0A);
	sei();
	TCNT0 =0;
	OCR0A = 124;
	TCCR0A &= ~((1<<WGM01)|(1<<WGM00));
	TCCR0A |= (1<<WGM01);
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
	TCCR0B |= (1<<CS01)|(1<<CS00);
}

void stop_millis()
{
	TIMSK0 &= ~(1<<OCIE0A);
}