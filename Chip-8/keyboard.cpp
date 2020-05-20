#include "pch.h"
#include "keyboard.h"
#include "stdio.h"
#include <conio.h>
#include "windows.h"



keyboard::keyboard()
{
}


keyboard::~keyboard()
{
}

BYTE keyboard::_listen_key()
{
	BYTE key = 0x0;
	key = _getch();
	//scanf_s("%c", &key);


	
	if (key >= 48 && key <= 57) { // 0-9
		key_pressed = key - 48;
	}
	else if (key >= 65 && key <= 70) {// A-F
		key_pressed = key - 65+ 0x0A;
	}
	else if (key >= 97 && key <= 102) {// a-f
		key_pressed = key - 97 + 0x0A;
	}

	return key_pressed;
}

BYTE keyboard::check_pressed(BYTE key)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	
	
	// first check the value
	if (key < 0x0A) {
		if (state[key + 48]) {
			return 1;
		}
	}
	// check if a
	key += 65;
	if ((key >= 65 && key <= 70)) {
		if (state[key]) {
			return 1;
		}
	}

	return 0;
}

BYTE keyboard::check_released(BYTE key)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	// first check the value
	if (key < 0x0A) {
		if (state[key + 48]) {
			return 0;
		}
	}
	// check if a
	key += 65;
	if ((key >= 65 && key <= 70)) {
		if (state[key]) {
			return 0;
		}
	}

	return 1;
}
