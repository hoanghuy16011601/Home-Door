#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "I2C.h"
#include "I2C_LCD.h"

#define RS 0
#define RW 1
#define E 2

void i2c_lcd_init_power()
{
	_delay_ms(50);
	i2c_lcd_command(0x30);
	_delay_ms(5);
	i2c_lcd_command(0x30);
	_delay_us(150);
	i2c_lcd_command(0x32);
}

void i2c_lcd_init(unsigned char func_set,unsigned char display_control, unsigned char mode_set)
{
	_delay_us(133);
	i2c_lcd_command(func_set);
	_delay_us(133);
	i2c_lcd_clear();
	_delay_us(2667);
	i2c_lcd_command(display_control);
	_delay_us(133);
	i2c_lcd_command(mode_set);
}

void i2c_lcd_turnoff()
{
	i2c_lcd_command(0x28);
	i2c_lcd_clear();
	i2c_lcd_command(0x08);
	i2c_lcd_command(0x04);
	
}

void i2c_lcd_clear()
{
	i2c_lcd_command(0x01);
	_delay_ms(2);
}

void i2c_lcd_begin(unsigned char position)
{
	i2c_lcd_command(position);
	_delay_us(50);
}


void i2c_lcd_command(unsigned char cmd)
{
	unsigned char data;
	data = (cmd & 0xF0)|0x08;
	i2c_write_master(data);
	data &= ~(1<<RS);
	i2c_write_master(data);
	data &= ~(1<<RW);
	i2c_write_master(data);
	data |= (1<<E);
	i2c_write_master(data);
	data &= ~(1<<E);
	i2c_write_master(data);
	_delay_us(200);
	
	data = (cmd<<4)|0x08;
	i2c_write_master(data);
	data &= ~(1<<RS);
	i2c_write_master(data);
	data &= ~(1<<RW);
	i2c_write_master(data);
	data |= (1<<E);
	i2c_write_master(data);
	data &= ~(1<<E);
	i2c_write_master(data);
}

void i2c_lcd_data(unsigned char cmd)
{
	unsigned char data;
	data = (cmd & 0xF0)|0x08;
	i2c_write_master(data);
	data |= (1<<RS);
	i2c_write_master(data);
	data &= ~(1<<RW);
	i2c_write_master(data);
	data |= (1<<E);
	i2c_write_master(data);
	data &= ~(1<<E);
	i2c_write_master(data);
	_delay_us(200);
	
	data = (cmd<<4)|0x08;
	i2c_write_master(data);
	data |= (1<<RS);
	i2c_write_master(data);
	data &= ~(1<<RW);
	i2c_write_master(data);
	data |= (1<<E);
	i2c_write_master(data);
	data &= ~(1<<E);
	i2c_write_master(data);
}

void i2c_lcd_string(unsigned char *str)
{
	for (int i=0;str[i]!=0;i++)
	{
		i2c_lcd_data(str[i]);
	}
}