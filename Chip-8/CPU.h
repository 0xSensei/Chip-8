#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"

#include "keyboard.h"

typedef  unsigned char BYTE;
typedef  unsigned short WORD;
typedef struct {
	BYTE v0;
	BYTE v1;
	BYTE v2;
	BYTE v3;
	BYTE v4;
	BYTE v5;
	BYTE v6;
	BYTE v7;
	BYTE v8;
	BYTE v9;
	BYTE va;
	BYTE vb;
	BYTE vc;
	BYTE vd;
	BYTE ve;
	BYTE vf;	// Flags
	WORD I;
} Registers;


class CPU
{
public:
	CPU(const char* RomPath);
	~CPU();



	
private:
	BYTE Memory[4096];
	WORD * pc;
	WORD stack[16];
	WORD * sp;
	BYTE registers[0x10];
	WORD I;
	keyboard kb;

	int ReadRom(const char* RomPath);
	void main_loop();
	int set_appropriate_register(BYTE Vx, BYTE val);
	// OPCODES

	void cls();
	void ret();
	void abs_jmp();
	void call();

	// conditional branching 
	void skip();


	void store();
	
	int add_val_to_register(BYTE Vx, BYTE val);
	int add_register_to_register(BYTE Vx, BYTE Vy);


	void _sub(BYTE Vx, BYTE Vy, int Mode); // if Mode = 1, Vx = Vx - Vy. else Vy = Vy - Vx

	void _or();
	void _and();
	void _xor();


	void _shift();

	int add_vx_to_I(BYTE Vx);


};

