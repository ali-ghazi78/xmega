/*
 * xmega_adc.c
 *
 * Created: 12/11/2018 6:26:38 PM
 * Author : ali
 */ 

/*
 * myFirstXmega.c
 *
 * Created: 1/24/2018 8:46:48 PM
 * Author : ali aqa ghazi
 */ 
#define F_CPU 8000000
#define F_SYS 32000000
#define F_TWI 100000
#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/delay.h"
#include <math.h>
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)
#define TWI_BAUDRATE TWI_BAUD(F_SYS, F_TWI)


#define TWI_ACK		0x00U
#define TWI_NACK	0x01U

#define ENTER (my_putchar('\n'))

#define PWM1 PIN0_bm
#define PWM2 PIN1_bm


#define LED_DOWN	PIN4_bm
#define LED_UP		PIN6_bm
#define LED_RIGHT	PIN7_bm
#define LED_LEFT	PIN5_bm
#define SCL_HIGH PORTE.OUTSET=2
#define SCL_LOW PORTE.OUTSET=2

#define SDA_HIGH PORTE.OUTSET=1
#define SDA_LOW PORTE.OUTCLR=1

#define KEY_UP		(PORTD.IN&PIN2_bm)
#define KEY_DOWN	(PORTD.IN&PIN1_bm)
#define KEY_LEFT	(PORTD.IN&PIN0_bm)
#define KEY_RIGHT	(PORTD.IN&PIN3_bm)

#define LED_DOWN_ON		(PORTC.OUTSET=LED_DOWN) 
#define LED_UP_ON		(PORTC.OUTSET=LED_UP)
#define LED_RIGHT_ON	(PORTC.OUTSET=LED_RIGHT)
#define LED_LEFT_ON		(PORTC.OUTSET=LED_LEFT)

#define LED_DOWN_OFF	(PORTC.OUTCLR=LED_DOWN)
#define LED_UP_OFF		(PORTC.OUTCLR=LED_UP)
#define LED_RIGHT_OFF	(PORTC.OUTCLR=LED_RIGHT)
#define LED_LEFT_OFF	(PORTC.OUTCLR=LED_LEFT)

#define MPU_ADDRESS 0x68
#define MPU_WHO_AM 0x75
#define GYRO_CONFIG 0x1B
#define SLEEP_CONFIG 0x6B
#define SAMPLE_RATE 0x19
#define USER_CONTROL 0x6A


#define SPI_P SPID
#define SPI_PORT PORTD
#define SPI_SCK  (1<<7)
#define SPI_MOSI (1<<5)
#define SPI_MISO (1<<6)
#define SPI_SS (1<<4)
#define SPI_SS_R (1<<4)//d4
#define SPI_SS_T (1<<4)//a4

#define SS_LOW (SPI_PORT.OUTCLR=SPI_SS)
#define SS_HIGH (SPI_PORT.OUTSET=SPI_SS)

#define SS_R_LOW (SPI_PORT.OUTCLR=SPI_SS_R)
#define SS_R_HIGH (SPI_PORT.OUTSET=SPI_SS_R)

#define SS_T_LOW (PORTA.OUTCLR=SPI_SS_T)
#define SS_T_HIGH (PORTA.OUTSET=SPI_SS_T)

#define R (ss=1)
#define T (ss=0)

#define CE_R_LOW (PORTC.OUTCLR=1<<0)//C0
#define CE_R_HIGH (PORTC.OUTSET=1<<0)//C0

#define CE_T_LOW (PORTC.OUTCLR=1<<1)//C1
#define CE_T_HIGH (PORTC.OUTSET=1<<1)//C1









void oled_write(unsigned char a);
void cmp_calibrate();
void test_led(void);
void test_key(void);
void init_pin(void);
void init_clock(void);
void my_putstr(char *p);
void my_putchar(unsigned char input_char);
void init_usart();
void init_usart_interupt();
void my_usart_asci();
void my_put_int( int number);
unsigned char my_getchar(void);
void pwm_init();
unsigned char is_ready_data=0;
unsigned char output_char1=0;



void adc_init();
unsigned int read_adc(unsigned char adc_pin);



ISR(USARTC0_RXC_vect)
{
	LED_UP_ON;
    unsigned char output_char1;
	output_char1=USARTC0.DATA;						//LOAD DATA INTO REG
	while((USARTC0.STATUS&USART_RXCIF_bm));		//IS READ COMPLETE
	is_ready_data=1;
	LED_UP_OFF;
}

int main(void)
{
	init_usart_interupt();
	init_pin();
	init_clock();
	test_led();
	adc_init();
	my_putchar('\n');	
	my_putstr("----------");
	my_putstr("starT");
	my_putstr("----------");
	my_putchar('\n');
	_delay_ms(500);
	
	
	while (1) 
    {
		LED_UP_ON;
		my_put_int(read_adc(0));
		my_putchar('\n');
		LED_UP_OFF;
		_delay_ms(100);

	}
}



void init_pin(void)
{
	PORTC.DIRSET=LED_RIGHT;
	PORTC.DIRSET=LED_LEFT;
	PORTC.DIRSET=LED_UP;
	PORTC.DIRSET=LED_DOWN;
	
	PORTD.DIRCLR=KEY_UP;
	PORTD.DIRCLR=KEY_DOWN;
	PORTD.DIRCLR=KEY_LEFT;
	PORTD.DIRCLR=KEY_RIGHT;
}
void test_led(void)
{
	for(int i=0;i<4;i++)
	{
		
		if(i==0)
		{
			PORTC.OUTTGL=LED_UP;
		}
		if(i==1)
		{	
			PORTC.OUTTGL=LED_RIGHT;
		}
		if(i==2)
		{	
			PORTC.OUTTGL=LED_DOWN;
		}
		if(i==3)
		{	
			PORTC.OUTTGL=LED_LEFT;
		}
		_delay_ms(50);
		
	}
	LED_LEFT_OFF;
	LED_RIGHT_OFF;
	LED_DOWN_OFF;
	LED_UP_OFF;
	_delay_ms(25);
	LED_RIGHT_ON;
	LED_UP_ON;
	LED_LEFT_ON;
	LED_DOWN_ON;
	_delay_ms(25);
	LED_LEFT_OFF;
	LED_RIGHT_OFF;
	LED_DOWN_OFF;
	LED_UP_OFF;
	_delay_ms(25);
	LED_RIGHT_ON;
	LED_UP_ON;
	LED_LEFT_ON;
	LED_DOWN_ON;
	_delay_ms(25);
	LED_LEFT_OFF;
	LED_RIGHT_OFF;
	LED_DOWN_OFF;
	LED_UP_OFF;
	_delay_ms(25);
	

}
void test_key(void)
{
	if(!(KEY_DOWN))
		LED_DOWN_ON;
	else
		LED_DOWN_OFF;
	
	if(!(KEY_UP))
		LED_UP_ON;
	else
		LED_UP_OFF;
	
	if(!(KEY_RIGHT))
		LED_RIGHT_ON;
	else
		LED_RIGHT_OFF;
	
	if(!(KEY_LEFT))
		LED_LEFT_ON;
	else
		LED_LEFT_OFF;
}
void adc_init()
{
	ADCA_CTRLB |= ADC_CURRLIMIT1_bm|ADC_CONMODE_bm; //, maximum sampling speed 1msps, resolution 12-bit right adjusted, signed mode
	ADCA_REFCTRL |= ADC_REFSEL0_bm; //INTVCC2 i.e. Vcc/1.6 is selected as reference voltage
	ADCA_EVCTRL = 0x00; //no event channel input is selected
	ADCA_PRESCALER |= ADC_PRESCALER2_bm; //prescaler 64 is selected and peripheral clock is 8MHz
	ADCA_INTFLAGS = ADC_CH0IF_bm; //clear interrupt flag by writing one to it
	ADCA_CH0_CTRL |= ADC_CH_INPUTMODE0_bm; //signed mode, single ended positive input selected
	ADCA_CTRLA=ADC_ENABLE_bm; //enable ADC
}
unsigned int read_adc(unsigned char adc_pin)
{
	unsigned int adc_val;
	
	ADCA_CH0_MUXCTRL |= (adc_pin%11)<<3; //select PIN1 as input for ADC
	ADCA_CTRLA|=ADC_CH0START_bm;			//start adc
	
	while(!(ADCA_INTFLAGS & ADC_CH0IF_bm));
	
	ADCA_INTFLAGS=ADC_CH0IF_bm;  //clear flag
	
	adc_val=ADCA_CH0RESL+ADCA_CH0RESH*256;
	return adc_val;
	
}

void init_clock(void)//SET 32MHZ
{
    CCP = CCP_IOREG_gc;             // disable register security for oscillator update
	OSC.CTRL|=1<<1;					//enable 32MHZ ."|" MIKONIM CHON 2MHZ HAM HAST
	while(!(OSC.STATUS&(1<<1)));	//wait for stablize
    CCP = CCP_IOREG_gc;             // disable register security for oscillator update
	CLK.CTRL=1;						//SELECT 32MHZ
	CCP = CCP_IOREG_gc;             // disable register security for oscillator update
	CLK.PSCTRL=CLK_PSBCDIV_4_1_gc;	//DIV FOR PERIPHERAL 4
	//YADET NARE CCP .KOLN CCP BAYAD BEZARI QABL AZ HAE KODOM TA BEHET EJEZA DASTRESI VA NEVESHTN BEDE VASE STATUE LAZEM NIS
}
void init_usart()
{
	PORTC.DIRSET=PIN3_bm;							//DON'T FORGET TO DO THIS OTHERWISE IT WON'T WORK
	PORTC.OUTSET=PIN3_bm;
	USARTC0.CTRLC=((1<<1)|(1));						//8 BIT NO PARITY 1 STOP BIT
	USARTC0.BAUDCTRLA=428%256;						//BAUD RATE CONTROLL 115200  BSELL
	USARTC0.BAUDCTRLB=((-7)<<4)|((int)(428/256));	//BAUD RATE CONTROLL 115200 -3=0B1101
	USARTC0.CTRLB=USART_TXEN_bm|USART_RXEN_bm;		//ENABLE TX AND RX AT FIRST ALL IS WHEN U FULL THE DATA BUFFER
	PORTC.DIRCLR=PIN2_bm;							//MAKE RX INPUT
}
void init_usart_interupt(void)
{
	init_usart();
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm; //programmable mask interupt control enable all level of interupt
	
	SREG=1<<7;////==sei() enable global interupt
	USARTC0.CTRLA=0b00110000;								//enable rx interupt with high peroirity
}
void my_usart_asci()
{
	my_putstr("----------asci table----------");
	for(unsigned int i=0;i<=254;i++)
	{
		my_put_int(i);
		my_putchar('\t');
		my_putchar(i);
		my_putchar('\n');
	}
	my_putstr("----------asci table----------");
}
void my_put_int(int number)
{
	 int counter=0;
	 if(number<0)
	 {
		my_putchar('-');
		number*=-1;
	 }
	 int temp=number;
	 while (temp)
	 {
		 counter++;
		 temp/=10;
	 }
	 temp=number;
	 while(counter&&counter!=1)
	 {
		my_putchar(number/pow(10,counter-1)+'0');
		if(counter>2)
			number%=(int)pow(10,counter-1);
		else
			break;
		counter--;
	 } 
	 my_putchar(temp%10+'0');
	 
}
void my_putchar(unsigned char input_char)
{
	while(!(USARTC0.STATUS&USART_DREIF_bm));		//HASE PREVOIUSE DATA SENT? data regisert is empty 1 yes 0 is full
	USARTC0.DATA=input_char;						//LOAD DATA INTO REG
	while(!(USARTC0.STATUS&USART_DREIF_bm));		//HASE PREVOIUSE DATA SENT? data regisert is empty 1 yes 0 is full
}
void my_putstr(char *p)
{
	for(int i=0;p[i]!='\0';i++)
	{
		my_putchar(p[i]);
	}
}
unsigned char my_getchar(void)
{
	unsigned char output_char;
	while(!(USARTC0.STATUS&USART_RXCIF_bm));		//HASE  DATA received? data regisert is empty 1 yes 0 is full
	output_char=USARTC0.DATA;						//LOAD DATA INTO REG
	return output_char;
}


