/*
 * rtc.c
 *
 * Created: 12.03.2020 11:55:49
 *  Author: V.Nezlo
 */ 

#include "rtc.h"

#define RTCADRR 0b11010001
#define RTCADRW 0b11010000

struct Alarm {uint8_t hour, min, sec; char state; } alarm1;
uint8_t sec, min, hour, day, date, month, year;

uint8_t RTC_ConvertFromBinDec(uint8_t c)
{
	uint8_t ch = ((c/10)<<4)|(c%10);
	return ch;
}

uint8_t RTC_ConvertFromDec(uint8_t c)
{
	uint8_t ch = ((c>>4)*10+(0b00001111&c));
	return ch;
}

void RTC_Set(void)
{

	//Тут можно было бы написать функцию с аргументами, но зачем?
	//И мне лень
	
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);//slave
	I2C_SendByte(0);//Регистр 00
	I2C_SendByte(RTC_ConvertFromBinDec(00));//секунды 00h
	I2C_SendByte(RTC_ConvertFromBinDec(00));//минуты 01h
	I2C_SendByte(RTC_ConvertFromBinDec(12));//часы   02h
	I2C_SendByte(RTC_ConvertFromBinDec(5));//день недели     03h
	I2C_SendByte(RTC_ConvertFromBinDec(13));//дата   04h
	I2C_SendByte(RTC_ConvertFromBinDec(3));//месяц   05h
	I2C_SendByte(RTC_ConvertFromBinDec(20));//год   06h
	I2C_SendByte(RTC_ConvertFromBinDec(0));//A1M1 секунды   07h
	I2C_SendByte(RTC_ConvertFromBinDec(0));//A1M2 минуты   08h
	I2C_SendByte(RTC_ConvertFromBinDec(15));//A1M3 часы   09h
	I2C_SendByte(0b10000000);				//A1M4 день+дата	 0Ah
	I2C_SendByte(RTC_ConvertFromBinDec(0));//A2 минуты   0Bh
	I2C_SendByte(RTC_ConvertFromBinDec(0));//A2 часы   0Ch
	I2C_SendByte(RTC_ConvertFromBinDec(0));//A2 день+дата   0Dh
	I2C_SendByte(0b00011101);//Контрольный  0Eh Прерывания будильника включены и включен будильник А1
	I2C_SendByte(0b00001000);//Контрольный/статусный  0Fh Записывать 0 в последний бит для сброса будильника
	I2C_StopCondition();
}

void RTC_Read(void)
{
I2C_SendByteByADDR(0,RTCADRW);
_delay_ms(200);
I2C_StartCondition();
I2C_SendByte(RTCADRR);
sec = I2C_ReadByte();
min = I2C_ReadByte();
hour = I2C_ReadByte();
day = I2C_ReadByte();
date = I2C_ReadByte();
month= I2C_ReadByte();
year = I2C_ReadByte();
alarm1.sec = I2C_ReadByte();
alarm1.min = I2C_ReadByte();
alarm1.hour = I2C_ReadLastByte();
I2C_StopCondition();

sec=RTC_ConvertFromDec(sec);
min=RTC_ConvertFromDec(min);
hour=RTC_ConvertFromDec(hour);
day=RTC_ConvertFromDec(day);
date=RTC_ConvertFromDec(date);
month=RTC_ConvertFromDec(month);
year=RTC_ConvertFromDec(year);
alarm1.sec = RTC_ConvertFromDec(alarm1.sec);
alarm1.min = RTC_ConvertFromDec(alarm1.min);
alarm1.hour = RTC_ConvertFromDec(alarm1.hour);
}
