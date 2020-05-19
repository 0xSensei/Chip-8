#include "pch.h"
#include "CPU.h"

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"


CPU::CPU(const char * RomPath)
{


	

	// clean up the state of registers and 
	memset(this->Memory, 0, 4096);
	memset(this->stack, 0, 2*16);

	memset(&this->registers, 0, sizeof(Registers));




	// Read Rom
	long size = ReadRom(RomPath);
	if (size == -1) {
		return;
	}


	// set up needed registers
	this->pc = (WORD *) &this->Memory[0x200];

	main_loop();
}



CPU::~CPU()
{
}



// open a ROM and Load it in memory and return the file size. if not return -1
int CPU::ReadRom(const char * RomPath)
{

	FILE * rom;
	fopen_s(&rom, RomPath, "r");
	if (rom == NULL) {
		printf("There was an error opening the ROM\n");
		system("pause");
		return -1;
	}
	fseek(rom, 0, SEEK_END);
	long size = ftell(rom);;
	fseek(rom, 0, SEEK_SET);
	fread(&this->Memory[0x200], 1,size, rom);
	printf("[+] Read the Content of ROM into memory\n");
	return size;
}

void CPU::main_loop()
{
	while (1) {
		// fetch & print opcodes
		printf("OPCODE: %X \n", *this->pc);

		switch (*(char*) this->pc)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 0xA:
			break;
		case 0xB:
			break;
		case 0xC:
			break;
		case 0xD:
			break;
		case 0xE:
			break;
		case 0xF:
			break;
		default:
			break;
		}




		if (!*this->pc) {
			printf("Reached Zero at PC\n");
			break;
		}
		this->pc++;

	}

}
