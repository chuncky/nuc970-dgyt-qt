#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <mach/gpio.h>

#include <asm/uaccess.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/device.h>

#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/gpio.h>

#include <mach/map.h>

#include <mach/regs-clock.h>
#include <mach/regs-gcr.h>
#include <mach/regs-gpio.h>

//#include "ADN460x.h"
#include "video_switch.h"


#define SW_PIN_H(i)  	gpio_set_value(i,1)
#define SW_PIN_L(i) 	gpio_set_value(i,0)
#define  DATA_DIR_OUT(i) gpio_direction_output(i,1)
#define  DATA_DIR_IN(i)  gpio_direction_input(i)

#define DATA_IN(i)	gpio_get_value(i)

#define ADN460x_CHIP_1_I2C_ADDR	  0x90//==========================U42
#define ADN460x_CHIP_2_I2C_ADDR   0x96//===========================U41

#define	DEVICEID_DIR_IN()  writel(readl(REG_GPIOB_DIR)&(~0x3C000),REG_GPIOB_DIR)


unsigned char SW16x16_CHIP_I2C_ADDR[4]={		
		0x90,//==========================U46
   		0x92,//==========================U47
		0x94,//==========================U48
   		0x96//==========================U49
	};


#define delay_us udelay
#define delay_ms mdelay

typedef unsigned char U8;
typedef unsigned short U16;

unsigned char sPoweron;

unsigned char XPT_MP0[8]={0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97};

unsigned char xpt_status[8]={0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7};

unsigned char SW8X8_XPT_MP0[8]={0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97};

const unsigned char SW8X8_Data_IN[8][4]={//d0 d1 d2 ck	
				{1, 	9, 	0, 	8},//1
				{3,	10,	2,	11},//2
				{5,	12,	4,	13},//3
				{7,	14,	6,	15},//4
				{14,	7,	15,	6},//5
				{12,	5,	13,	4},//6
				{10,	3, 	11, 2},//7
				{8, 	1, 	9, 	0},//8
							};
									
const unsigned char SW8X8_Data_OUT[8][4]={//d0 d1 d2 ck	
							  { 1,	15,	0,	14},//1
							  {3,	13,	2,	12},//2
							  { 5,	11,	4,	10},//3
							  { 7,	9, 	6,	8},//4
							  { 9, 7, 	8, 	6},//5
							  { 11,5,	10,	4},//6
							  { 13,3,	12,	2},//7
							  { 15,1, 	14, 0},//8
							   };





const unsigned char SW16X16_Data_IN[16][4]={//d0 d1 d2 ck	
				{15, 	7, 	8, 	8},//1
				{14,	6,	9,	9},//2
				{13,	5,	10,	10},//3
				{12,	4,	11,	11},//4
				{11,	3,	12,	12},//5
				{10,	2,	13,	13},//6
				{9,	1, 	14, 14},//7
				{8, 	0, 	15, 15},//8
				{7,  15,  0,  0},//9
				{6,  14,  1,  1},//10
				{5,  13,  2,  2},//11
				{4,  12,  3,  3},//12
				{3,  11,  4,  4},//13
				{2,  10,  5,  5},//14
				{1,   9,  6,  6},//15
				{0,   8,  7,  7},//16
				};
									
const unsigned char SW16X16_Data_OUT[16][4]={//d0 d1 d2 ck	
			{7,	7,	0,	0},//1
			{6,	6,	1,	1},//2
			{5,	5,	2,	2},//3
			{4,	4, 	3,	3},//4
			{3, 3, 	4, 	4},//5
			{2, 2,	5,	5},//6
			{1, 1,	6,	6},//7
			{0, 0, 	7, 7},//8
			{15,	15,	8,	8},//9
			{14,	14,	9,	9},//10
			{13,	13,	10,	10},//11
			{12,	12, 11,	11},//12
			{11,  11, 12, 12},//13
			{10,  10,	13,	13},//14
			{9,    9,	14,	14},//15
			{8,    8, 15, 15},//16
			};

// Êý¾ÝÇø
const unsigned char SW20X20_Data_IN[20][4]={//d0 d1 d2 ck	
				{ 1,  0,  1,  0},//1
				{ 3,  2,  3,  2},//2
				{ 5,  4,  5,  4},//3
				{ 7,  6,  7,  6},//4
				{ 9,  8,  9,  8},//5
				{11, 10, 11, 10},//6
				{13, 12, 13, 12},//7
				{15, 14, 15, 14},//8
				{17, 16, 17, 16},//9
				{19, 18, 19, 18},//10
				{21, 20, 21, 20},//11
				{23, 22, 23, 22},//12
				{25, 24, 25, 24},//13
				{27, 26, 27, 26},//14
				{29, 28, 29, 28},//15
				{31, 30, 31, 30},//16
				{33, 32, 33, 32},//17
				{35, 34, 35, 34},//18
				{37, 36, 37, 36},//19
				{39, 38, 39, 38},//20
				};
									
const unsigned char SW20X20_Data_OUT[20][4]={//d0 d1 d2 ck	
				{ 1,  0,  1,  0},//1
				{ 3,  2,  3,  2},//2
				{ 5,  4,  5,  4},//3
				{ 7,  6,  7,  6},//4
				{ 9,  8,  9,  8},//5
				{11, 10, 11, 10},//6
				{13, 12, 13, 12},//7
				{15, 14, 15, 14},//8
				{17, 16, 17, 16},//9
				{19, 18, 19, 18},//10
				{21, 20, 21, 20},//11
				{23, 22, 23, 22},//12
				{25, 24, 25, 24},//13
				{27, 26, 27, 26},//14
				{29, 28, 29, 28},//15
				{31, 30, 31, 30},//16
				{33, 32, 33, 32},//17
				{35, 34, 35, 34},//18
				{37, 36, 37, 36},//19
				{38, 39, 38, 39},//20
				};
//=======×î¸ßÎ»Îª1,´Î¸ßÎ»Îª1,ºó6Î»´ú±íÍ¨µÀnum£¨×î¸ßÎ»Ñ¡ÔñDDC_FPGA,´Î¸ßÎ»Ñ¡ÔñOUTSDA£© =====//	


unsigned char SW20X20_XPT_MP0[40]={0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
			0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
			0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
			0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B};


static int matrix_type;

U16 temp;
static int video_switch_Major = 0;           /* Ö÷Éè±žºÅ */

Switch_Info *psw_info;

#define ADN4605_CHIP_1_I2C_ADDR		0x90//==========================U40
#define ADN4605_CHIP_2_I2C_ADDR		0x92//==========================U41


Switch_Info sw8x8_info={
	.rs=NUC970_PA13,
	.scl=NUC970_PG8,
	.sda=NUC970_PG7,
	.update=NUC970_PG6,
	.output_maxnum=8,
	.input_maxnum=8,

	//.XPT_MP0=&SW8X8_XPT_MP0,
	//.Data_IN=&SW8X8_Data_IN,
	//.Data_OUT=&SW8X8_Data_OUT,
};


Switch_Info sw20x20_info={
	.rs=NUC970_PA13,
	.scl=NUC970_PG8,
	.sda=NUC970_PG7,
	.update=NUC970_PG6,
	.output_maxnum=20,
	.input_maxnum=20,
	//.XPT_MP0=&SW20X20_XPT_MP0,
	//.Data_IN=&SW20X20_Data_IN,
	//.Data_OUT=&SW20X20_Data_OUT,
};

Switch_Info sw16x16_info={
	.rs=NUC970_PA11,
	.scl=NUC970_PG8,
	.sda=NUC970_PG7,
	.update=NUC970_PG6,
	.output_maxnum=16,
	.input_maxnum=16,
	//.XPT_MP0=&SW8X8_XPT_MP0,
	//.Data_IN=&SW16X16_Data_IN,
	//.Data_OUT=&SW16X16_Data_OUT,
};

void ADN460x_I2CStart(void)
{
	 //ADN460x_DIR_OUT();
	DATA_DIR_OUT(psw_info->sda);
	
 	 //ADN460x_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //ADN460x_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
   delay_us(10);
	 //ADN460x_SDA_L;//DATA
	SW_PIN_L(psw_info->sda);
	 delay_us(10);
	 //SCL_L;//CLK
}
//////////////
void ADN460x_I2CStop(void)
{
 	 //DIR_OUT;//
	 //ADN460x_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //ADN460x_SDA_L;//DATA
	SW_PIN_L(psw_info->sda);
	 delay_us(10);
	 //ADN460x_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	 delay_us(10);
}

////////////send ACK
void ADN460x_SendACK(void)
{
 	 //ADN460x_SDA_L;//DATA
	SW_PIN_L(psw_info->sda);
	 delay_us(10);
	 //ADN460x_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //ADN460x_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	 delay_us(10);
}

/////////////send NotACK
void ADN460x_SendNotACK(void)
{
 	 //ADN460x_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	 delay_us(10);
	 //ADN460x_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //ADN460x_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	 delay_us(10);
}
//////////////wait ack
unsigned char ADN460x_WaitAck(void)
{
 	  unsigned char errtime=255;
	  //ADN460x_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	  delay_us(10);

	  //ADN460x_DIR_IN();
	DATA_DIR_IN(psw_info->sda);
	  delay_us(10);

		//while(ADN460x_DATA)//
		while(DATA_IN(psw_info->sda))//
		{
				errtime--;
				if(!errtime)
			{
					ADN460x_I2CStop();
				        printk("no ack\r\n");
					return 0;
			}
		}
//	 if(debug)
//		 uart4_putchar(0x12);
 	 //ADN460x_DIR_OUT();
	DATA_DIR_OUT(psw_info->sda);
	 //ADN460x_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //ADN460x_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	 delay_us(10);
	 return 1;
}

/////I2c  send a byte 
void ADN460x_I2CSendByte(unsigned char x)
{
	unsigned char i=8,mid;

	while(i--)
  {
			//ADN460x_SCL_L;//CLK
		SW_PIN_L(psw_info->scl);
			delay_us(10);
			mid=x&0x80;
			if(mid==0)
				//ADN460x_SDA_L;//DATA
				SW_PIN_L(psw_info->sda);
			else
				//ADN460x_SDA_H;//DATA
				SW_PIN_H(psw_info->sda);
			x<<=1;
			delay_us(10);
			//ADN460x_SCL_H;//CLK
			SW_PIN_H(psw_info->scl);
			delay_us(10);
  }
	//ADN460x_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	delay_us(10);
}
///////// receive a byte
unsigned char ADN460x_I2CReceiveByte(void)/////////////////
{
	unsigned char i=8,data=0,mid;
	//ADN460x_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	//ADN460x_DIR_IN();
	DATA_DIR_IN(psw_info->sda);
	while(i--)
	{
			data<<=1;
			//ADN460x_SCL_L;//CLK
			SW_PIN_L(psw_info->scl);
			delay_us(10);
			//mid=ADN460x_DATA;
			mid=DATA_IN(psw_info->sda);
			if(mid==0)
					data|=0;
			else
					data|=1;
			//ADN460x_SCL_H;//CLK
			SW_PIN_H(psw_info->scl);
			delay_us(10);	
			//ADN460x_SCL_L;//CLK
			SW_PIN_L(psw_info->scl);	
	}
   //ADN460x_DIR_OUT();
	DATA_DIR_OUT(psw_info->sda);
   return data;
}








void ADN460x_I2C_Write(unsigned char i2c_addr,unsigned char Wdata,unsigned int RomAddress)
{
  ADN460x_I2CStart();
	ADN460x_I2CSendByte(i2c_addr);//Device Add+W
	ADN460x_WaitAck();
	ADN460x_I2CSendByte(RomAddress);
	ADN460x_WaitAck();
	ADN460x_I2CSendByte(Wdata);
	ADN460x_SendACK();
	ADN460x_I2CStop();
	delay_ms(10);
}

unsigned char ADN460x_I2C_Read(unsigned char i2c_addr,unsigned char RomAddress)
{
  unsigned char Rx_Data;
  ADN460x_I2CStart();
	ADN460x_I2CSendByte(i2c_addr);//Device Add+W
	ADN460x_WaitAck();
	ADN460x_I2CSendByte(RomAddress);
	ADN460x_WaitAck();
	
	ADN460x_I2CStart();
	ADN460x_I2CSendByte(i2c_addr+1);//Device Add+R
	ADN460x_WaitAck();
	Rx_Data=ADN460x_I2CReceiveByte();
	ADN460x_SendNotACK();
	ADN460x_I2CStop();
	return Rx_Data;
}


void ADN4605_I2CStart(void)
{
	//SDA_DIR_OUT();
	DATA_DIR_OUT(psw_info->sda);
	//ADN4605_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	delay_us(10);
	//SW_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	delay_us(10);
	//SW_SDA_L;//DATA
	SW_PIN_L(psw_info->sda);
	delay_us(10);
	 //SCL_L;//CLK
}





//////////////
void ADN4605_I2CStop(void)
{
	//DIR_OUT;//??????????
	//SW_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	delay_us(10);
	//SW_SDA_L;//DATA
	SW_PIN_L(psw_info->sda);
	delay_us(10);
	//SW_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	delay_us(10);
}




//////////////?????,?????(????)
void ADN4605_SendACK(void)
{
 	 //SW_SDA_L;//DATA
	SW_PIN_L(psw_info->sda);
	 delay_us(10);
	 //SW_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //SW_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	 delay_us(10);
}



//////////////?????,?????(?????)
void ADN4605_SendNotACK(void)
{
 	 //SW_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	 delay_us(10);
	 //SW_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //SW_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	 delay_us(10);
}



//////////////??????)
unsigned char ADN4605_WaitAck(void)
{
 	  unsigned char errtime=255;
	  //SW_SDA_H;//DATA
	SW_PIN_H(psw_info->sda);
	  delay_us(10);

	  //SDA_DIR_IN();
	DATA_DIR_IN(psw_info->sda);
	  delay_us(10);

		while(DATA_IN(psw_info->sda))//
		{
				errtime--;
				if(!errtime)
			{
					ADN4605_I2CStop();
// 					if(debug)
// 						uart4_putchar(0x11);
				printk("no ack\r\n");
					return 0;
			}
		}
	 //if(debug)
		// printk(0x12);
 	 //SDA_DIR_OUT();
	DATA_DIR_OUT(psw_info->sda);
	 //SW_SCL_H;//CLK
	SW_PIN_H(psw_info->scl);
	 delay_us(10);
	 //SW_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	 delay_us(10);
	 return 1;
}



//////////////??????
void ADN4605_I2CSendByte(unsigned char x)
{
	unsigned char i=8,mid;

	while(i--)
  {
			//SW_SCL_L;//CLK
			SW_PIN_L(psw_info->scl);
			delay_us(10);
			mid=x&0x80;
			if(mid==0)
				//SW_SDA_L;//DATA
				SW_PIN_L(psw_info->sda);
			else
				//SW_SDA_H;//DATA
				SW_PIN_H(psw_info->sda);
			x<<=1;
			delay_us(10);
			//SW_SCL_H;//CLK
			SW_PIN_H(psw_info->scl);
			delay_us(10);
  }
	//SW_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	delay_us(10);
}






//////////////??????
unsigned char ADN4605_I2CReceiveByte(void)/////////////////
{
	unsigned char i=8,data=0,mid;
	//SW_SCL_L;//CLK
	SW_PIN_L(psw_info->scl);
	//SDA_DIR_IN();
	DATA_DIR_IN(psw_info->sda);
	while(i--)
  {
			data<<=1;
			//SW_SCL_L;//CLK
			SW_PIN_L(psw_info->scl);
			delay_us(10);
			mid=DATA_IN(psw_info->sda);
			if(mid==0)
					data|=0;
			else
					data|=1;
			//SW_SCL_H;//CLK
			SW_PIN_H(psw_info->scl);
			delay_us(10);	
			//SW_SCL_L;//CLK	
			SW_PIN_L(psw_info->scl);
   }
   //SDA_DIR_OUT();
	DATA_DIR_OUT(psw_info->sda);
   return data;
}


void ADN4605_I2C_Write(unsigned char i2c_addr,unsigned char Wdata,unsigned int RomAddress)
{
  ADN4605_I2CStart();
	ADN4605_I2CSendByte(i2c_addr);//Device Add+W
	ADN4605_WaitAck();
	ADN4605_I2CSendByte(RomAddress);
	ADN4605_WaitAck();
	ADN4605_I2CSendByte(Wdata);
	ADN4605_SendACK();
	ADN4605_I2CStop();
//  	delay_ms(5);
}




unsigned char ADN4605_I2C_Read(unsigned char i2c_addr,unsigned char RomAddress)
{
  unsigned char Rx_Data;
  ADN4605_I2CStart();
	ADN4605_I2CSendByte(i2c_addr);//Device Add+W
	ADN4605_WaitAck();
	ADN4605_I2CSendByte(RomAddress);
	ADN4605_WaitAck();
	
	ADN4605_I2CStart();
	ADN4605_I2CSendByte(i2c_addr+1);//Device Add+R
	ADN4605_WaitAck();
	Rx_Data=ADN4605_I2CReceiveByte();
	ADN4605_SendNotACK();
	ADN4605_I2CStop();
	return Rx_Data;
}

void reset_adn4605(void)
{
	printk("%s-01\n",__func__);		
	//SW0_RESET_L;
	SW_PIN_L(psw_info->rs);
 	delay_ms(3000);
	//SW0_RESET_H;
	SW_PIN_H(psw_info->rs);
		
   	ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,0x01,0x00);
		ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,0x01,0x00);
// 		ADN4605_UPDATE_L;
// 		ADN4605_UPDATE_H;	
// 	 	printf("================value=0x%x\r\n",ADN4605_I2C_Read(0x1));	
printk("%s-02\n",__func__);
}
void reset_ADN460x(void)
{
		//ADN460x_RESET_L;
	SW_PIN_L(psw_info->rs);
		delay_ms(200);
	//	ADN460x_RESET_H;
	SW_PIN_H(psw_info->rs);
		ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0x00,0x00);
	  ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0x00,0x00);
}

void SW20x20_Up_One_Data(unsigned char output,unsigned char input)
{
	
	ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,SW20X20_Data_IN[input][0],SW20X20_XPT_MP0[SW20X20_Data_OUT[output][0]]);
	ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,SW20X20_Data_IN[input][1],SW20X20_XPT_MP0[SW20X20_Data_OUT[output][1]]);
	ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,SW20X20_Data_IN[input][2],SW20X20_XPT_MP0[SW20X20_Data_OUT[output][2]]);
	ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,SW20X20_Data_IN[input][3],SW20X20_XPT_MP0[SW20X20_Data_OUT[output][3]]);		
	
	//SW_UPDATE_L;
	SW_PIN_L(psw_info->update);
	//SW_UPDATE_H;
	SW_PIN_H(psw_info->update);
	ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,0x01,0x01);
 	ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,0x01,0x01);
	 
}

void SW20x20_Up_All_Data(unsigned char channels, unsigned char iMtxState[])
{


	unsigned char iNdx_O = 0,iNdx_I = 0,value = iMtxState[0];
	printk("%s:channels=%d:\n",__func__,channels);	
// 	  
		for(iNdx_O=0;iNdx_O<channels;iNdx_O++)
		{

				iNdx_I=iMtxState[iNdx_O];
				ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,SW20X20_Data_IN[iNdx_I][0],SW20X20_XPT_MP0[SW20X20_Data_OUT[iNdx_O][0]]);
				ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,SW20X20_Data_IN[iNdx_I][1],SW20X20_XPT_MP0[SW20X20_Data_OUT[iNdx_O][1]]);
				ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,SW20X20_Data_IN[iNdx_I][2],SW20X20_XPT_MP0[SW20X20_Data_OUT[iNdx_O][2]]);
				ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,SW20X20_Data_IN[iNdx_I][3],SW20X20_XPT_MP0[SW20X20_Data_OUT[iNdx_O][3]]);				
		}		
// 		ADN4605_UPDATE_L;
// // 		delay_ms(100);
// 		ADN4605_UPDATE_H;
   	ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,0x01,0x01);
 	ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,0x01,0x01);
}


void SW20x20_DCS_ALL_Init(void)
{
		int i = 0;
printk("%s-01\n",__func__);
 		reset_adn4605();
//  	  ADN4605_I2C_Write(0xC0,0xBA);
	  ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,0x00,0x02);
		ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,0x00,0x02);
printk("%s-02\n",__func__);
	for(i=0;i<11;i++)
	{
    ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,0xff,0xC0+i);
	}
 	for(i=0;i<10;i++)
	{
    ADN4605_I2C_Write(ADN4605_CHIP_1_I2C_ADDR,0xff,0xb0+i);
	}
	
	for(i=0;i<11;i++)
	{
    ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,0xff,0xC0+i);
	}
 	for(i=0;i<10;i++)
	{
    ADN4605_I2C_Write(ADN4605_CHIP_2_I2C_ADDR,0xff,0xb0+i);
	}
printk("%s-03\n",__func__);
}

void SW8x8_DCS_ALL_Init(void)
{
		int i = 0;
		reset_ADN460x();
		ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0x77,0x18);
	  ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0x77,0x18);
		ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0x00,0x81);
		ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0x00,0x81);
	
		ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0xff,0x6E);
		ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0xff,0x6F);		
		ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0xff,0x6E);
		ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0xff,0x6F);		
	
	  for(i=0x30;i<0x50;i++)
		{
			ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,(i &0x1 ) ? 0x0F : 0xFF,i);
			ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,(i &0x1 ) ? 0x0F : 0xFF,i);
		}
		
	
	
}

void SW8x8_Up_One_Data(unsigned char output,unsigned char input)
{
	printk("%s:input %d to output %d\n",__func__,input,output);
	
	 // ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0x00,0x81);	
	
		ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,(SW8X8_Data_IN[input][0] << 4) | SW8X8_Data_IN[input][2],SW8X8_XPT_MP0[SW8X8_Data_OUT[output][0] >> 1]);

	
	 // ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0x00,0x81);
		ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,(SW8X8_Data_IN[input][1] << 4) | SW8X8_Data_IN[input][3],SW8X8_XPT_MP0[SW8X8_Data_OUT[output][1] >> 1]);
// 	  ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0x01,0x80);
// 		ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0x01,0x80);
		//ADN460x_UPDATE_L;
	SW_PIN_L(psw_info->update);
		//ADN460x_UPDATE_H;
	SW_PIN_H(psw_info->update);
}


void SW8x8_Up_All_Data(unsigned char channels,unsigned char iMtxState[])
{
    unsigned char iNdx_O = 0,iNdx_I = 0;


printk("%s:channels=%d:\n",__func__,channels);	
		for(iNdx_O=0;iNdx_O<channels;iNdx_O++)
		{
printk("%4d;",iMtxState[iNdx_O]);	
				iNdx_I=iMtxState[iNdx_O];
				ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,(SW8X8_Data_IN[iNdx_I][0] << 4) | SW8X8_Data_IN[iNdx_I][2],SW8X8_XPT_MP0[SW8X8_Data_OUT[iNdx_O][0] >> 1]); 
				ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,(SW8X8_Data_IN[iNdx_I][1] << 4) | SW8X8_Data_IN[iNdx_I][3],SW8X8_XPT_MP0[SW8X8_Data_OUT[iNdx_O][1] >> 1]);			
		}
printk("\n");		 
// 	  ADN460x_I2C_Write(ADN460x_CHIP_1_I2C_ADDR,0x01,0x80);
// 		ADN460x_I2C_Write(ADN460x_CHIP_2_I2C_ADDR,0x01,0x80);
		//ADN460x_UPDATE_L;
	SW_PIN_L(psw_info->update);
		//ADN460x_UPDATE_H;
	SW_PIN_H(psw_info->update);
}



void reset_sw16x16(void)
{
		//ADN460x_RESET_L;
	SW_PIN_L(psw_info->rs);
		delay_ms(1000);
		//ADN460x_RESET_H;
	SW_PIN_H(psw_info->rs);
	
// 		ADN460x_RESET_L;
// 		delay_ms(1000);
// 		ADN460x_RESET_H;	
// 	
// 			ADN460x_RESET_L;
// 		delay_ms(1000);
// 		ADN460x_RESET_H;
// 	
// 		ADN460x_RESET_L;
// 		delay_ms(1000);
// 		ADN460x_RESET_H;	
	
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0x00,0x00);
		
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0x00,0x00);
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0x00,0x00);
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0x00,0x00);
}


void SW16x16_DCS_ALL_Init(void)
{
		int i = 0;
// 		for(i=0;i<20;i++)
		reset_sw16x16();
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0x77,0x18);
	  	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0x77,0x18);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0x77,0x18);
	  	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0x77,0x18);
	
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0x00,0x81);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0x00,0x81);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0x00,0x81);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0x00,0x81);
	
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0xff,0x10);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0xff,0x10);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0xff,0x10);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0xff,0x10);
	
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0xff,0x11);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0xff,0x11);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0xff,0x11);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0xff,0x11);
	
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0xff,0x6E);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],0xff,0x6F);		
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0xff,0x6E);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],0xff,0x6F);		
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0xff,0x6E);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],0xff,0x6F);		
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0xff,0x6E);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],0xff,0x6F);	
	
	  for(i=0x30;i<0x50;i++)
		{
			ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],(i &0x1 ) ? 0x0F : 0xFF,i);
			ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],(i &0x1 ) ? 0x0F : 0xFF,i);
			ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],(i &0x1 ) ? 0x0F : 0xFF,i);
			ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],(i &0x1 ) ? 0x0F : 0xFF,i);
			
		}
		
	
	
}

void SW16x16_Up_One_Data(unsigned char output,unsigned char input)
{


	  unsigned char data[4]={0};
	//unsigned char i = 0;
printk("%s:input %d to output %d\n",__func__,input,output);

	data[0] = ADN460x_I2C_Read(SW16x16_CHIP_I2C_ADDR[0],xpt_status[SW16X16_Data_OUT[output][0]>>1]);
	data[1] = ADN460x_I2C_Read(SW16x16_CHIP_I2C_ADDR[1],xpt_status[SW16X16_Data_OUT[output][1]>>1]);
	data[2] = ADN460x_I2C_Read(SW16x16_CHIP_I2C_ADDR[2],xpt_status[SW16X16_Data_OUT[output][2]>>1]);
	data[3] = ADN460x_I2C_Read(SW16x16_CHIP_I2C_ADDR[3],xpt_status[SW16X16_Data_OUT[output][3]>>1]);

	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],(SW16X16_Data_IN[input][0] << ((SW16X16_Data_OUT[output][0] & 0x1) ? 0x4 :0x0)) | (data[0] & ((SW16X16_Data_OUT[output][0] & 0x1) ? 0x0f :0xf0)),SW8X8_XPT_MP0[SW16X16_Data_OUT[output][0] >> 1]); 
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],(SW16X16_Data_IN[input][1] << ((SW16X16_Data_OUT[output][1] & 0x1) ? 0x4 :0x0)) | (data[1] & ((SW16X16_Data_OUT[output][1] & 0x1) ? 0x0f :0xf0)),SW8X8_XPT_MP0[SW16X16_Data_OUT[output][1] >> 1]);			
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],(SW16X16_Data_IN[input][2] << ((SW16X16_Data_OUT[output][2] & 0x1) ? 0x4 :0x0)) | (data[2] & ((SW16X16_Data_OUT[output][2] & 0x1) ? 0x0f :0xf0)),SW8X8_XPT_MP0[SW16X16_Data_OUT[output][2] >> 1]); 
	ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],(SW16X16_Data_IN[input][3] << ((SW16X16_Data_OUT[output][3] & 0x1) ? 0x4 :0x0)) | (data[3] & ((SW16X16_Data_OUT[output][3] & 0x1) ? 0x0f :0xf0)),SW8X8_XPT_MP0[SW16X16_Data_OUT[output][3] >> 1]);
		
		


		//ADN460x_UPDATE_L;
	SW_PIN_L(psw_info->update);
		//ADN460x_UPDATE_H;
	SW_PIN_H(psw_info->update);


}
void SW16x16_Up_All_Data_poweron(unsigned char channels,unsigned char iMtxState[])
//void Up_Config_All_Data_poweron(void)
{
	unsigned char iNdx_O = 0,iNdx_I = 0,iNdx_I2 = 0;	
	unsigned char i = 0;

	unsigned char dataout,temp1,temp2,datain;

printk("%s-01,channels=%d\n",__func__,channels);
		for(iNdx_O=0;iNdx_O<channels;iNdx_O+=2)
		{

					iNdx_I = iMtxState[iNdx_O];
			    iNdx_I2 = iMtxState[iNdx_O+1];
// uart5_puts("ssssssssssssssssssssssss");
// 			
// uart5_putchar((Data_IN[iNdx_I][0] << ((Data_OUT[iNdx_O][0] & 0x1) ? 0x4 :0x0)) | (Data_IN[iNdx_I2][0] << ((Data_OUT[iNdx_O][0] & 0x1) ? 0x0 :0x4)));	
		for(i=0;i<4;i++){
			dataout=SW16X16_Data_OUT[iNdx_O][i];
			temp1=((dataout & 0x1) ? 0x4 :0x0);
			temp2=((dataout & 0x1) ? 0x0 :0x4);
			datain=(SW16X16_Data_IN[iNdx_I][i]<<temp1)|(SW16X16_Data_IN[iNdx_I2][i]<<temp2);
			dataout=SW8X8_XPT_MP0[SW16X16_Data_OUT[iNdx_O][i] >> 1];
			ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[i],datain,dataout);

		}
		
#if 0
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],
			(SW16X16_Data_IN[iNdx_I][0] << ((SW16X16_Data_OUT[iNdx_O][0] & 0x1) ? 0x4 :0x0)) | (SW16X16_Data_IN[iNdx_I2][0] << ((SW16X16_Data_OUT[iNdx_O][0] & 0x1) ? 0x0 :0x4)) ,
			SW8X8_XPT_MP0[SW16X16_Data_OUT[iNdx_O][0] >> 1]);


				
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],
			(SW16X16_Data_IN[iNdx_I][1] << ((SW16X16_Data_OUT[iNdx_O][1] & 0x1) ? 0x4 :0x0)) | (SW16X16_Data_IN[iNdx_I2][1] << ((SW16X16_Data_OUT[iNdx_O][1] & 0x1) ? 0x0 :0x4)) ,
			SW8X8_XPT_MP0[SW16X16_Data_OUT[iNdx_O][1] >> 1]);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],
			(SW16X16_Data_IN[iNdx_I][2] << ((SW16X16_Data_OUT[iNdx_O][2] & 0x1) ? 0x4 :0x0)) | (SW16X16_Data_IN[iNdx_I2][2] << ((SW16X16_Data_OUT[iNdx_O][2] & 0x1) ? 0x0 :0x4)) ,
			SW8X8_XPT_MP0[SW16X16_Data_OUT[iNdx_O][2] >> 1]);
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],
			(SW16X16_Data_IN[iNdx_I][3] << ((SW16X16_Data_OUT[iNdx_O][3] & 0x1) ? 0x4 :0x0)) | (SW16X16_Data_IN[iNdx_I2][3] << ((SW16X16_Data_OUT[iNdx_O][3] & 0x1) ? 0x0 :0x4)) ,
			SW8X8_XPT_MP0[SW16X16_Data_OUT[iNdx_O][3] >> 1]);
#endif		
// uart5_puts("ssssssssssssssssssssssss");			
		}

		//ADN460x_UPDATE_L;
	SW_PIN_L(psw_info->update);
		//ADN460x_UPDATE_H;
	SW_PIN_H(psw_info->update);

}

void SW16x16_Up_All_Data(unsigned char channels,unsigned char iMtxState[])
{
	unsigned char iNdx_O = 0,iNdx_I = 0;	
	unsigned char i = 0;

	if(sPoweron){
		SW16x16_Up_All_Data_poweron(channels,iMtxState);
		sPoweron=0;
		return;
	}


	for(iNdx_O=0;iNdx_O<channels;iNdx_O+=2)
	{
		if(iMtxState[iNdx_O]!=iMtxState[iNdx_O+1])
		{
			//uart5_puts("ssssssssssssssssssssssss");
			i = 1;
		}
	}
	if(i)
	{
		SW16x16_Up_All_Data_poweron(channels,iMtxState);
	}
	else
	{
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[0],SW16X16_Data_IN[iMtxState[0]][0],0x82); 
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[1],SW16X16_Data_IN[iMtxState[0]][1],0x82);			
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[2],SW16X16_Data_IN[iMtxState[0]][2],0x82); 
		ADN460x_I2C_Write(SW16x16_CHIP_I2C_ADDR[3],SW16X16_Data_IN[iMtxState[0]][3],0x82);
	
		//ADN460x_UPDATE_L;
		SW_PIN_L(psw_info->update);
		//ADN460x_UPDATE_H;
		SW_PIN_H(psw_info->update);			
	}

}



int read_devicetype(void)
{
	int id;
	DEVICEID_DIR_IN();

    	id=(readl(REG_GPIOB_DATAIN)>>6)&0x0f;
	printk("read device id =0x%x\n",id);
    	return id;
}



int GPIO_Init(void)
{
	int ret;

	printk("GPIO_Init....................\n");
	
	printk("REG_CLK_PCLKEN0=0x%x\n",readl(REG_CLK_PCLKEN0));
	
	
	writel(readl(REG_MFP_GPA_H)&0xff0f0fff,REG_MFP_GPA_H);//select GPA11 GPA13 gpio function,for reset pin

	writel(readl(REG_MFP_GPG_L)&0x00ffffff,REG_MFP_GPG_L);//select PG6-PG7 gpio function for update sda
	writel(readl(REG_MFP_GPG_H)&0xfffffff0,REG_MFP_GPG_H);//select PG8     gpio function for scl

	//for read device id
	writel(readl(REG_MFP_GPB_L)&0x00ffffff,REG_MFP_GPB_L);//select PB6-PB7 gpio function 
	writel(readl(REG_MFP_GPB_H)&0xfffffff0,REG_MFP_GPB_H);//select PB8-PB9    gpio function 

	matrix_type=read_devicetype();
	if(matrix_type==SV_8X8){
		printk("init SV_8x8\n");
		psw_info=&sw8x8_info;
		psw_info->SW_Reset = reset_ADN460x;
		psw_info->SW_I2C_Write = ADN460x_I2C_Write;
		psw_info->SW_I2C_Read= ADN460x_I2C_Read;
		psw_info->Up_Config_One_Data=SW8x8_Up_One_Data;
		psw_info->Up_Config_All_Data=SW8x8_Up_All_Data;
		psw_info->DCS_ALL_Init=SW8x8_DCS_ALL_Init;
		psw_info->iMtxState=kmalloc(psw_info->output_maxnum,GFP_KERNEL);

	} 
	else if ((matrix_type==SV_20X20)) {
		printk("init SV_20x20\n");
		psw_info=&sw20x20_info;
		psw_info->SW_Reset = reset_adn4605;
		psw_info->SW_I2C_Write = ADN4605_I2C_Write;
		psw_info->SW_I2C_Read= ADN4605_I2C_Read;
		psw_info->Up_Config_One_Data=SW20x20_Up_One_Data;
		psw_info->Up_Config_All_Data=SW20x20_Up_All_Data;
		psw_info->DCS_ALL_Init=SW20x20_DCS_ALL_Init;
		psw_info->iMtxState=kmalloc(psw_info->output_maxnum,GFP_KERNEL);

	}
	else if ((matrix_type==SV_16X16)) {
		printk("init SV_16X16\n");
		psw_info=&sw16x16_info;
		psw_info->SW_Reset = reset_sw16x16;
		psw_info->SW_I2C_Write = ADN4605_I2C_Write;
		psw_info->SW_I2C_Read= ADN4605_I2C_Read;
		psw_info->Up_Config_One_Data=SW16x16_Up_One_Data;
		psw_info->Up_Config_All_Data=SW16x16_Up_All_Data;
		psw_info->DCS_ALL_Init=SW16x16_DCS_ALL_Init;
		psw_info->iMtxState=kmalloc(psw_info->output_maxnum,GFP_KERNEL);

	}
	else {
		printk("error:unknown swtich type\n");
	}
//init reset
	ret = gpio_request(psw_info->rs , "PIN RESET");
	if (ret) {
      		printk("PIN RESET  failed ret=%d\n",ret);
      		return ret;
	}
	gpio_direction_output(psw_info->rs,1);
//init sda

	ret = gpio_request(psw_info->sda, "PIN_SDA ");
	if (ret) {
      		printk("PIN SDA  failed ret=%d\n",ret);
      		return ret;
	}
	gpio_direction_output(psw_info->sda,1);
//init scl
	ret = gpio_request(psw_info->scl , "PIN_SCL ");
	if (ret) {
      		printk("PIN SCL  failed ret=%d\n",ret);
      		return ret;
	}	
	gpio_direction_output(psw_info->scl,1);
//init update
	ret = gpio_request(psw_info->update , "PIN_UPDATE");
	if (ret) {
      		printk("PIN UPDATE  failed ret=%d\n",ret);
      		return ret;
	}
	gpio_direction_output(psw_info->update,1);

	ndelay(1000);

printk("%s-end....................\n",__func__);
	return 0;
}



long video_switch_ioctl(struct file *file, unsigned int cmd, unsigned long arg)

{
	int ret = 0;	

	int __user *devicetype;
        int data;
	unsigned char output,input;
        int  channels=psw_info->output_maxnum;
	unsigned char *matrix;
	unsigned char * iMtxState=psw_info->iMtxState;

	devicetype = (int __user *)arg;
	

	printk("video_switch_ioctl(cmd=0x%x), value=0x%lu\n",
		cmd, arg);
	
	switch(cmd)
	{
		case VIDEO_SWITCH_GETDEVICE:

			return put_user(matrix_type, devicetype) ? -EFAULT : 0;
			break;	

		case VIDEO_SWITCH_SETONECHANNEL:
			data=*devicetype;
			output=(data>>16)&0xff;
			input=data&0xff;
			//printk("%s:data=0x%x\n",__func__,data);
			psw_info->Up_Config_One_Data(output,input);
			return 0;
			break;

		case VIDEO_SWITCH_SETALLCHANNEL:
			printk("%s-02\n",__func__);
			matrix=(unsigned char __user *)arg;
			copy_from_user(iMtxState, matrix,
					channels);
			psw_info->Up_Config_All_Data(channels,iMtxState);
			return 0;
			break;

	
#if 0	
		case VIDEO_ACTIVE_WINDOW_COORDINATES:
			
			break;
		case VIDEO_DISPLAY_REFRESH:
			nuc970fb_lcdrefresh(info);
			break;
		case VIDEO_DISPLAY_ON:
			ret=lcd_command(LCD_DISPLAY_ON,0);
			break;
		case VIDEO_DISPLAY_OFF:
			ret=lcd_command(LCD_DISPLAY_OFF,0);
			break;
		case IOCTLCLEARSCREEN:
			
			break;
		case IOCTL_LCD_CONTRAST:
			data=arg&0x07;
			ret=lcd_command(LCD_CONTRAST_SET,data);
			break;
		case IOCTL_LCD_BACKLIGHT:
			data = arg&0x01;
			ret=lcd_command(LCD_ADDCMD_BL,data);
			break;	
#endif																	
		default:
		    return(-ENOIOCTLCMD);		
			break;	
	}


	return ret;
}












static int video_switch_open(struct inode *inode, struct file *file)
{
    printk("video_switch Driver Open is Called!\n");
	psw_info->DCS_ALL_Init();

    return 0;
}

static int video_switch_release(struct inode *inode, struct file *file)
{
    printk("video_switch Driver Release Called!\n");
    return 0;
}

static int video_switch_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	unsigned short data = 0xffff;
	unsigned char i = 0;

	copy_to_user(buf,&data,2);
	return 0;

}


static ssize_t video_switch_write(struct file *file, const char *buffer, size_t count, loff_t * ppos)
{   
	char val;
	
      //printk("video_switch Driver Write!\n");

	if((count>1)||(count==0))
	{
		printk("parameter count error \n");
		return 0;
	}
		
	copy_from_user(&val, buffer, count);


	 
	return 0;
}




static struct file_operations video_switch_fops =
{
    .owner   =   THIS_MODULE,
    .open     =   video_switch_open, 
    .release =  video_switch_release,
    .read     =   video_switch_read,
    .write    =   video_switch_write,
    .unlocked_ioctl	= video_switch_ioctl,
};


static struct class *video_switch_class;

 
static int __init video_switch_init(void)
{
    printk("video_switch DRIVER MODULE INIT\n");

    GPIO_Init();  //
    //psw_info->DCS_ALL_Init();

    //printk("%s-01\n",__func__);
    //Matrix_init();
    video_switch_Major = register_chrdev(0, "video_switch", &video_switch_fops);
    if (video_switch_Major < 0)
    {
        printk( "video_switch can't register major number\n");
        return video_switch_Major;
    }
    printk("register video_switch Driver OK! Major = %d\n", video_switch_Major);
    video_switch_class = class_create(THIS_MODULE, "video_switch");
    if(IS_ERR(video_switch_class))
    {
        printk("Err: failed in video_switch class. \n");
        return -1;
    }

    device_create(video_switch_class, NULL, MKDEV(video_switch_Major, 0), NULL, "video_switch");
    //printk("video_switch initialized\n");

	//psw_info->DCS_ALL_Init();

	sPoweron=1;

    return 0;

}

 
static void __exit video_switch_exit(void)
{
	unsigned char * iMtxState=psw_info->iMtxState;    

	printk("video_switch DRIVER MODULE EXIT\n");
	unregister_chrdev(video_switch_Major, "video_switch");
	device_destroy(video_switch_class, MKDEV(video_switch_Major, 0));
	class_destroy(video_switch_class);
        kfree(iMtxState);
}

 

module_init(video_switch_init);
module_exit(video_switch_exit);


MODULE_AUTHOR("Tanshi Li");     

MODULE_DESCRIPTION("video switch Driver");  

MODULE_LICENSE("GPL");



