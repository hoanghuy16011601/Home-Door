#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdint.h>
#include "spi.h"

const char SS = 2, MOSI =3, MISO =4, SCK =5;


void spi_init_master()
{
	DDRB |= (1<<SS)|(1<<MOSI)|(1<<SCK);
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void spi_turnoff()
{
	DDRB &= ~((1<<SS)|(1<<MOSI)|(1<<SCK));
	SPCR &= 0;
}

void enable_spi()
{
	PORTB &= ~(1<<SS);
}


void disable_spi()
{
	PORTB |= (1<<SS);
}

uint8_t spi_transmit(uint8_t data)
{
	SPDR = data;
	while(!(SPSR&(1<<SPIF)));
	return SPDR;
	
}