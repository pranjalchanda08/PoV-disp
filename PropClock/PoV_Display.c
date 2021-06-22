/*
 * PropClock.c
 *
 * Created: 22-06-2021 11:57:07
 *  Author: Pranjal Chanda
 */

//*******************************************Definations******************************************
#define F_CPU 50000000UL
#define F_OSC 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define clock   0
#define data    1
#define latch   2
#define clr     3

#define SHIFT_LEN       8
#define USART_BAUDRATE  9600
#define BRC ((( F_OSC / ( USART_BAUDRATE * 16UL ))) - 1)

#define UpdateTime  0x1D
#define ShowString  0x2D
#define ShowTime    0x3D

volatile char str3[40]=" ";
char UReceivedByte ;
int URecieveFlag=0;

char hr, min, sec;
int i,j,i1=0;
char ReceivedByte;
int RecieveFlag=0;

int cmd;

void setLED(uint8_t *arr, uint8_t len);
void disp_time_date(void);
void TO(void);

//************************************Character Defination***************************************

typedef struct{
    uint8_t ch;
    uint8_t * arr;
    uint8_t len;
}char_map_t;

uint8_t A[4]={0xFE, 0x11, 0xFE, 0X00};
uint8_t B[4]={0xff, 0x89, 0x76, 0X00};
uint8_t C[4]={0x7E, 0x81, 0xC3, 0X00};
uint8_t D[4]={0xFF, 0x81, 0x7E, 0X00};
uint8_t E[4]={0xFF, 0x89, 0x81, 0X00};
uint8_t F[4]={0xFF, 0x09, 0x01, 0X00};
uint8_t G[4]={0xFE, 0x81, 0x72, 0X00};
uint8_t H[4]={0xFF, 0x18, 0xFF, 0X00};
uint8_t I[4]={0x81, 0xFF, 0x81, 0X00};
uint8_t J[4]={0x41, 0x81, 0x7F, 0X00};
uint8_t K[4]={0xFF, 0x18, 0xE7, 0X00};
uint8_t L[4]={0xFF, 0x80, 0x80, 0X00};
uint8_t M[6]={0xFF, 0x04, 0x08, 0X04, 0XFF, 0X00};
uint8_t N[6]={0xFF, 0x07, 0x38, 0XC0, 0XFF, 0X00};
uint8_t O[4]={0x7E, 0x81, 0x7E, 0X00};
uint8_t P[4]={0xFF, 0x09, 0x0f, 0x00};
uint8_t Q[5]={0x7E, 0xA1, 0xC1, 0X7E, 0X00};
uint8_t R[4]={0xFF, 0x39, 0xCF, 0X00};
uint8_t S[4]={0x86, 0x89, 0x71, 0X00};
uint8_t T[4]={0x01, 0xFF, 0x01, 0X00};
uint8_t U[4]={0xFF, 0x80, 0xFF, 0X00};
uint8_t V[4]={0x7F, 0x80, 0x7F, 0X00};
uint8_t W[6]={0xFF, 0x40, 0x20, 0X40, 0XFF, 0X00};
uint8_t X[6]={0xC3, 0x2C, 0x30, 0X2C, 0XC3, 0X00};
uint8_t Y[5]={0x83, 0x8C, 0x68, 0X1F, 0X00};
uint8_t Z[6]={0xC3, 0xA1, 0x91, 0X8D, 0XC3, 0X00};
uint8_t A0[4]={0xFF, 0x81, 0xFF, 0X00};
uint8_t A1[4]={0x82, 0xFF, 0x80, 0X00};
uint8_t A2[4]={0xF2, 0x91, 0xDE, 0X00};
uint8_t A3[4]={0x42, 0x91, 0x7E, 0X00};
uint8_t A4[4]={0x1F, 0x10, 0xFF, 0X00};
uint8_t A5[4]={0xDF, 0x91, 0xF1, 0X00};
uint8_t A6[4]={0xFF, 0x91, 0xF1, 0X00};
uint8_t A7[4]={0x01, 0x01, 0xfF, 0X00};
uint8_t A8[4]={0xFF, 0x91, 0xFF, 0X00};
uint8_t A9[4]={0x9F, 0x91, 0xFF, 0X00};
uint8_t A10[2]={0x00, 0x00};
uint8_t A11[3]={0x00, 0x66, 0x00};

char_map_t char_map[] =
{
    'A', A, sizeof(A),'B', B, sizeof(B),'C', C, sizeof(C),
    'D', D, sizeof(D),'E', E, sizeof(E),'F', F, sizeof(F),
    'G', G, sizeof(G),'H', H, sizeof(H),'I', I, sizeof(I),
    'J', J, sizeof(J),'K', K, sizeof(K),'L', L, sizeof(L),
    'M', M, sizeof(M),'N', N, sizeof(N),'O', O, sizeof(O),
    'P', P, sizeof(P),'Q', Q, sizeof(Q),'R', R, sizeof(R),
    'S', S, sizeof(S),'T', T, sizeof(T),'U', U, sizeof(U),
    'V', V, sizeof(V),'W', W, sizeof(W),'X', X, sizeof(X),
    'Y', Y, sizeof(Y),'Z', Z, sizeof(Z),'0', A0, sizeof(A0),
    '1', A1, sizeof(A1),'2', A2, sizeof(A2),'3', A3, sizeof(A3),
    '4', A4, sizeof(A4),'5', A5, sizeof(A5),'6', A6, sizeof(A6),
    '7', A7, sizeof(A7),'8', A8, sizeof(A8),'9', A9, sizeof(A9),
    ' ', A10, sizeof(A10),':', A11, sizeof(A11),
}
uint16_t char_map_len = sizeof(char_map)/sizeof(char_map_t);

//*****************************************************Shift Register Functions********************************************
void setLED(uint8_t ch)
{
    uint16_t i;
    uint8_t j;
    for (i = 0; i < char_map_len; i++)
    {
        if(ch == (char_map[i].ch))
        {
            for (j = 0; j < char_map[i].len; j++)
            {
                shift(char_map[i].arr[j]);
                display();
            }
        }
    }
}

void display()
{
    PORTC&=~(1<<latch);
    _delay_us(10);
    PORTC|=1<<latch;
    _delay_us(10);
    PORTC&=~(1<<latch);
}

void shift(int DATA)
{
    uint8_t i;

    for (i=0;i<SHIFT_LEN;i++)
    {

        PORTC&=~(1<<clock);
        _delay_us(10);

        if (DATA&0x80)
        {
            PORTC|=1<<data;
        }

        else
        {
            PORTC&=~(1<<data);
        }
        DATA=DATA<<1;

        PORTC|=1<<clock;
        _delay_us(10);
    }
}

void InitShift(void)
{
    PORTC&= ~(1<<clock);
    PORTC&= ~(1<<latch);
    PORTC&= ~(1<<data);
    PORTC|= (1<<clr);
}

void MC(void)
{
    PORTC&=~(1<<clr);
    _delay_us(10);
    PORTC|=(1<<clr);
}

//************************************************************Characterisations *******************************************
void Character(unsigned char ch)
{
    setLED(toupper(ch));
}
void String( char *msg)
{
    while (*msg!='\0')
    {
        Character(*msg);
        {
            msg++;
        }

    }
}

//*************************************************USART Functions************************************************

void USARTInit(uint16_t ubrr_value)
{
    UCSRA = 0;
    UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
    UCSRC = 1<<URSEL | 1<<UCSZ1 | 1<<UCSZ0 ;

    UBRRL = ubrr_value;
    UBRRH = (ubrr_value>>8);
}

char USARTReadChar()
{
    while(!(UCSRA & (1<<RXC)))
    {
        //Do nothing
    }
    return UDR;
}

void UART_SendChar(unsigned char ch)
{
    while(!(UCSRA&(1<<UDRE)));
    UDR=ch;
}

void UART_SendString(char *p)
{
    while(*p!='\0')
    {
        UART_SendChar(*p);
        p++;
    }
}

/********************************************************TWI Functions************************************************/

char RTChex(char x)
{
    x+=((x/10)*6);
    return x;
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
    char units,tens;

    ch = rtc_read(0xd0 , 0x02);
    units=ch&0x0f;
    tens =ch&0xf0;
    Character(tens);
    Character(units);
    Character(':');

    ch = rtc_read(0xd0 , 0x01);
    units=ch&0x0f;
    tens =ch&0xf0;
    Character(tens);
    Character(units);
    Character(':');

    ch = rtc_read(0xd0 , 0x00);
    units=ch&0x0f;
    tens =ch&0xf0;
    Character(tens);
    Character(units);

    //Date

    units=ch&0x0f;
    tens =ch&0xf0;
    Character(tens);
    Character(units);
    Character('/');

    ch = rtc_read(0xd0 , 0x05);
    units=ch&0x0f;
    tens =ch&0xf0;
    Character(tens);
    Character(units);
    Character('/');

    ch = rtc_read(0xd0 , 0x06);
    units=ch&0x0f;
    tens =ch&0xf0;
    Character(tens);
    Character(units);
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


char ch1;
//**************************************************Main Functions********************************************************
int main(void)
{
    DDRD =0b11111011;
    DDRB =0xff;
    DDRC=0xff;
    //GICR =(1<<INT0);
    init_i2c();
    USARTInit(BRC);
    sei();
    InitShift();

    while(1)
    {

        if (!(PIND&(1<<PD2)))
        {
            String(str3);
        }
        else
        {
            String("");
        }
    }
}

//****************************************************Interrupt Functions***************************************
ISR(USART_RXC_vect)
{
    ReceivedByte=UDR;
    if (ReceivedByte=='/')
    {
        if (i1>0)
        {
            i1--;
            str3[i1]=" ";
        }
    }
    else
    {
        str3[i1]=ReceivedByte;
        i1++;
    }
}

void TO()
{
    TCNT0=0x20;
    TCCR0=0x01;
    while((TIFR&0x01)==0);
    TCCR0=0;
    TIFR=0x01;
}

