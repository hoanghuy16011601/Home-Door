#define F_CPU 16000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "check_button.h"
#include "I2C.h"
#include "I2C_LCD.h"

unsigned char key_flag;
unsigned char key_flag_check;
unsigned char k;
unsigned char j;
unsigned char value;



void waitkey()
{
	unsigned char value_scan = 0xfe;
	for (int i=0;i<4;i++)
	{
		PORTD = value_scan;
		_delay_us(1);
		if ((PIND & 0XF0)!=0xF0)
		{
			key_flag =1;
			j = i;
			value = value_scan;
			break;
		}
		else
		{
			value_scan = value_scan<<1;
			value_scan +=0x01;
		}
	}
}

void check_key()
{
	unsigned char value_scan = 0x10;
	for (int i=0;i<4;i++)
	{
		if((PIND&value_scan)==0)
		{
			k=i+1;
			key_flag_check =1;
			break;
		}
		value_scan = value_scan <<1;
		
	}
}

int find_button()
{
	key_flag_check = 0;
	key_flag = 0;
	loop: while(key_flag==0)
	{
		waitkey();
	}
	if (mode_rfid ==1)
	{
		return 99;
	}
	check_key();
	_delay_ms(200);
	check_key();
	if (key_flag_check ==0)
	{
		key_flag =0;
		goto loop;
	}
	if (k<4&&k>=1)
	{
		return k+3*j;
	}
	if (k==4)
	{
		return 5*k+j;
	}
	
}
