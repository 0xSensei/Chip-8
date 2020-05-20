#pragma once


typedef  unsigned char BYTE;


class keyboard
{
public:
	keyboard();
	~keyboard();
	BYTE key_pressed;


	BYTE _listen_key();
};

