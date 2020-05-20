#pragma once
#include "SDL.h"

typedef  unsigned char BYTE;


class keyboard
{
public:
	keyboard();
	~keyboard();
	BYTE key_pressed;


	BYTE _listen_key(); //wait

	BYTE check_pressed(BYTE key);
	BYTE check_released(BYTE key);

};

