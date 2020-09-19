/*
 * CarPlayerTiny.cpp
 *
 * Created: 25.07.2020 1:18:09
 */ 

// 1.2 MHz (default) built in resonator
#define F_CPU 1200000UL

#include <avr/io.h>
#include <util/delay.h>
#include "PlayerCtrl.h"

#define LED_BIT _BV(PB2)

#define MODE_ENGINE 0
#define MODE_MUSIC 1
#define IN_PB3 (PINB & 0x08)
#define IN_PB4 (PINB & 0x10)

#define WHEEL_SENSOR IN_PB4
#define BUTTONS_SENSOR MUX3

#define WHEEL_ROTATION_INTERVAL 20
#define LONG_BUTTON_PRESS_INTERVAL 10

uint8_t mode = MODE_ENGINE;

typedef enum
{
	BTN_NONE=0,
	BTN_LEFT,
	BTN_RIGHT,
	BTN_OK,
	BTN_LEFT_LONG=0x11,
	BTN_RIGHT_LONG,
	BTN_OK_LONG
	
} BUTTONS;

void Blink(int times)
{
	for(int i=0;i<times;i++)
	{
		PORTB |= LED_BIT; // HIGH
		_delay_ms(50);
		PORTB &= ~LED_BIT; // LOW
		_delay_ms(50);
	}
}

void setEngineMode()
{
	mode=MODE_ENGINE;	
	ctrlStop();
	ctrlLoop();	
	ctrlPlaySong(1);
}

void setMusicMode()
{
	mode=MODE_MUSIC;	
	ctrlStop();
	ctrlPlaySong(4);
	ctrlPlaySequentially();	
}

BUTTONS GetRawBtn()
{
	uint8_t val;
	
	// Reading ADC
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	val = ADCH;
	
	// Processing key presses
	if(val<10)
	{
		return BTN_OK;
	}
	else if(val<55)
	{
		return BTN_LEFT;
	}
	else if(val<130)
	{
		return BTN_RIGHT;
	}	
	return BTN_NONE;
}

BUTTONS GetButton()
{
	static uint8_t pressCounter=0;	
	BUTTONS btn;
	
	btn = GetRawBtn();
	_delay_ms(50);
	if(btn==BTN_NONE || btn!=GetRawBtn())
	{
		pressCounter=0;
		return BTN_NONE;
	}

	if(pressCounter==0)
	{
		pressCounter++;		
		return btn;
	}
	
	pressCounter++;
	if(pressCounter>=LONG_BUTTON_PRESS_INTERVAL)
	{
		_delay_ms(50);
		return btn+0x10;
	}

	return BTN_NONE;
}

#define EEPROM_write(uiAddress, ucData)\
{\
	while(EECR & (1<<EEPE));\
	EEAR = uiAddress;\
	EEDR = ucData;\
	EECR |= (1<<EEMPE);\
	EECR |= (1<<EEPE);\
}

#define EEPROM_read(var, uiAddress)\
{\
	while(EECR & (1<<EEWE));\
	EEAR = uiAddress;\
	EECR |= (1<<EERE);\
	var=EEDR;\
}

int main()
{
	uint8_t volume;
	uint8_t wheelCounter=0;
	uint8_t engineSound=1;
	uint8_t idleEngineCounter=0;

	DDRB |= LED_BIT; // OUTPUT

	DDRB &= ~(1 << DDB4); //set PB4 as wheel input

	ADMUX |= 3; // PB3 as buttons input (ADC)
	ADMUX |= (1 << ADLAR);
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

	ctrlInit();
	_delay_ms(2500);
		
	EEPROM_read(volume,0);
	if(volume>31 || volume<1)
	{
		volume=10;
	}
//	Blink(volume);	
	
	ctrlSetVolume(volume);
	setEngineMode();
	
	while(1)
	{
		switch(GetButton())
		{
		case BTN_OK:
			if(mode==MODE_ENGINE)
			{
				setMusicMode();
			}
			else
			{
				setEngineMode();
			}
			break;
		case BTN_LEFT:
			if(mode == MODE_MUSIC)
			{
				ctrlStop();
				ctrlPlay();
			}
			break;
		case BTN_RIGHT:
			if(mode == MODE_MUSIC)
			{
				ctrlNext();
			}
			break;
		case BTN_LEFT_LONG:
			if(volume>1)
			{
				volume--;
				ctrlSetVolume(volume);				
			}
			break;
		case BTN_RIGHT_LONG:
			if(volume<31)
			{
				volume++;
				ctrlSetVolume(volume);
			}
			break;
		case BTN_OK_LONG:
			EEPROM_write(0,volume);
			for(int i=0;i<4;i++)
			{
				ctrlPlaySong(2-i%2);
				_delay_ms(1000);
			}
			break;
		default:
			break;
		}
			
			
		if(wheelCounter)
		{
			wheelCounter--;
		}
		if(!WHEEL_SENSOR)
		{
			wheelCounter=WHEEL_ROTATION_INTERVAL;
		}

		idleEngineCounter++; // this is watchdog for engine sound, MP3 module is not reliable
		if(mode==MODE_ENGINE)
		{	
			if(wheelCounter && engineSound==1)
			{
				engineSound=2;
				ctrlPlaySong(2);
				ctrlPlaySong(2);
			}
			else if((!wheelCounter && engineSound==2) || (!idleEngineCounter&& engineSound==1))
			{
				engineSound=1;
				ctrlPlaySong(1);
				//ctrlReset();
				ctrlLoop();
				ctrlPlaySong(1);
			}			
		}
		// No need to delay here, there's 50 delay in GetButton
	}
}