#pragma once

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
	Registers registers;


	int ReadRom(const char* RomPath);
	void main_loop();

	// OPCODES

	void cls();
	void ret();
	void abs_jmp();
	void call();

	// conditional branching 
	void skip();


	void store();
	
	void add();
	void sub();

	void _or();
	void _and();
	void _xor();


	void _shift();



};

