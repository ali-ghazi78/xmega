/*
 * i2c.c
 *
 * Created: 9/13/2018 8:31:16 PM
 * Author : ali
 */ 

/**********************************************************

Software I2C Library for AVR Devices.

Copyright 2008-2012
eXtreme Electronics, India
www.eXtremeElectronics.co.in
**********************************************************/

#include <avr/io.h>
#include <util/delay.h>


#define Q_DEL _delay_loop_2(3)
#define H_DEL _delay_loop_2(5)


#define SCLPORT(port)	(port.OUT)	//TAKE PORTD as SCL OUTPUT WRITE
#define SCLDDR(ddr)	(ddr.DIR)	//TAKE DDRB as SCL INPUT/OUTPUT configure

#define SDAPORT(port)	(port.OUT)	//TAKE PORTD as SDA OUTPUT WRITE
#define SDADDR(ddr)	(ddr.DIR)	//TAKE PORTD as SDA INPUT configure

#define SDAPIN(port)	port.IN	//TAKE PORTD TO READ DATA
#define SCLPIN(port)	port.IN	//TAKE PORTD TO READ DATA

#define SCL(pin)	pin		//PORTD.0 PIN AS SCL PIN
#define SDA(pin)	pin		//PORTD.1 PIN AS SDA PIN


#define  SOFT_I2C_SDA_LOW(ddr,pin_number)	SDADDR(ddr)|=((1<<pin_number))
#define SOFT_I2C_SDA_HIGH(ddr,pin_number)	SDADDR(ddr)&=(~(1<<pin_number))

#define SOFT_I2C_SCL_LOW(ddr,pin_number)	SCLDDR(ddr)|=((1<<pin_number))
#define SOFT_I2C_SCL_HIGH(ddr,pin_number)	SCLDDR(ddr)&=(~(1<<pin_number))



void i2c_init(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin)
{
	
	
	SDAPORT(i2c_port)&=(1<<SDA(i2c_sda_pin));
	SCLPORT(i2c_port)&=(1<<SCL(i2c_scl_pin));
	
	SOFT_I2C_SDA_HIGH(i2c_port,i2c_sda_pin);
	SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
	
}
void i2c_start(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin)
{
	 SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
	H_DEL;
	
	  SOFT_I2C_SDA_LOW(i2c_port,i2c_sda_pin);
	H_DEL;
}

void i2c_stop(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin)
{
	 SOFT_I2C_SDA_LOW(i2c_port,i2c_sda_pin);
	H_DEL;
	 SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
	Q_DEL;
	 SOFT_I2C_SDA_HIGH(i2c_port,i2c_sda_pin);
	H_DEL;
}

uint8_t i2c_write(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin,uint8_t data)
{
	
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		  SOFT_I2C_SCL_LOW(i2c_port,i2c_scl_pin);
		Q_DEL;
		
		if(data & 0x80)
		 SOFT_I2C_SDA_HIGH(i2c_port,i2c_sda_pin);
		else
		 SOFT_I2C_SDA_LOW(i2c_port,i2c_sda_pin);
		
		H_DEL;
		
		 SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
		H_DEL;
		
		while((SCLPIN(i2c_port) & (1<<SCL(i2c_scl_pin)))==0);
		
		data=data<<1;
	}
	
	//The 9th clock (ACK Phase)
	  SOFT_I2C_SCL_LOW(i2c_port,i2c_scl_pin);
	Q_DEL;
	
	 SOFT_I2C_SDA_HIGH(i2c_port,i2c_sda_pin);
	H_DEL;
	
	 SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
	H_DEL;
	
	uint8_t ack=!(SDAPIN(i2c_port) & (1<<SDA(i2c_sda_pin)));
	
	  SOFT_I2C_SCL_LOW(i2c_port,i2c_scl_pin);
	H_DEL;
	
	return ack;
	
}
uint8_t i2c_read(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin,uint8_t ack)
{
	uint8_t data=0x00;
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		
		  SOFT_I2C_SCL_LOW(i2c_port,i2c_scl_pin);
		H_DEL;
		 SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
		H_DEL;
		
		while((SCLPIN(i2c_port) & (1<<SCL(i2c_scl_pin)))==0);
		
		if(SDAPIN(i2c_port) &(1<<SDA(i2c_sda_pin)))
		data|=(0x80>>i);
		
	}
	
	  SOFT_I2C_SCL_LOW(i2c_port,i2c_scl_pin);
	Q_DEL;						//Soft_I2C_Put_Ack
	
	if(ack)
	{
		 SOFT_I2C_SDA_LOW(i2c_port,i2c_sda_pin);
	}
	else
	{
		 SOFT_I2C_SDA_HIGH(i2c_port,i2c_sda_pin);
	}
	H_DEL;
	
	 SOFT_I2C_SCL_HIGH(i2c_port,i2c_scl_pin);
	H_DEL;
	
	  SOFT_I2C_SCL_LOW(i2c_port,i2c_scl_pin);
	H_DEL;
	
	return data;
	
}




unsigned char i2c_master_write(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin,unsigned char address,unsigned char reg,unsigned char data)
{
	unsigned char ack=0;
	
	i2c_start(i2c_port,i2c_sda_pin,i2c_scl_pin);
	ack=i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,address);
	ack=ack|(i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,reg)<<1);
	ack=ack|(i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,data)<<2);
	i2c_stop(i2c_port,i2c_sda_pin,i2c_scl_pin);
	return ack;
}
int i2c_master_read_1_byte(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin,unsigned char address,unsigned char reg)
{
	int data=0;
	i2c_start(i2c_port,i2c_sda_pin,i2c_scl_pin);
	i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,address);
	i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,reg);
	i2c_start(i2c_port,i2c_sda_pin,i2c_scl_pin);
	i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,address|1);
	data=i2c_read(i2c_port,i2c_sda_pin,i2c_scl_pin,0);
	i2c_stop(i2c_port,i2c_sda_pin,i2c_scl_pin);
	return data;
}
int i2c_master_read_2_byte(PORT_t i2c_port,unsigned char i2c_sda_pin,unsigned char i2c_scl_pin,unsigned char address,unsigned char reg)
{
	
	int data=0;
	i2c_start(i2c_port,i2c_sda_pin,i2c_scl_pin);
	i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,address);
	i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,reg);
	i2c_start(i2c_port,i2c_sda_pin,i2c_scl_pin);
	i2c_write(i2c_port,i2c_sda_pin,i2c_scl_pin,address|1);
	data=i2c_read(i2c_port,i2c_sda_pin,i2c_scl_pin,1);
	data*=256;
	data+=i2c_read(i2c_port,i2c_sda_pin,i2c_scl_pin,0);
	i2c_stop(i2c_port,i2c_sda_pin,i2c_scl_pin);
	return data;
}











