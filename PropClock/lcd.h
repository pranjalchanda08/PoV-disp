
#define lcd PORTD
#define rs PD2
//#define rw PD2
#define en PD3

#define clrscr 0x01
#define Return_Home 0x02
#define Dec_Cursor 0x04
#define Inc_Cursor 0x06
#define Shift_Disp_Rt 0x05
#define Shift_Disp_Lt 0x07
#define Dis_Off_Cur_Off 0x08
#define Dis_Off_Cur_On 0x0A
#define Dis_On_Cur_Off 0x0C
#define Dis_On_Cur_On 0x0E
#define Dis_On_Cur_Blink 0x0F
#define Shift_Cur_Lt 0x10
#define Shift_Cur_Rt 0x14
#define Shift_EntDisp_Rt 0x1C
#define Shift_EntDisp_Lt 0x18


//-------------------------
void lcd_init();
void lcd_cmd(unsigned char com);
void lcd_data(unsigned char data);
void lcd_num(unsigned int value);
void lcd_string(char *str);
//--------

void lcd_cmd(unsigned char com)
{
	unsigned char cmd1;
	cmd1=(com&(0XF0));//UPPER 4 BIT DATA
	lcd=cmd1;
	lcd&=~(1<<rs);//rs 0
	//lcd&=~(1<<rw);//rw 0
	lcd|=(1<<en);
	_delay_ms(1);
	lcd&=~(1<<en);// en 0

	cmd1=((com<<4)&(0xF0));//LOWER 4 BIT DATA
	lcd=cmd1;
	lcd&=~(1<<rs);//rs 0
	//lcd&=~(1<<rw);//rw 0
	lcd|=(1<<en);
	_delay_ms(1);
	lcd&=~(1<<en);// en 0
}
void lcd_data(unsigned char data)
{
	unsigned char data1;
	data1=(data&(0XF0));//UPPER 4 BIT DATA
	lcd=data1;
	lcd|=(1<<rs);//rs 1
	//lcd&=~(1<<rw);//rw 1
	lcd|=(1<<en);//en 0
	_delay_ms(1);
	lcd&=~(1<<en);// en=0

	data1=((data<<4)&(0XF0));
	lcd=data1;
	lcd|=(1<<rs);//rs 1
	//lcd&=~(1<<rw);//rw 0
	lcd|=(1<<en);
	_delay_ms(1);
	lcd&=~(1<<en);// en 0
}
//------------------------------------
void lcd_string(char *str)
{
	int i=0;
	while(str[i]!='\0')
	{
		lcd_data(str[i]);
		i++;
	}
}


void lcd_num(unsigned int valu)
{
	unsigned int rem;
	lcd_cmd(0x04);
	while(valu!=0)
	{
		rem=valu%10;
		rem+=48;
		lcd_data(rem);
		valu/=10;
	}
	lcd_cmd(0x06);
}

void lcd_init()
{
	lcd_cmd(0x02);
	lcd_cmd(0x28);
	lcd_cmd(0x0E);
	lcd_cmd(0x06);
	lcd_cmd(Dis_On_Cur_Off);
}
