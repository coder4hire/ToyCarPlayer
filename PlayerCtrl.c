#define F_CPU 1200000UL

#include "PlayerCtrl.h"
#include "uart13.h"
#include <util/delay.h>


void ctrlInit()
{
	uart_init();
}

void ctrlPlaySong(uint8_t songID)
{
	uart_send("A7:00");
	uart_send1(songID/100+'0');
	uart_send1((songID/10)%10+'0');
	uart_send1(songID%10+'0');
//	uart_send("A8:00/C1*/00002*mp3");
//	uart_send1(songID/100+'0');
//	uart_send1((songID/10)%10+'0');
//	uart_send1(songID%10+'0');
//	uart_send("*???");
	_delay_ms(500);
//	ctrlPlay();
}

void ctrlPlay()
{
	uart_send("A2");
	_delay_ms(300);
}

void ctrlStop()
{
	uart_send("A4");
	_delay_ms(300);
}

void ctrlNext()
{
	uart_send("A5");
	_delay_ms(300);
}

void ctrlReset()
{
	uart_send("AE");
	_delay_ms(300);
}

void ctrlSetVolume(uint8_t vol)
{
	uart_send("AF:");

	uart_send1((vol/10)%10+'0');
	uart_send1(vol%10+'0');
	_delay_ms(300);
}

void ctrlLoop()
{
	uart_send("B4:01");
	_delay_ms(300);
}

void ctrlPlaySequentially()
{
	uart_send("B4:04");
	_delay_ms(300);
}
