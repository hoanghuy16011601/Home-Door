#define  F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "check_button.h"
#include "I2C.h"
#include "I2C_LCD.h"
#include "external_interrupt.h"
#include "mfrc522.h"
#include "spi.h"
#include "timer0.h"


#define ENTER 23
#define CLEAR 22
#define INSERT_CARD 21
#define CHANGE_PASSWORD 20



uint8_t seconds = 0;
uint8_t number_id =0;
float error_position, error_before = 0;
float count =0;


void lcd_power_on();
void lcd_power_off();
uint8_t compare_string(unsigned char *string1, unsigned char *string2, uint8_t number_char);
void showString(PGM_P string);
void check_rfid();
void buzzer();
void check_pass();
void run_seconds();
uint8_t byte_2_str1(uint8_t byte);
uint8_t byte_2_str2(uint8_t byte);
void write_eep(uint16_t address, unsigned char data);
unsigned char read_eep(uint16_t address);
void right_password();
void warning();
uint8_t change_pass();
void insert_card();
void fast_pwm(uint16_t speed);
void fast_pwm_start();
void open_door();
void close_door();


int main(void)
{
	lcd_power_off();
	i2c_lcd_begin(0x80);
	showString(PSTR("HELLO"));
	/*
    unsigned char pass_init[] ="160102";
    unsigned char *pt = pass_init;
    for (uint8_t i = 0; i<6;i++)
    {
	    write_eep(i,*(pt+i));
    }
    DDRC |= (1<<0)|(1<<1)|(1<<2);
    PORTC &= ~((1<<0)|(1<<1)|(1<<2));
	DDRB |= (1<<0)|(1<<1);
	PORTB |=(1<<0);
	DDRD = 0x80;
	PORTD = 0x7F;
	pinchange_interrupt(PORT_D, 3);
	spi_init_master();
	mfrc522_init();
	mode_rfid = 1;
	while (1)
	{
		check_rfid();
	}
	*/
}


void check_rfid()
{
	buzzer();
	DDRD = 0x80;
	PORTD = 0x7F;
	uint8_t array[MAX_LEN];
	while(mode_rfid == 1)
	{
		uint8_t	byte = mfrc522_request(PICC_REQALL,array);
		byte = mfrc522_get_card_serial(array);
		if (byte == CARD_FOUND)
		{
			buzzer();
			lcd_power_on();
			uint8_t insert_id[4];
			uint8_t *pt = insert_id;
			for (byte=0;byte<4;byte++)
			{
				*(pt+byte) = array[byte];
			}
			uint8_t id_saved[4];
			pt = id_saved;
			for (uint8_t i =1;i<=number_id; i++ )
			{
				for (uint8_t j =0; j<4;j++ )
				{
					*(pt +j) = read_eep(2+4*i+j);
				}
				if (compare_string(insert_id,id_saved,4)==1)
				{
					right_password();
					goto OUT;
				}
			}
			i2c_lcd_begin(0x80);
			showString(PSTR ("WRONG CARD"));
			_delay_ms(2000);
			OUT:
			lcd_power_off();
		}

	}
}


void check_pass()
{
	DDRD = 0x0F;
	lcd_power_on();
	static uint8_t time_wrong =0;
	unsigned char *pass_saved = calloc(6, sizeof(unsigned char));
	unsigned char *pass_in = calloc(6, sizeof(unsigned char));
	if (pass_saved == NULL || pass_in == NULL)
	{
		i2c_lcd_begin(0x80);
		showString(PSTR("FULL SRAM"));
		return;
	}
	unsigned char num[]="0123456789*0#";
	unsigned char *number = num;
	INIT:
	for (uint8_t i =0; i<6; i++)
	{
		*(pass_saved + i) = read_eep(i);
	}
	PASS:
	i2c_lcd_clear();
	i2c_lcd_begin(0x80);
	showString(PSTR ("ENTER PASSWORD"));
	i2c_lcd_begin(0xC0);
	for (unsigned char i=0;i<10;i++)
	{
		unsigned char value_button = find_button();
		if (value_button == 23)
		{
			break;
		}
		if (value_button == 22)
		{
			for (uint8_t i=0;i<6;i++)
			{
				*(pass_in+i) =NULL;
			}
			goto PASS;	
		}
		if (value_button == 21)
		{
			if (compare_string(pass_saved,pass_in,6) == 1)
			{
				insert_card();
				i2c_lcd_clear();
				goto PASS;	
			}
			else
			{
				i2c_lcd_clear();
				i2c_lcd_begin(0x80);
				showString(PSTR("WRONG PASSWORD"));
				_delay_ms(3000);
			}
			goto PASS;
		}
		if (value_button == 20)
		{
			if (compare_string(pass_saved,pass_in,6) == 1)
			{
				uint8_t succes = change_pass();
				if (succes ==1)
				{
					goto INIT;
				}
			}
			else
			{
				i2c_lcd_clear();
				i2c_lcd_begin(0x80);
				showString(PSTR("WRONG PASSWORD"));
				_delay_ms(2000);
			}
			goto PASS;
		}
		if (value_button ==99)
		{
			goto OUT;
		}
		*(pass_in+i)=*(number + value_button);
		i2c_lcd_data(*(pass_in+i));
	}
	if (compare_string(pass_saved,pass_in,6)==1)
	{
		right_password();
		time_wrong =0;
		goto OUT;
	}
	else
	{
		i2c_lcd_clear();
		i2c_lcd_begin(0x80);
		showString(PSTR("WRONG PASSWORD"));
		_delay_ms(200);
		time_wrong+=1;
		if (time_wrong==3)
		{
			warning();
			goto OUT;
		}
		
		while(mode_rfid == 0)
		{
			if ((PIND&(1<<3))==0)
			{
				_delay_ms(100);
				goto PASS;
			}
		}
	} 
	OUT:
	free(pass_saved);
	free(pass_in);
	return;
}


void warning()
{
	i2c_lcd_clear();
	i2c_lcd_begin(0x80);
	showString(PSTR("WARNING"));
	buzzer();
}

uint8_t change_pass()
{
	unsigned char num[]="0123456789*0#";
	unsigned char *number = num;
	unsigned char number_in[6];
	unsigned char *pass_input = number_in;
	i2c_lcd_clear();
	i2c_lcd_begin(0x80);
	showString(PSTR("NEW PASS"));
	i2c_lcd_begin(0xC0);
	for (uint8_t i =0; i<6; i++)
	{
		uint8_t	value_button = find_button();
		
		if (value_button == CHANGE_PASSWORD)
		{
			goto OUT;
		}

		*(pass_input+i)=*(number+value_button);
		i2c_lcd_data(*(pass_input+i));
	}
	unsigned char number_out[6];
	unsigned char *new_pass = number_out;
	_delay_ms(100);
	i2c_lcd_clear();
	i2c_lcd_begin(0x80);
	showString(PSTR("NEW PASS AGAIN"));
	i2c_lcd_begin(0xC0);
	for (uint8_t i =0; i<6; i++)
	{
		uint8_t  value_button = 0;
		value_button = find_button();
		if (value_button == CHANGE_PASSWORD)
		{
			goto OUT;
		}
		*(new_pass+i)=*(number+value_button);
		i2c_lcd_data(*(new_pass+i));
	}
	i2c_lcd_clear();
	i2c_lcd_begin(0x80);
	if (compare_string(new_pass,pass_input,6) ==1)
	{
		showString(PSTR("PASS CHANGED"));
		for (uint8_t i = 0; i<6;i++)
		{
			write_eep(i,*(new_pass+i));
		}
		_delay_ms(2000);
		return 1;
		}
		else
		{
			showString(PSTR("ERROR"));
			_delay_ms(2000);
		}
		
	OUT:
	return 0;
}

void insert_card()
{
	i2c_lcd_clear();
	i2c_lcd_begin(0x80);
	uint8_t *new_id = calloc(4, sizeof(uint8_t));
	uint8_t *id_saved = calloc(4, sizeof(uint8_t));
	unsigned char *array = calloc(MAX_LEN,sizeof(unsigned char));
	if (new_id == NULL || id_saved == NULL || array == NULL)
	{
		showString(PSTR("FULL STACK"));
		return;
	}
	showString(PSTR("NEW ID:"));
	while(mode_rfid ==0)
	{
		uint8_t	byte = mfrc522_request(PICC_REQALL,array);
		byte = mfrc522_get_card_serial(array);
		if (byte == CARD_FOUND)
		{	
			buzzer();
			i2c_lcd_begin(0x88);
			for (uint8_t i=0; i<4; i++)
			{
				i2c_lcd_data(byte_2_str1(array[i]));
				i2c_lcd_data(byte_2_str2(array[i]));
				*(new_id+i) = array[i];
			}
			i2c_lcd_begin(0xC0);
			for (uint8_t k =1; k <= number_id; k++)
			{
				for (uint8_t h = 0; h<4; h++)
				{
					*(id_saved+h) = read_eep(2+4*k+h);  
				}
				if (compare_string(new_id,id_saved,4)==1)
				{
					showString(PSTR("ID HAS SAVED"));
					_delay_ms(2000);
					goto OUT;
					
				}
			}
			number_id +=1;
			for (uint8_t i=0; i<4;i++)
			{
				write_eep(2+4*number_id+i,array[i]);
			}
			showString(PSTR("SUCCESSFULLY"));
			_delay_ms(2000);
			OUT:
			free(array);
			free(new_id);
			free(id_saved);
			return;
		}
	}
	
}


uint8_t byte_2_str1(uint8_t byte)
{
	uint8_t byte1 = (byte&0xF0)>>4;
	uint8_t result1;
	if (byte1<=9)
	{
		result1 = byte1 + 48; 
	}else
	{
		result1 = byte1 + 55; 
	}
	return result1;
}

uint8_t byte_2_str2(uint8_t byte)
{
	uint8_t result2;
	uint8_t byte1 = byte&0x0F;
	if (byte1<=9)
	{
		result2 = byte1 + 48;
	}else
	{
		result2 = byte1 + 55;
	}
	return result2;
}



void write_eep(uint16_t address, unsigned char data)
{
	while(EECR&(1<<EEPE));
	EEAR = address;
	EEDR = data;
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
}

unsigned char read_eep(uint16_t address)
{
	while(EECR &(1<<EEPE));
	EEAR = address;
	EECR |= (1<<EERE);
	return EEDR;
}


void showString(PGM_P string)
{
	unsigned char c;
	while ((c = pgm_read_byte(string++)) != 0)
	i2c_lcd_data(c);
}


uint8_t compare_string(unsigned char *string1, unsigned char *string2, uint8_t number_char)
{
	for (uint8_t i=0; i<number_char; i++)
	{
		if (*(string1+i)!= *(string2+i))
		{
			return 0;
		}
	}
	return 1;
}

void buzzer()
{
	PORTC |= (1<<2);
	_delay_ms(50);
	PORTC &= ~(1<<2);
}


void lcd_power_on()
{
	i2c_initialization_master();
	i2c_start();
	i2c_write_master(0x27<<1);
	i2c_lcd_init_power();
	i2c_lcd_init(0x28,0x0F,0x10);
}

void lcd_power_off()
{
	i2c_lcd_turnoff();
	i2c_stop();
	
}

void open_door()
{
	i2c_lcd_clear();
	i2c_lcd_begin(0x84);
	showString(PSTR("WELCOME"));
	PORTB |= (1<<1);
	PORTC |= (1<<0);
	PORTC &= ~(1<<1);
	external_interrupt(INT0, RAISING);
	error_position = 90;
	TIMSK1 &= ~(1<<OCIE1A);
	OCR1A = 0;
	TCNT1 = 0;
	TCCR1A = (1<<COM1A1)|(1<<WGM10);
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
	run_millis();
	fast_pwm(255);
	while (error_position>0)
	{
	}
	PORTB &= ~(1<<1);
	TCCR1B = 0;
	count = 0;
	error_before =0;
	stop_millis();
	external_interrupt_stop(INT0);
	_delay_ms(5000);
	close_door();
}


void close_door()
{
	PORTB |= (1<<1);
	PORTC |= (1<<1);
	PORTC &= ~(1<<0);
	external_interrupt(INT0, RAISING);
	error_position = 90;
	TIMSK1 &= ~(1<<OCIE1A);
	OCR1A = 0;
	TCNT1 = 0;
	TCCR1A = (1<<COM1A1)|(1<<WGM10);
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
	run_millis();
	fast_pwm(255);
	while (error_position>0)
	{
	}
	PORTB &= ~(1<<1);
	TCCR1B =0;
	count = 0;
	error_before = 0;
	stop_millis();
	external_interrupt_stop(INT0);
}


void right_password()
{
	open_door();
}



void fast_pwm(uint16_t speed)
{
	if (speed >2000)
	{
		speed = 2000;
	}
	else if ( speed <0)
	{
		speed =0;
	}
	OCR1A = speed;
}




ISR(INT0_vect)
{
	count +=1;
}

ISR(TIMER0_COMPA_vect)
{
	milisecond +=1;
	if (milisecond  >=20 )
	{
		error_position = 90 - (count/750)*360;
		uint16_t speed = (uint16_t) 10*error_position + ((error_position - error_before)/0.02)*0.01;
		error_before = error_position;
		fast_pwm(speed);
		milisecond =0;
	}
}




ISR(PCINT2_vect)
{
	pinchange_interrupt_stop(PORT_D,3);
	mode_rfid = 0;
	run_seconds();
	_delay_ms(100);
	check_pass();
	mode_rfid = 1;
	key_flag = 1;
	seconds =0;
	lcd_power_off();
	PORTD = 0x7F;
	pinchange_interrupt(PORT_D,3);
	check_rfid();
}


void run_seconds()
{
	seconds =0;
	TIMSK1 |= (1<<OCIE1A);
	sei();
	OCR1A = 31249;
	TCCR1A = 0X00;
	TCCR1B = (1<<WGM12)|(1<<CS02);
	TCNT1 = 0;
}

ISR(TIMER1_COMPA_vect)
{
	seconds+=1;
	TCNT1 = 0;
	if (seconds >= 30)
	{
		mode_rfid =1;
		key_flag = 1;
		TIMSK1 &= ~(1<<OCIE1A);
		TCCR1B = 0;
	}
}