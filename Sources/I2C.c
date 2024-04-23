#define F_CPU 8000000UL
#include <avr/io.h>
#include "I2C.h"

void i2c_initialization_master()
{
	TWBR = 8;
	TWSR = (1<<TWPS0);
	TWCR = (1<<TWEN);
}

void i2c_initialization_slaver(unsigned char address)
{
	TWAR = address;
	TWCR =(1<<TWEN)|(1<<TWEA);
}

void i2c_start()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while(!(TWCR&(1<<TWINT)));
}

void i2c_stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

unsigned char i2c_det()
{
	unsigned char data;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR&(1<<TWINT)));
	data = TWSR & 0XF8;
	return data;
}

void i2c_write_master(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
}
void i2c_write_slaver(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR&(1<<TWINT)));
}


unsigned char i2c_read_master()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR&(1<<TWINT)));
	return(TWDR);
}

unsigned char i2c_read_slaver()
{
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
	return (TWDR);
}

void i2c_nak()
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
}