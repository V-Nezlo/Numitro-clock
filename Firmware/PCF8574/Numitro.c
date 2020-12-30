/*
  I2C-шинные 7 сегментные часы со статической индикацией
  DS3231,DS3231M,DS3232 RTC
  PCF8574T или PCF8574AT (разные адреса, в проекте используется PCF8574T)
 
  Created: 17.06.2020 13:54:05
  Author: V.Nezlo
 */ 

#include "main.h"

#define RTCADRR 0b11010001
#define RTCADRW 0b11010000

#define DIGIADR1 0b01000000
#define DIGIADR2 0b01000010
#define DIGIADR3 0b01000110
#define DIGIADR4 0b01001110

/* 
//             xabcdefg
#define NUM1 0b00000110
#define NUM2 0b01011011
#define NUM3 0b01001111
#define NUM4 0b01100110
#define NUM5 0b01101101
#define NUM6 0b01111101
#define NUM7 0b00000111
#define NUM8 0b01111111
#define NUM9 0b01101111
#define NUM0 0b00111111
#define ZERO 0b00000000
*/
#define ZERO 0b11111111
//1&2 pcf
#define NUM11 0b11001111
#define NUM21 0b10101000
#define NUM31 0b10001001
#define NUM41 0b11000011
#define NUM51 0b10010001
#define NUM61 0b10010000
#define NUM71 0b10001111
#define NUM81 0b10000000
#define NUM91 0b10000001
#define NUM01 0b10000100
//3 pcf       //xacbedgf
#define NUM12 0b11001111
#define NUM22 0b10100001
#define NUM32 0b10001001
#define NUM42 0b11001100
#define NUM52 0b10011000
#define NUM62 0b10010000
#define NUM72 0b10001111
#define NUM82 0b10000000
#define NUM92 0b10001000
#define NUM02 0b10000010
//4 pcf
#define NUM13 0b10011111
#define NUM23 0b10100001
#define NUM33 0b10001001
#define NUM43 0b10011100
#define NUM53 0b11001000
#define NUM63 0b11000000
#define NUM73 0b10001111
#define NUM83 0b10000000
#define NUM93 0b10001000
#define NUM03 0b10000010
//4 pcf с знаком будильника
#define NUM1a 0b00011111
#define NUM2a 0b00100001
#define NUM3a 0b00001001
#define NUM4a 0b00011100
#define NUM5a 0b01001000
#define NUM6a 0b01000000
#define NUM7a 0b00001111
#define NUM8a 0b00000000
#define NUM9a 0b00001000
#define NUM0a 0b00000010

#define CHAR1 0b11000011 // Ч
#define CHAR2 0b11000000 // А
#define CHAR3 0b11110111 // С
#define CHAR4 0b11111111 // Ы

struct Alarm {uint8_t hour, min, sec; char state, flag ;} alarm1;
struct TimeFlags {char incminflag, inchourflag;} timef, alarm1f; 

uint8_t sec, min, hour, day, date, month, year;
uint8_t alarm_const;
char mode, flashflag, alarmflag, zummerflag, alarmresetflag;

ISR (INT1_vect)
{
	alarmflag=1;//Алярма при получении сигнала прерывания с RTC
}

/*
ISR (TIMER2_COMPA_vect) Светодиодик не мигает, отключено
{
	if (flashflag==1)flashflag=0; //blink 0,5s (or not 0,5)
	else flashflag=1;
}
*/

ISR (TIMER0_COMPA_vect)
{
	
		if (alarmflag==1)//Если будильник будит
		{
			if ((!(PIND&0b01000000))|(!(PIND&0b10000000))){ //любое нажатие сбрасывает будильник
				 alarmflag=0; 
				 alarmresetflag=1;
				 _delay_ms(200);
				}
			if (zummerflag==0) zummerflag=1;//зуммер кричит
			else zummerflag=0;
		}
		else zummerflag=0;
		
     	if(!(PIND&0b10000000)) //Проверка кнопок и установка режимов-времен
		 {
			 if (mode ==0  |mode==1 )
			 {
			switch (mode)
				{
				case 0:
				mode = 1;
				break;
				case 1:
				mode = 0;
				break;
				}
			}
		else if (mode==2) timef.inchourflag=1;//ставим флаги для работы с RTC
		else if (mode==3) timef.incminflag=1;
		else if (mode==4) alarm1f.inchourflag=1;
		else if (mode==5) alarm1f.incminflag=1;
		_delay_ms(100);
		}

		else if(!(PIND&0b01000000))
		{
			if (mode==0) mode = 2;//Время, установка часов
			else if (mode==2) mode = 3;//Время, установка минут
			else if (mode==3) mode = 0;//Возврат в нормальный режим
			else if (mode==1) mode = 4;//Будильник1, установка часов
			else if (mode==4) mode = 5;//Будильник1, установка минут
			else if (mode==5) //Будильник1, вкл-выкл
			{
				mode = 1;
				if (alarm1.state == 0) alarm1.flag=1;//Включить будильник
				else alarm1.flag=2;//Выключить будильник
			}
			_delay_ms(100);
		}			
}

void setdigit(char digit)
{
	switch (digit)
	{
		case 1:
		I2C_SendByte(DIGIADR1);
		break;
		case 2:
		I2C_SendByte(DIGIADR2);
		break;
		case 3:
		I2C_SendByte(DIGIADR3);
		break;
		case 4:
		I2C_SendByte(DIGIADR4);
		break;
	}
}

void setnumber1(char number)
{
	switch (number)
	{
		case 1:
		I2C_SendByte(NUM11);
		break;
		case 2:
		I2C_SendByte(NUM21);
		break;
		case 3:
		I2C_SendByte(NUM31);
		break;
		case 4:
		I2C_SendByte(NUM41);
		break;
		case 5:
		I2C_SendByte(NUM51);
		break;
		case 6:
		I2C_SendByte(NUM61);
		break;
		case 7:
		I2C_SendByte(NUM71);
		break;
		case 8:
		I2C_SendByte(NUM81);
		break;
		case 9:
		I2C_SendByte(NUM91);
		break;
		case 0:
		I2C_SendByte(NUM01);
		break;
	}
}

void setnumber2(char number)
{
	switch (number)
	{
		case 1:
		I2C_SendByte(NUM12);
		break;
		case 2:
		I2C_SendByte(NUM22);
		break;
		case 3:
		I2C_SendByte(NUM32);
		break;
		case 4:
		I2C_SendByte(NUM42);
		break;
		case 5:
		I2C_SendByte(NUM52);
		break;
		case 6:
		I2C_SendByte(NUM62);
		break;
		case 7:
		I2C_SendByte(NUM72);
		break;
		case 8:
		I2C_SendByte(NUM82);
		break;
		case 9:
		I2C_SendByte(NUM92);
		break;
		case 0:
		I2C_SendByte(NUM02);
		break;
	}
}

void setnumber3(char number)
{
	switch (number)
	{
		case 1:
		I2C_SendByte(NUM13);
		break;
		case 2:
		I2C_SendByte(NUM23);
		break;
		case 3:
		I2C_SendByte(NUM33);
		break;
		case 4:
		I2C_SendByte(NUM43);
		break;
		case 5:
		I2C_SendByte(NUM53);
		break;
		case 6:
		I2C_SendByte(NUM63);
		break;
		case 7:
		I2C_SendByte(NUM73);
		break;
		case 8:
		I2C_SendByte(NUM83);
		break;
		case 9:
		I2C_SendByte(NUM93);
		break;
		case 0:
		I2C_SendByte(NUM03);
		break;
	}
}

void setnumber_walarm(char number)
{
	switch (number)
	{
		case 1:
		I2C_SendByte(NUM1a);
		break;
		case 2:
		I2C_SendByte(NUM2a);
		break;
		case 3:
		I2C_SendByte(NUM3a);
		break;
		case 4:
		I2C_SendByte(NUM4a);
		break;
		case 5:
		I2C_SendByte(NUM5a);
		break;
		case 6:
		I2C_SendByte(NUM6a);
		break;
		case 7:
		I2C_SendByte(NUM7a);
		break;
		case 8:
		I2C_SendByte(NUM8a);
		break;
		case 9:
		I2C_SendByte(NUM9a);
		break;
		case 0:
		I2C_SendByte(NUM0a);
		break;
	}
}

void setdiginumber(char digit, char number, char set)
{
	I2C_StartCondition();
	setdigit(digit);
	if (set==1)    setnumber1(number);
	else if (set==2)  setnumber2(number);
	else if (set==3)  setnumber3(number);
	I2C_StopCondition();
}

void setdiginumber_walarm(char digit, char number)
{
	I2C_StartCondition();
	setdigit(digit);
	if (alarm1.state == 1) setnumber_walarm(number);
	else setnumber3(number);
	I2C_StopCondition();
}

void setzero(char digit)
{
	I2C_StartCondition();
	setdigit(digit);
	I2C_SendByte(ZERO);
	I2C_StopCondition();
}

void increment_hour(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x02);//Регистр 02
	if (hour <23) 	I2C_SendByte(RTC_ConvertFromBinDec(hour+1));//hour
    else 			I2C_SendByte(RTC_ConvertFromBinDec(0));//hour
	I2C_StopCondition();
}

void increment_min(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x00);//Регистр 00
	I2C_SendByte(RTC_ConvertFromBinDec(0));//sec = 0;
	if (min<59)	I2C_SendByte(RTC_ConvertFromBinDec(min+1));
	else 		I2C_SendByte(RTC_ConvertFromBinDec(0));
	I2C_StopCondition();
}

void increment_alarm_hour(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x09);//Регистр 09
	if (alarm1.hour <23) 	I2C_SendByte(RTC_ConvertFromBinDec(alarm1.hour+1));//hour
    else 			I2C_SendByte(RTC_ConvertFromBinDec(0));//hour
	I2C_StopCondition();
}

void increment_alarm_min(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x07);//Регистр 07
	I2C_SendByte(RTC_ConvertFromBinDec(0));//sec = 0;
	if (alarm1.min<59)	I2C_SendByte(RTC_ConvertFromBinDec(alarm1.min+1));
	else 		I2C_SendByte(RTC_ConvertFromBinDec(0));
	I2C_StopCondition();
}

char RTC_CheckAlarm(void)
{
uint8_t alarm_status; 
I2C_SendByteByADDR(14,RTCADRW);//14 это 0Eh адрес с INTCN&A2IE&A1IE битами
_delay_ms(200);
I2C_StartCondition();
I2C_SendByte(RTCADRR);//читаем
alarm_status = I2C_ReadLastByte();
if ((alarm_status&0b00000001)>0) return 3; //Если последний бит регистра 0Eh равен 1 - будильник включен, INTCN не используется
else return 0;
}

void alarm_reset(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x0F);//Регистр 0F
	I2C_SendByte(0b11001000);//Очистить флаг сработавшего будильника
	I2C_StopCondition();
}

void alarm_on(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x0E);//Регистр 0E
	I2C_SendByte(0b00011101);//Включить будильник
	I2C_StopCondition();
}

void alarm_off(void)
{
	I2C_StartCondition();
	I2C_SendByte(RTCADRW);
	I2C_SendByte(0x0E);//Регистр 0E
	I2C_SendByte(0b00011100);//Выключить будильник
	I2C_StopCondition();
}

void set_indicators(void)
{
		switch (mode)
	{
		case 0://Обычный режим
		PORTB |=(1<<PB0);
	setdiginumber(1, hour%100/10, 1);
	setdiginumber(2, hour%10, 1);
	setdiginumber(3, min%100/10, 2);
    setdiginumber_walarm(4, min%10);
		break;
		
		case 1://Будильник1
		PORTB &= ~(1<<PB0);
	setdiginumber(1, alarm1.hour%100/10, 1);
	setdiginumber(2, alarm1.hour%10, 1);
	setdiginumber(3, alarm1.min%100/10, 2);
    setdiginumber_walarm(4, alarm1.min%10);
		break;
		
		case 2://Время, установка часов
		PORTB &= ~(1<<PB0);
	setdiginumber(1, hour%100/10, 1);
	setdiginumber(2, hour%10, 1);
	setzero(3);
	setzero(4);
		break;
		
		case 3://Время, установка минут
		PORTB &= ~(1<<PB0);
	setzero(1);
	setzero(2);
	setdiginumber(3, min%100/10, 2);
    setdiginumber(4, min%10, 3);
		break;
		
		case 4://Будильник1, установка часов
		PORTB &= ~(1<<PB0);
	setdiginumber(1, alarm1.hour%100/10, 1);
	setdiginumber(2, alarm1.hour%10, 1);
	setzero(3);
	setzero(4);
		break;
		
		case 5://будильник1, установка минут
		PORTB &= ~(1<<PB0);
	setzero(1);
	setzero(2);
	setdiginumber(3, alarm1.min%100/10, 2);
    setdiginumber(4, alarm1.min%10, 3);
		break;
	}
}

void check_flags(void)
{
	if (timef.inchourflag==1)
	{
		increment_hour();
		timef.inchourflag=0;
	}
	else if (timef.incminflag==1)
	{
		increment_min();
		timef.incminflag=0;
	}
	if (alarm1f.inchourflag==1)
	{
		increment_alarm_hour();
		alarm1f.inchourflag=0;
	}
	else if (alarm1f.incminflag==1)
	{
		increment_alarm_min();
		alarm1f.incminflag=0;
	}
	if (alarmresetflag==1)
	{
		alarm_reset();
		alarmresetflag=0;
	}
	if (alarm1.flag==1)//1 если нужно включить будильник
	{
		alarm_on();
		alarm1.state=1;
		alarm1.flag=0;
	}
	else if (alarm1.flag==2)//2 если нужно выключить будильник
	{
		alarm_off();
		alarm1.state=0;
		alarm1.flag=0;
	}
	else if (alarm1.flag==3)//3 для первичной инициализации
	{
		alarm1.state=1;
		alarm1.flag=0;
	}
	
	if (zummerflag==1) 	PORTB |=(1<<PB7);
	else  PORTB &=~(1<<PB7);
}

void debug_signal()
{
		PORTB |= (1<<PB7);
		_delay_ms(100);
}

void initial_show()
{
	I2C_StartCondition();
	I2C_SendByte(DIGIADR1);
	I2C_SendByte(CHAR1);
	I2C_StopCondition();

	I2C_StartCondition();
	I2C_SendByte(DIGIADR2);
	I2C_SendByte(CHAR2);
	I2C_StopCondition();

	I2C_StartCondition();
	I2C_SendByte(DIGIADR3);
	I2C_SendByte(CHAR3);
	I2C_StopCondition();

	I2C_StartCondition();
	I2C_SendByte(DIGIADR4);
	I2C_SendByte(CHAR4);
	I2C_StopCondition();
}

int main(void)
{
	EICRA = 0b00001010;//INT1&0 в режиме нисходящего фронта
	EIMSK |= (1<<INT1);//INT1 прерывание включено
	DDRD =  0x00;
	PORTD = 0xFF;
	DDRB =  0b11111111;
	PORTB &=~(1<<PB0); //светодиод
	PORTB &=~(1<<PB7); //зуммер
	
	/*timer2 Таймер 2 не используется, хотя планировался
	TIMSK2 &= ~(1<<OCIE2A);
	TIMSK2 &= ~(1<<OCIE2B);
    TIMSK2 &= ~(1<<TOIE2);
	ASSR |=(1<<AS2);//асинхронный режим
	ASSR |=(1<<EXCLK);//внешний клок для таймера
	TCNT2 = 1;
	OCR2A = 255; //
	TCCR2A |= 1<<WGM21;
	TCCR2B |=(1<<CS21)|(1<<CS22);//256 прескалер - 128 тиков в секунду
	TIFR2 &=~(1<<OCF2A);
	TIFR2 &=~(1<<OCF2B);
    TIFR2 &=~(1<<TOV2);
	TIMSK2 |= (1<<OCIE2A);
	*/
	
	//timer0
	TIMSK0 &= ~(1<<OCIE0A);
	TIMSK0 &= ~(1<<OCIE0B);
	TIMSK0 &= ~(1<<TOIE0);
	TCNT0 = 0;
	OCR0A = 255;
	TIFR0 &=~(1<<OCF0A);
	TIFR0 &=~(1<<OCF0B);
    TIFR0 &=~(1<<TOV0);
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS02)|(1<<CS00);
	TIMSK0 |= (1<<OCIE0A);
	//timer0
	
	cli();
	I2C_Init(); //Включить I2C
	_delay_ms(100);//
	if ((!(PIND&0b11000000))) RTC_Set();//Если зажать при старте 2 кнопки то произойдет первичная запись в RTC
	alarm1.flag = RTC_CheckAlarm();//Проверить при включении, включен ли таймер в RTC
	check_flags(); // Обработчик флагов
	_delay_ms(100);
	initial_show();
	_delay_ms(1000);
	sei(); //Вперед, прерывания
	
    while(1)
    {
	RTC_Read();//Прочитать RTC
	set_indicators();//Установить все индикаторы
	check_flags();//Обработать все флаги
    }
}