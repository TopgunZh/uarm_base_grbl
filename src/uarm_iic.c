#include "uarm_iic.h"


#define ARML_SDA_SET			(PORTC |= 0x02)
#define ARML_SDA_RESET		(PORTC &= 0xFD)
#define ARML_SCL_SET			(PORTC |= 0x08)	
#define ARML_SCL_RESET		(PORTC &= 0xF7)
#define ARML_SDA_INPUT		(DDRC &= 0xFD)
#define ARML_SDA_OUTPUT		(DDRC |= 0x02)
#define ARML_SCL_INPUT		(DDRC &= 0xF7)
#define ARML_SCL_OUTPUT		(DDRC |= 0x08)
#define ARML_SDA_READ			(PINC & 0x02)
#define ARML_PORT_DDR			(DDRC)
#define ARML_SCK_H      	{ARML_SCL_SET;   }  
#define ARML_SCK_L      	{ARML_SCL_RESET;   }
#define ARML_SDA_H      	{ARML_SDA_OUTPUT;ARML_SDA_SET; }      
#define ARML_SDA_L      	{ARML_SDA_OUTPUT;ARML_SDA_RESET; }  
#define ARML_READ_SDA   	ARML_SDA_READ

#define ARMR_SDA_SET			(PORTH |= 0x04)
#define ARMR_SDA_RESET		(PORTH &= 0xFB)
#define ARMR_SCL_SET			(PORTH |= 0x80)	
#define ARMR_SCL_RESET		(PORTH &= 0x7F)
#define ARMR_SDA_INPUT		(DDRH &= 0xFB)
#define ARMR_SDA_OUTPUT		(DDRH |= 0x04)
#define ARMR_SCL_INPUT		(DDRH &= 0x7F)
#define ARMR_SCL_OUTPUT		(DDRH |= 0x80)
#define ARMR_SDA_READ			(PINH & 0x04)
#define ARMR_PORT_DDR			(DDRH)
#define ARMR_SCK_H      	{ARMR_SCL_SET;   }  
#define ARMR_SCK_L      	{ARMR_SCL_RESET;   }
#define ARMR_SDA_H      	{ARMR_SDA_OUTPUT;ARMR_SDA_SET; }      
#define ARMR_SDA_L      	{ARMR_SDA_OUTPUT;ARMR_SDA_RESET; }  
#define ARMR_READ_SDA   	ARMR_SDA_READ

#define BASE_SDA_SET			(PORTD |= 0x20)
#define BASE_SDA_RESET		(PORTD &= 0xDF)
#define BASE_SCL_SET			(PORTD |= 0x40)	
#define BASE_SCL_RESET		(PORTD &= 0xBF)
#define BASE_SDA_INPUT		(DDRD &= 0xDF)
#define BASE_SDA_OUTPUT		(DDRD |= 0x20)
#define BASE_SCL_INPUT		(DDRD &= 0xBF)
#define BASE_SCL_OUTPUT		(DDRD |= 0x40)
#define BASE_SDA_READ			(PIND & 0x20)
#define BASE_PORT_DDR			(DDRD)
#define BASE_SCK_H      	{BASE_SCL_SET;   }  
#define BASE_SCK_L      	{BASE_SCL_RESET;   }
#define BASE_SDA_H      	{BASE_SDA_OUTPUT;BASE_SDA_SET; }      
#define BASE_SDA_L      	{BASE_SDA_OUTPUT;BASE_SDA_RESET; }    
#define BASE_READ_SDA   	BASE_SDA_READ

#define AS5600_DEFAULT_ADDRESS 0x36


static void arml_iic_start(void)
{
  ARML_SDA_H;
  ARML_SCK_H;            
  delay_us(5);  
  ARML_SDA_L;        
  delay_us(5);  
  ARML_SCK_L;       
  delay_us(2);
}

static void arml_iic_stop(void)
{
  ARML_SCK_L;             
  ARML_SDA_L;             
  delay_us(5);
  ARML_SCK_H;
  delay_us(5);       
  ARML_SDA_H;             
  delay_us(5);       
}

static uint8_t arml_iic_wait_ack(void)
{
  uint8_t ucErrTime=0;
  ARML_SDA_H;delay_us(2); 
  ARML_SCK_H;delay_us(2);  


  ARML_SDA_INPUT;

  while(ARML_READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      arml_iic_stop();
      return 1;
    }
  }
  
  ARML_SCK_L;             
  delay_us(2);  
  return 0;  
}

static void arml_iic_ack(void)
{
  ARML_SCK_L;               
  ARML_SDA_L;            
  delay_us(2);
  ARML_SCK_H;
  delay_us(5);      
  
  ARML_SCK_L;           
  delay_us(2);      
  ARML_SDA_H;  
  delay_us(2);
}

static void arml_iic_nack(void)
{
  ARML_SCK_L;           
  ARML_SDA_H;           
  delay_us(2);
  ARML_SCK_H;
  delay_us(5);      
  
  ARML_SCK_L;            
  delay_us(2);     
  ARML_SDA_H;  
  delay_us(2);
}

static void arml_iic_send_byte(uint8_t data)
{
  uint8_t i; 
  for(i=0;i<8;i++)
  { 
    ARML_SCK_L;           
    if(data&0x80)  
      ARML_SDA_H          
    else
      ARML_SDA_L
    delay_us(5); 
    ARML_SCK_H;      
    delay_us(5); 
    ARML_SCK_L;       
    data<<=1;
    delay_us(5);
  }
  
  delay_us(2);  
  ARML_SDA_H;  
  delay_us(2);
}

static uint8_t arml_iic_read_byte(uint8_t ack)
{
  uint8_t i,receive=0;
  
  delay_us(2);  
  ARML_SDA_H;  
  delay_us(2);
  
  for(i=0;i<8;i++)
  {
    ARML_SCK_L;     
    delay_us(5);
    ARML_SCK_H;      
    receive<<=1;
    delay_us(5);
    ARML_SDA_INPUT;

    if(ARML_READ_SDA)
      receive|=0x01;
    else
      receive&=0xfe;
    delay_us(5);
  }
  if(!ack)
    arml_iic_nack();
  else
    arml_iic_ack();
  
  ARML_SCK_L;             
  delay_us(2);
  
  return receive;
}

static void arml_iic_write_onebyte(uint8_t deviceaddr,uint8_t writeaddr,uint8_t writedata)
{
  arml_iic_start();
  arml_iic_send_byte(deviceaddr&0xfe);
  arml_iic_wait_ack();
  arml_iic_send_byte(writeaddr);
  arml_iic_wait_ack();
  arml_iic_send_byte(writedata);
  arml_iic_wait_ack();
  arml_iic_stop();
  delay_us(10);              
}

static uint8_t arml_iic_read_onebyte(uint8_t deviceaddr,uint8_t readaddr)
{
  uint8_t temp;
  arml_iic_start();
  arml_iic_send_byte(deviceaddr&0xfe);
  arml_iic_wait_ack();
  arml_iic_send_byte(readaddr);
  arml_iic_wait_ack();

  arml_iic_start();
  arml_iic_send_byte(deviceaddr|0x01);
  arml_iic_wait_ack();
  temp=arml_iic_read_byte(0);
  arml_iic_stop();
  return temp;
}

static void armr_iic_start(void)
{
  ARMR_SDA_H;
  ARMR_SCK_H;            
  delay_us(5);  
  ARMR_SDA_L;        
  delay_us(5);  
  ARMR_SCK_L;       
  delay_us(2);
}

static void armr_iic_stop(void)
{
  ARMR_SCK_L;             
  ARMR_SDA_L;             
  delay_us(5);
  ARMR_SCK_H;
  delay_us(5);       
  ARMR_SDA_H;             
  delay_us(5);       
}

static uint8_t armr_iic_wait_ack(void)
{
  uint8_t ucErrTime=0;
  ARMR_SDA_H;delay_us(2); 
  ARMR_SCK_H;delay_us(2);  


  ARMR_SDA_INPUT;

  while(ARMR_READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      armr_iic_stop();
      return 1;
    }
  }
  
  ARMR_SCK_L;             
  delay_us(2);  
  return 0;  
}

static void armr_iic_ack(void)
{
  ARMR_SCK_L;               
  ARMR_SDA_L;            
  delay_us(2);
  ARMR_SCK_H;
  delay_us(5);      
  
  ARMR_SCK_L;           
  delay_us(2);      
  ARMR_SDA_H;  
  delay_us(2);
}

static void armr_iic_nack(void)
{
  ARMR_SCK_L;           
  ARMR_SDA_H;           
  delay_us(2);
  ARMR_SCK_H;
  delay_us(5);      
  
  ARMR_SCK_L;            
  delay_us(2);     
  ARMR_SDA_H;  
  delay_us(2);
}

static void armr_iic_send_byte(uint8_t data)
{
  uint8_t i; 
  for(i=0;i<8;i++)
  { 
    ARMR_SCK_L;           
    if(data&0x80)  
      ARMR_SDA_H          
    else
      ARMR_SDA_L
    delay_us(5); 
    ARMR_SCK_H;      
    delay_us(5); 
    ARMR_SCK_L;       
    data<<=1;
    delay_us(5);
  }
  
  delay_us(2);  
  ARMR_SDA_H;  
  delay_us(2);
}

static uint8_t armr_iic_read_byte(uint8_t ack)
{
  uint8_t i,receive=0;
  
  delay_us(2);  
  ARMR_SDA_H;  
  delay_us(2);
  
  for(i=0;i<8;i++)
  {
    ARMR_SCK_L;     
    delay_us(5);
    ARMR_SCK_H;      
    receive<<=1;
    delay_us(5);
    ARMR_SDA_INPUT;

    if(ARMR_READ_SDA)
      receive|=0x01;
    else
      receive&=0xfe;
    delay_us(5);
  }
  if(!ack)
    armr_iic_nack();
  else
    armr_iic_ack();
  
  ARMR_SCK_L;             
  delay_us(2);
  
  return receive;
}

static void armr_iic_write_onebyte(uint8_t deviceaddr,uint8_t writeaddr,uint8_t writedata)
{
  armr_iic_start();
  armr_iic_send_byte(deviceaddr&0xfe);
  armr_iic_wait_ack();
  armr_iic_send_byte(writeaddr);
  armr_iic_wait_ack();
  armr_iic_send_byte(writedata);
  armr_iic_wait_ack();
  armr_iic_stop();
  delay_us(10);              
}

static uint8_t armr_iic_read_onebyte(uint8_t deviceaddr,uint8_t readaddr)
{
  uint8_t temp;
  armr_iic_start();
  armr_iic_send_byte(deviceaddr&0xfe);
  armr_iic_wait_ack();
  armr_iic_send_byte(readaddr);
  armr_iic_wait_ack();

  armr_iic_start();
  armr_iic_send_byte(deviceaddr|0x01);
  armr_iic_wait_ack();
  temp=armr_iic_read_byte(0);
  armr_iic_stop();
  return temp;
}

static void base_iic_start(void)
{
  BASE_SDA_H;
  BASE_SCK_H;            
  delay_us(5);  
  BASE_SDA_L;        
  delay_us(5);  
  BASE_SCK_L;       
  delay_us(2);
}

static void base_iic_stop(void)
{
  BASE_SCK_L;             
  BASE_SDA_L;             
  delay_us(5);
  BASE_SCK_H;
  delay_us(5);       
  BASE_SDA_H;             
  delay_us(5);       
}

static uint8_t base_iic_wait_ack(void)
{
  uint8_t ucErrTime=0;
  BASE_SDA_H;delay_us(2); 
  BASE_SCK_H;delay_us(2);  


  BASE_SDA_INPUT;

  while(BASE_READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      base_iic_stop();
      return 1;
    }
  }
  
  BASE_SCK_L;             
  delay_us(2);  
  return 0;  
}

static void base_iic_ack(void)
{
  BASE_SCK_L;               
  BASE_SDA_L;            
  delay_us(2);
  BASE_SCK_H;
  delay_us(5);      
  
  BASE_SCK_L;           
  delay_us(2);      
  BASE_SDA_H;  
  delay_us(2);
}

static void base_iic_nack(void)
{
  BASE_SCK_L;           
  BASE_SDA_H;           
  delay_us(2);
  BASE_SCK_H;
  delay_us(5);      
  
  BASE_SCK_L;            
  delay_us(2);     
  BASE_SDA_H;  
  delay_us(2);
}

static void base_iic_send_byte(uint8_t data)
{
  uint8_t i; 
  for(i=0;i<8;i++)
  { 
    BASE_SCK_L;           
    if(data&0x80)  
      BASE_SDA_H          
    else
      BASE_SDA_L
    delay_us(5); 
    BASE_SCK_H;      
    delay_us(5); 
    BASE_SCK_L;       
    data<<=1;
    delay_us(5);
  }
  
  delay_us(2);  
  BASE_SDA_H;  
  delay_us(2);
}

static uint8_t base_iic_read_byte(uint8_t ack)
{
  uint8_t i,receive=0;
  
  delay_us(2);  
  BASE_SDA_H;  
  delay_us(2);
  
  for(i=0;i<8;i++)
  {
    BASE_SCK_L;     
    delay_us(5);
    BASE_SCK_H;      
    receive<<=1;
    delay_us(5);
    BASE_SDA_INPUT;

    if(BASE_READ_SDA)
      receive|=0x01;
    else
      receive&=0xfe;
    delay_us(5);
  }
  if(!ack)
    base_iic_nack();
  else
    base_iic_ack();
  
  BASE_SCK_L;             
  delay_us(2);
  
  return receive;
}

static void base_iic_write_onebyte(uint8_t deviceaddr,uint8_t writeaddr,uint8_t writedata)
{
  base_iic_start();
  base_iic_send_byte(deviceaddr&0xfe);
  base_iic_wait_ack();
  base_iic_send_byte(writeaddr);
  base_iic_wait_ack();
  base_iic_send_byte(writedata);
  base_iic_wait_ack();
  base_iic_stop();
  delay_us(10);              
}

static uint8_t base_iic_read_onebyte(uint8_t deviceaddr,uint8_t readaddr)
{
  uint8_t temp;
  base_iic_start();
  base_iic_send_byte(deviceaddr&0xfe);
  base_iic_wait_ack();
  base_iic_send_byte(readaddr);
  base_iic_wait_ack();

  base_iic_start();
  base_iic_send_byte(deviceaddr|0x01);
  base_iic_wait_ack();
  temp=base_iic_read_byte(0);
  base_iic_stop();
  return temp;
}

void angle_iic_init(void){
	ARML_SDA_OUTPUT;
	ARML_SCL_OUTPUT;	
	ARML_SDA_SET;
	ARML_SCL_SET;

	ARMR_SDA_OUTPUT;
	ARMR_SCL_OUTPUT;	
	ARMR_SDA_SET;
	ARMR_SCL_SET;

	BASE_SDA_OUTPUT;
	BASE_SCL_OUTPUT;	
	BASE_SDA_SET;
	BASE_SCL_SET;
}

void iic_write_byte(uint8_t channel, uint8_t dev_addr, uint8_t write_addr, uint8_t write_data){

	switch(channel){
		case 0:
			arml_iic_write_onebyte(dev_addr, write_addr, write_data);
			break;
		case 1:
			armr_iic_write_onebyte(dev_addr, write_addr, write_data);
			break;
		case 2:
			base_iic_write_onebyte(dev_addr, write_addr, write_data);
			break;
	}
}

uint8_t iic_read_byte(uint8_t channel, uint8_t dev_addr, uint8_t read_addr){
	uint8_t read_byte = 0;
	switch(channel){
		case 0:
			read_byte = arml_iic_read_onebyte(dev_addr, read_addr);
			break;
		case 1:
			read_byte = armr_iic_read_onebyte(dev_addr, read_addr);
			break;
		case 2:
			read_byte = base_iic_read_onebyte(dev_addr, read_addr);
			break;
	}
	return read_byte;
}



