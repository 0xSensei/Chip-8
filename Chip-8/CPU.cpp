#include "pch.h"
#include "CPU.h"

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"

// little Endiann to Big Endiann
#define SWAP_UINT16(x) ( (WORD) (((x) >> 8) | ((x) << 8)))

// extract First Byte
#define ext_first(x)  ((SWAP_UINT16(x) & 0xF000) >> 12)
#define ext_second(x)  ((SWAP_UINT16(x) & 0x0F00) >> 8)
#define ext_third(x)  ((SWAP_UINT16(x) & 0x00F0) >> 4)
#define ext_fourth(x)  ((SWAP_UINT16(x) & 0x000F) >> 0)

#define ext_lastTwo(x) ((SWAP_UINT16(x) & 0x00FF) >> 0)
#define ext_lastThree(x) ((SWAP_UINT16(x) & 0x0FFF) >> 0)

#define ext_TwoMiddle(x) ((SWAP_UINT16(x) & 0x0FF0) >> 4)



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
	this->sp = stack;
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
		printf("OPCODE: %X \n", SWAP_UINT16(*this->pc));
		char opcode =ext_first(*this->pc);
		WORD jmp;
		switch (opcode)
		{
		case 0:
			if (((char*)this->pc)[3] == 0) {
				//cls(); // 00E0	Clear the screen
			}
			else {
				//ret(); // 00EE	Return from a subroutine
			}
			break;
		case 1: // Jump absuloute
			// since its little endian we have to reverse our logic
			jmp = ext_lastThree(*this->pc);
			this->pc = (WORD*)&this->Memory[jmp];
			continue;
			break;
		case 2:
			// 2NNN	Execute subroutine starting at address NNN
			*this->sp = *(this->pc++);
			*this->pc = *this->pc & 0x0FFF;
			continue;
			break;
		case 3: // 3XNN	Skip the following instruction if the value of register VX equals 
		case 4: // 4XNN	Skip the following instruction if the value of register VX is not equal to NN
		case 5: // 5XY0	Skip the following instruction if the value of register VX is equal to the value of register VY
			skip(); // will handle all logic
			continue;
			break;
		case 6: 
			// 6XNN	Store number NN in register VX
			/*BYTE val = ext_lastTwo(*this->pc);
			BYTE choice = ext_second(*this->pc);*/
			set_appropriate_register(ext_second(*this->pc),
										ext_lastTwo(*this->pc));
			break;
		case 7: 
			// 7XNN	Add the value NN to register VX
			add_val_to_register(ext_second(*this->pc),
				ext_lastTwo(*this->pc));
			break;
		case 8:
			// 8XY0	Store the value of register VY in register VX
			if (((char*)this->pc)[3] == 0) {
				//store();
			}
			else if (((char*)this->pc)[3] == 1) {
				// 8XY1	Set VX to VX OR VY

			}
			else if (((char*)this->pc)[3] == 2) {
				// 8XY2	Set VX to VX AND VY

			}
			else if (((char*)this->pc)[3] == 3) {
				// 8XY3	Set VX to VX XOR VY

			}
			else if (((char*)this->pc)[3] == 4) {
				// 8XY4	Add the value of register VY to register VX
				// Set VF to 01 if a carry occurs
				//	Set VF to 00 if a carry does not occur
			}
			else if (((char*)this->pc)[3] == 5) {
				// 8XY5	Subtract the value of register VY from register VX
				// Set VF to 00 if a borrow occurs
				//	Set VF to 01 if a borrow does not occur
			}
			else if (((char*)this->pc)[3] == 6) {
				// 8XY6	Store the value of register VY shifted right one bit in register VX
				// Set register VF to the least significant bit prior to the shift
			}
			else if (((char*)this->pc)[3] == 7) {
				// 8XY7	Set register VX to the value of VY minus VX
				// Set VF to 00 if a borrow occurs
				//	Set VF to 01 if a borrow does not occur
			}
			else if (((char*)this->pc)[3] == 0xE) {
				// 8XYE	Store the value of register VY shifted left one bit in register VX
				// Set register VF to the most significant bit prior to the shift
			}
			
			break;
		case 9:
			// 9XY0	Skip the following instruction if the value of register VX is not equal to the value of register VY

			break;
		case 0xA:
			// ANNN	Store memory address NNN in register I
			this->registers.I = SWAP_UINT16(*this->pc) & 0x0FFF;
			break;
		case 0xB:
			// BNNN	Jump to address NNN + V0
			break;
		case 0xC:
			// CXNN	Set VX to a random number with a mask of NN
			break;
		case 0xD:
			// DXYN	Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
			// Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
			break;
		case 0xE:
			// EX9E	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
			// EXA1	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
			break;
		case 0xF:
			// FX07	Store the current value of the delay timer in register VX
			// FX0A	Wait for a keypress and store the result in register VX
			// FX15	Set the delay timer to the value of register VX
			// FX18	Set the sound timer to the value of register VX
			// FX29	Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
			// FX33	Store the binary - coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
			// FX55	Store the values of registers V0 to VX inclusive in memory starting at address I
			// I is set to I + X + 1 after operation
			// FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
			// I is set to I + X + 1 after operation

			// FX1E	Add the value stored in register VX to register I
			if (ext_lastTwo(*this->pc) == 0x1E) {
				add_vx_to_I(ext_second(*this->pc));
			}

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



int CPU::set_appropriate_register(BYTE Vx, BYTE val)
{
	switch (Vx)
	{
	case 0x00:
		registers.v0 = val;
		break;
	case 0x01:
		registers.v1 = val;
		break;
	case 0x02:
		registers.v2 = val;
		break;
	case 0x03:
		registers.v3 = val;
		break;
	case 0x04:
		registers.v4 = val;
		break;
	case 0x05:
		registers.v5 = val;
		break;
	case 0x06:
		registers.v6 = val;
		break;
	case 0x07:
		registers.v7 = val;
		break;
	case 0x08:
		registers.v8 = val;
		break;
	case 0x09:
		registers.v9 = val;
		break;
	case 0x0A:
		registers.va = val;
		break;
	case 0x0B:
		registers.vb = val;
		break;
	case 0x0C:
		registers.vc = val;
		break;
	case 0x0D:
		registers.vd = val;
		break;
	case 0x0E:
		registers.ve = val;
		break;
	default:
		return -1;
		break;
	}

	return 0;
}

void CPU::skip()
{

			// 3XNN	Skip the following instruction if the value of register VX equals 
			// 4XNN	Skip the following instruction if the value of register VX is not equal to NN
			// 5XY0	Skip the following instruction if the value of register VX is equal to the value of register VY
	BYTE opcode = ext_first(*this->pc);
	BYTE Vx;
	BYTE Vy;
	BYTE val;
	if (opcode == 0x03) {
		Vx = ext_second(*this->pc);
		val = ext_lastTwo(*this->pc);
		switch (Vx)
			{
				case 0x00:
					registers.v0 += val;
					break;
				case 0x01:
					registers.v1 += val;
					break;
				case 0x02:
					registers.v2 += val;
					break;
				case 0x03:
					registers.v3 += val;
					break;
				case 0x04:
					registers.v4 += val;
					break;
				case 0x05:
					registers.v5 += val;
					break;
				case 0x06:
					registers.v6 += val;
					break;
				case 0x07:
					registers.v7 += val;
					break;
				case 0x08:
					registers.v8 += val;
					break;
				case 0x09:
					registers.v9 += val;
					break;
				case 0x0A:
					registers.va += val;
					break;
				case 0x0B:
					registers.vb += val;
					break;
				case 0x0C:
					registers.vc += val;
					break;
				case 0x0D:
					registers.vd += val;
					break;
				case 0x0E:
					registers.ve += val;
					break;
				default:
			}

	}
	else if (ext_first(*this->pc) == 0x04) {

	}
	else if (ext_first(*this->pc) == 0x05) {

	}




}

int CPU::add_val_to_register(BYTE Vx, BYTE val)
{
	switch (Vx)
	{
	case 0x00:
		registers.v0 += val;
		break;
	case 0x01:
		registers.v1 += val;
		break;
	case 0x02:
		registers.v2 += val;
		break;
	case 0x03:
		registers.v3 += val;
		break;
	case 0x04:
		registers.v4 += val;
		break;
	case 0x05:
		registers.v5 += val;
		break;
	case 0x06:
		registers.v6 += val;
		break;
	case 0x07:
		registers.v7 += val;
		break;
	case 0x08:
		registers.v8 += val;
		break;
	case 0x09:
		registers.v9 += val;
		break;
	case 0x0A:
		registers.va += val;
		break;
	case 0x0B:
		registers.vb += val;
		break;
	case 0x0C:
		registers.vc += val;
		break;
	case 0x0D:
		registers.vd += val;
		break;
	case 0x0E:
		registers.ve += val;
		break;
	default:
		return -1;
	}

	return 0;
}

int CPU::add_vx_to_I(BYTE Vx)
{
	switch (Vx)
	{
	case 0x00:
		registers.I += registers.v0;
		break;
	case 0x01:
		registers.I += registers.v1;
		break;
	case 0x02:
		registers.I += registers.v2;
		break;
	case 0x03:
		registers.I += registers.v3;
		break;
	case 0x04:
		registers.I += registers.v4;
		break;
	case 0x05:
		registers.I += registers.v5;
		break;
	case 0x06:
		registers.I += registers.v6;
		break;
	case 0x07:
		registers.I += registers.v7;
		break;
	case 0x08:
		registers.I += registers.v8;
		break;
	case 0x09:
		registers.I += registers.v9;
		break;
	case 0x0A:
		registers.I += registers.va;
		break;
	case 0x0B:
		registers.I += registers.vb;
		break;
	case 0x0C:
		registers.I += registers.vc;
		break;
	case 0x0D:
		registers.I += registers.vd;
		break;
	case 0x0E:
		registers.I += registers.ve;
		break;
	default:
		return -1;
		break;
	}

	return 0;

}
