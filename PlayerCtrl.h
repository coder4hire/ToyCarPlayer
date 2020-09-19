#pragma once
#include <avr/io.h>

	void ctrlInit();
	void ctrlPlaySong(uint8_t songID);
	void ctrlPlay();
	void ctrlStop();
	void ctrlNext();
	void ctrlReset();
	void ctrlSetVolume(uint8_t vol);
	
	void ctrlLoop();
	void ctrlPlaySequentially();

