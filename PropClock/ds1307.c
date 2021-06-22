/*
 * ds1307.c
 *
 * Created: 06-02-2015 21:31:06
 *  Author: PC
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include <string.h>

void disp_time_date(void);

void USARTInit(uint16_t ubrr_value)
{
	UCSRA = 0;
	UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
	UCSRC = 1<<URSEL | 1<<UCSZ1 | 1<<UCSZ0 ;

	UBRRL = ubrr_value;
	UBRRH = (ubrr_value>>8);


}
/*
char USARTReadChar()
{
	char ch;
	while(!(UCSRA &(1<<RXC)))
	{
		//disp_time_date();
	}
	ch=UDR;
	return ch;
}*/

void USARTWriteChar(char data)
{
	while(!(UCSRA & (1<<UDRE)));
	
	UDR=data;
}

void init_i2c()
{
	TWSR = 0X00;
	TWBR = 0X47;
	TWCR = (1<<TWEN);
}

unsigned char read_i2c()
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}

void write_i2c(unsigned char ch)
{
	TWDR = ch;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR&(1<<TWINT)));
}

void start()
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while((TWCR &(1<<TWINT))==0);
}

void stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	_delay_ms(1);
}

void rtc_write(char dev_addr,char dev_loc,char dev_data)
{
	start();
	write_i2c(dev_addr);
	write_i2c(dev_loc);
	write_i2c(dev_data);
	stop();
	_delay_ms(10);
}

unsigned char rtc_read(char dev_addr,char dev_loc)
{
	char ch;

	start();

	write_i2c(dev_addr);
	write_i2c(dev_loc);

	start();
	write_i2c(dev_addr|0x01);
	ch = read_i2c();

	stop();
	return ch;
}

void disp_time_date(void)
{
	char ch;
	lcd_cmd(0x80);
	lcd_string("Time: ");
	
	ch = rtc_read(0xd0 , 0x02);
	lcd_data(ch/16+48);
	lcd_data(ch%16+48);
	lcd_data(':');
	
	ch = rtc_read(0xd0 , 0x01);
	lcd_data(ch/16+48);
	lcd_data(ch%16+48);
	lcd_data(':');
	
	ch = rtc_read(0xd0 , 0x00);
	lcd_data(ch/16+48);
	lcd_data(ch%16+48);
	
	lcd_cmd(0xC0);
	lcd_string("Date: ");
	
	ch = rtc_read(0xd0 , 0x04);
	lcd_data(ch/16+48);
	lcd_data(ch%16+48);
	lcd_data('/');
	
	ch = rtc_read(0xd0 , 0x05);
	lcd_data(ch/16+48);
	lcd_data(ch%16+48);
	lcd_data('/');
	
	ch = rtc_read(0xd0 , 0x06);
	lcd_data(ch/16+48);
	lcd_data(ch%16+48);
	
}

char rtchex(char a, char b)
{
	uint8_t hex=0;
	a=a<<4;
	b-=0x30;
	b&=0x0F;
	hex=(a+b)&0x00ff;
	//USARTWriteChar(hex);
	return hex;
	
}
char hr, min, sec;
int i,j;
char ReceivedByte;
char ar[100];
int RecieveFlag=0;
