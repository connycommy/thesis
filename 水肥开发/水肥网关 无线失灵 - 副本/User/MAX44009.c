#include "max44009.h"
#include "math.h"

/****************************
启动I2C
数据在时钟高电平的时候从高往低跃变
*****************************/

void MAX44009_Transtart(void)
{
    LIGHT_DTA_W;
    LIGHT_DTA_H;
    LIGHT_SCK_H;
    Delayus(TIMEOUT);
    LIGHT_DTA_L;
    Delayus(TIMEOUT);
    LIGHT_SCK_L;
    Delayus(TIMEOUT);
    LIGHT_DTA_R;//安全使用
}

/********************************

结束I2C

数据在时钟高电平的时候从低往高跃变
********************************/

void MAX44009_Transtop(void)
{
    LIGHT_DTA_W;
    LIGHT_DTA_L;
    LIGHT_SCK_H;
    Delayus(TIMEOUT);
    LIGHT_DTA_H;
    Delayus(TIMEOUT);
    LIGHT_SCK_L;
    Delayus(TIMEOUT);
    LIGHT_DTA_R;//安全使用
}

/******************************
发送字节并且判断是否收到ACK
当收到ACK返回为0，否则返回为1
******************************/
u8 MAX44009_Send(u8 val)
{
    u8 i;
    u8 err=0;

    LIGHT_DTA_W;

    for(i=0; i<8; i++)
    {
        if(val&0x80)
            LIGHT_DTA_H;
        else
            LIGHT_DTA_L;

        Delayus(TIMEOUT);
        LIGHT_SCK_H;
        Delayus(TIMEOUT);
        LIGHT_SCK_L;
        val<<=1;
        Delayus(TIMEOUT);
    }

    LIGHT_DTA_H;
    LIGHT_DTA_R;
    Delayus(TIMEOUT);
    LIGHT_SCK_H;
    Delayus(TIMEOUT);

    if(LIGHT_DTA)
    {
        err=0;
    }
    else
    {
        err=1;
    }

    Delayus(TIMEOUT);
    LIGHT_SCK_L;
    Delayus(TIMEOUT);

    return err;
}


/***************************
读取I2C的字节，并且发送ACK
当参数为1的时候发送一个ACK(低电平)
***************************/

u8 MAX44009_Read(u8 ack)
{
    u8 i;
    u8 val=0;

    LIGHT_DTA_R;

    for(i=0; i<8; i++)
    {
        val<<=1;
        LIGHT_SCK_H;
        Delayus(TIMEOUT);
        if(LIGHT_DTA)
            val|=0x01;
        LIGHT_SCK_L;
        Delayus(TIMEOUT);
    }
    LIGHT_DTA_W;

    if(ack!=0)
        LIGHT_DTA_L;
    else
        LIGHT_DTA_H;

    Delayus(TIMEOUT);
    LIGHT_SCK_H;
    Delayus(TIMEOUT);
    LIGHT_SCK_L;
    LIGHT_DTA_H;
    LIGHT_DTA_R;//安全使用
    return val;
}

void MAX44009_INIT(void)
{
    MAX44009_Transtart();
    MAX44009_Send(MAX44009_WRITE);   //发送地址
    MAX44009_Send(CONFIGURATION);
    MAX44009_Send(0x80);
    MAX44009_Transtop();
}

u16 get_light(void)
{
    u16 value=0x0000;

    MAX44009_Transtart();
    MAX44009_Send(MAX44009_WRITE);   //发送slave写地址
    MAX44009_Send(LUX_HIGH_BYTE);	   //发送光照高字节寄存器地址

    MAX44009_Transtart();				//restart
    MAX44009_Send(MAX44009_READ);	 	//发送slave读地址
    value = MAX44009_Read(0); 		//读光照高字节

    MAX44009_Transtart();			   //restart
    MAX44009_Send(MAX44009_WRITE);   //发送slave写地址
    MAX44009_Send(LUX_LOW_BYTE);	   //发送光照低字节寄存器地址

    MAX44009_Transtart();			   //restart
    MAX44009_Send(MAX44009_READ);	   //发送slave读地址

    value=(value<<8) | MAX44009_Read(0);	 //读光照低字节

    MAX44009_Transtop();			   //停止IIC传输

    return value;
}

u16 Get_Illuminance(void)
{
    u16 result = 0x0000;
    u16 temp = 0x0000;

    u8  exp = 0x00;
    u8  coef = 0x00;

    double res = 0.0;

    temp = get_light();
    exp =  (temp & 0xF000) >> 12;
    coef = ((temp & 0x0F00) >> 4) | (temp & 0x000F);

    if(exp == 0x0F)   //指数超量程情况，返回0xFFFF作为区分
    {
        return 0xFFFF;
    }
    else
    {
        res = pow(2,exp) * 0.045 * coef / 10;  //是实际值的1/10
        result = (u16)res;
        return result;
    }

}

u16 GET_PRESSUE0(void)   //大气压力;通道0-光照度PC0
{
    u16 value;
    u16 data[5]= {0x00};

    MAX44009_Transtart();
    MAX44009_Send(0x50);
    MAX44009_Send(0xAC);
    MAX44009_Transtop();
    Delayms(100);//这个延时很重要，时间要大于37ms

    MAX44009_Transtart();
    MAX44009_Send(0x51);   //发送地址
    data[0]=MAX44009_Read(1);
    data[1]=MAX44009_Read(1);
    data[2]=MAX44009_Read(1);
    data[3]=MAX44009_Read(1);
    data[4]=MAX44009_Read(0);
    MAX44009_Transtop();

    if((data[0]&0x60)==0x40)
    {
        value=(data[1]<<8)|data[2];
    }
    else
    {
        value=0xffff;
    }
    return value;
}
u16 PRESSUE_level2(void)   //液位压力LWP5050GD;通道0-光照度PC0
{
    u16 value;
    u8 data[7]= {0x00};

    MAX44009_Transtart();
    MAX44009_Send(0x00);
//Delayms(5);
    MAX44009_Send(0xAA);
//Delayms(5);
    MAX44009_Send(0x00);
//Delayms(5);
    MAX44009_Send(0x80);
//Delayms(5);
    MAX44009_Transtop();
    Delayms(30);//这个延时很重要，时间要大于5ms

    MAX44009_Transtart(); //读状态byte
    MAX44009_Send(0x01);   //发送地址
//Delayms(5);
    data[6]=MAX44009_Read(1);
    data[0]=MAX44009_Read(1);
    data[1]=MAX44009_Read(1);
    data[2]=MAX44009_Read(1);
    data[3]=MAX44009_Read(1);
    data[4]=MAX44009_Read(1);
    data[5]=MAX44009_Read(0);
    MAX44009_Transtop();
    value=((data[0]<<16)+(data[1]<<8)+data[2])>>8;
    level2_temperature=((data[3]<<16)+(data[4]<<8)+data[5])>>8;
    return value;
}


u16 GET_level0(void)   //液位压力cps120;通道0-光照度PC0
{
    u16 value;
    u16 data[5]= {0x00};

    MAX44009_Transtart();
    MAX44009_Send(0x50);
    MAX44009_Send(0xAC);
    MAX44009_Transtop();
    Delayms(100);//这个延时很重要，时间要大于37ms

    MAX44009_Transtart();
    MAX44009_Send(0x51);   //发送地址
    data[0]=MAX44009_Read(1);
    data[1]=MAX44009_Read(1);
    data[2]=MAX44009_Read(1);
    data[3]=MAX44009_Read(0);
    MAX44009_Transtop();

    if((data[0]&0xC0)==0x00)
    {
        value=(data[0]<<8)|data[1];
    }
    else
    {
        value=0xffff;
    }
    return value;
}


