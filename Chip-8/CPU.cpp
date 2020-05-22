#include "pch.h"
#include "CPU.h"



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


	srand(0);

	// clean up the state of registers and 
	memset(this->Memory, 0, 4096);
	memset(this->stack, 0, 2*16);

	memset(&this->registers, 0, sizeof(Registers));

	this->I = 0;
	delay_timer = 0;
	sound_timers = 0;


	// Read Rom
	long size = ReadRom(RomPath);
	if (size == -1) {
		return;
	}


	// set up needed registers
	this->pc = (WORD *) &this->Memory[0x200];
	this->sp = stack;


	gr.init();
	main_loop();
}



CPU::~CPU()
{
}

// FX29

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
	WORD dbg = 0;
	while (1) {
		// fetch & print opcodes
		printf("OPCODE: %X \n", SWAP_UINT16(*this->pc));
		char opcode =ext_first(*this->pc);
		WORD jmp;
		switch (opcode)
		{
		case 0:
			if (ext_fourth(*this->pc) == 0) {
				//cls(); // 00E0	Clear the screen
			}
			else {
				//ret(); // 00EE	Return from a subroutine
				this->sp--;
				this->pc = (WORD*) &this->Memory[*this->sp];
				
				continue;
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
			
			//dbg = (((BYTE*)this->pc - this->Memory));
			*this->sp = (WORD)    (((BYTE*)this->pc - this->Memory)) + 2;
			this->sp++;
			this->pc = (WORD *) (ext_lastThree(*this->pc) + this->Memory);
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
			if (ext_fourth(*this->pc)  == 0) {
				//store();
				registers[ext_second(*this->pc)] = registers[ext_third(*this->pc)];
			}
			else if (ext_fourth(*this->pc) == 1) {
				// 8XY1	Set VX to VX OR VY
				registers[ext_second(*this->pc)] = registers[ext_second(*this->pc)] | registers[ext_third(*this->pc)];
			}
			else if (ext_fourth(*this->pc) == 2) {
				// 8XY2	Set VX to VX AND VY
				registers[ext_second(*this->pc)] = registers[ext_second(*this->pc)] & registers[ext_third(*this->pc)];
			}
			else if (ext_fourth(*this->pc) == 3) {
				// 8XY3	Set VX to VX XOR VY
				registers[ext_second(*this->pc)] = registers[ext_second(*this->pc)] ^ registers[ext_third(*this->pc)];
			}
			else if (ext_fourth(*this->pc) == 4) {
				// 8XY4	Add the value of register VY to register VX
				// Set VF to 01 if a carry occurs
				//	Set VF to 00 if a carry does not occur
				add_register_to_register(ext_second(*this->pc),
					ext_third(*this->pc));

			}
			else if (ext_fourth(*this->pc) == 5) {
				// 8XY5	Subtract the value of register VY from register VX
				// Set VF to 00 if a borrow occurs
				// Set VF to 01 if a borrow does not occur
				// Vx = Vx - Vy
				_sub(ext_second(*this->pc),
					ext_third(*this->pc),
					1);

			}
			else if (ext_fourth(*this->pc) == 6) {
				// 8XY6	Store the value of register VY shifted right one bit in register VX
				// Set register VF to the least significant bit prior to the shift
				registers[0x0F] = registers[ext_third(*this->pc)] & 1;
				registers[ext_second(*this->pc)] = registers[ext_third(*this->pc)] >> 1;
			}
			else if (ext_fourth(*this->pc) == 7) {
				// 8XY7	Set register VX to the value of VY minus VX
				// Set VF to 00 if a borrow occurs
				// Set VF to 01 if a borrow does not occur
				// Vy = Vy - Vx
				_sub(ext_second(*this->pc),
					ext_third(*this->pc),
					0);
			}
			else if (ext_fourth(*this->pc) == 0xE) {
				// 8XYE	Store the value of register VY shifted left one bit in register VX
				// Set register VF to the most significant bit prior to the shift
				registers[0x0F] = (registers[ext_third(*this->pc)] >> 7) & 1;
				registers[ext_second(*this->pc)] = registers[ext_third(*this->pc)] << 1;
			}
			
			break;
		case 9:
			// 9XY0	Skip the following instruction if the value of register VX is not equal to the value of register VY
			skip();
			break;
		case 0xA:
			// ANNN	Store memory address NNN in register I, fix into ext_lastThrea
			this->I = SWAP_UINT16(*this->pc) & 0x0FFF;
			break;
		case 0xB:
			// BNNN	Jump to address NNN + V0
			this->pc = (WORD*) this->Memory[ext_lastThree(*this->pc)] + registers[0];
			break;
		case 0xC:
			// CXNN	Set VX to a random number with a mask of NN
			this->registers[ext_second(*this->pc)] = rand() & ext_lastTwo(*this->pc);

			break;
		case 0xD:
			// DXYN	Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
			// Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
			break;
		case 0xE:
			// EX9E	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
			// EXA1	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
			if (ext_lastTwo(*this->pc) == 0x9E) {
				if (kb.check_pressed(registers[ext_second(*this->pc)])) {
					this->pc++; // if pressed it will be pc+4
				}
			}
			else if (ext_lastTwo(*this->pc) == 0xA1) {
				if (kb.check_released(registers[ext_second(*this->pc)])) {
					this->pc++; // if pressed it will be pc+4
				}
			}
			break;
		case 0xF:
			
			

			// FX1E	Add the value stored in register VX to register I
			if (ext_lastTwo(*this->pc) == 0x1E) {
				add_vx_to_I(ext_second(*this->pc));
			}
			// FX0A	Wait for a keypress and store the result in register VX	
			else if (ext_lastTwo(*this->pc) == 0x0A) {		
				registers[ext_second(*this->pc)] = kb._listen_key();
			}
			// FX15	Set the delay timer to the value of register VX
			else if (ext_lastTwo(*this->pc) == 0x15) {			
				this->delay_timer = ext_second(*this->pc);
			}
			// FX07	Store the current value of the delay timer in register 
			else if (ext_lastTwo(*this->pc) == 0x07) {
				registers[ext_second(*this->pc)] = this->delay_timer;
			}
			// FX18	Set the sound timer to the value of register VX
			else if (ext_lastTwo(*this->pc) == 0x18) {
				 this->sound_timers = registers[ext_second(*this->pc)];
			}
			// FX29	Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
			else if (ext_lastTwo(*this->pc) == 0x29) {
				this->I = registers[ext_second(*this->pc)];
			}
			// FX55	Store the values of registers V0 to VX inclusive in memory starting at address I
			// I is set to I + X + 1 after operation
			else if (ext_lastTwo(*this->pc) == 0x55) {
				for (BYTE i = 0; i <= ext_second(*this->pc); i++, this->I++)
				{
					this->Memory[this->I] = registers[i];
				}
				this->I++;
			}
			// FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
			// I is set to I + X + 1 after operation
			else if (ext_lastTwo(*this->pc) == 0x65) {
				for (BYTE i = 0; i <= ext_second(*this->pc); i++, this->I++)
				{
					registers[i] = this->Memory[this->I];
				}
				this->I++;
			}
			// FX33	Store the binary - coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
			else if (ext_lastTwo(*this->pc) == 0x33) {
				
				_bcd(ext_second(*this->pc));
				
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
	registers[Vx] = val;
	return 0;
}

void CPU::skip()
{

			// 3XNN	Skip the following instruction if the value of register VX equals 
			// 4XNN	Skip the following instruction if the value of register VX is not equal to NN
			// 5XY0	Skip the following instruction if the value of register VX is equal to the value of register VY
			// 9XY0	Skip the following instruction if the value of register VX is not equal to the value of register VY

	BYTE opcode = ext_first(*this->pc);
	BYTE Vx;
	BYTE Vy;
	BYTE val;
	if (opcode == 0x03) {
		Vx = ext_second(*this->pc);
		val = ext_lastTwo(*this->pc);
		if (registers[Vx] == val) {
			this->pc++;
			this->pc++;
			return;
		}

	}
	else if (ext_first(*this->pc) == 0x04) {
		Vx = ext_second(*this->pc);
		val = ext_lastTwo(*this->pc);
		if (registers[Vx] != val) {
			this->pc++;
			this->pc++;
			return;
		}
	}
	else if (ext_first(*this->pc) == 0x05) {
		Vx = ext_second(*this->pc);
		Vy = ext_third(*this->pc);
		if (registers[Vx] == registers[Vy]) {
			this->pc++;
			this->pc++;
			return;
		}
	}
	else if (ext_first(*this->pc) == 0x09) {
		Vx = ext_second(*this->pc);
		Vy = ext_third(*this->pc);
		if (registers[Vx] != registers[Vy]) {
			this->pc++;
			this->pc++;
			return;
		}
	}

	this->pc++;
	return;


}

int CPU::add_val_to_register(BYTE Vx, BYTE val)
{
			//registers.v0 += val;
			// note in case of Overflow save registers in bigger type and modulo 255 to get the correct values
	WORD tmp_vx = 0;
	tmp_vx = registers[Vx];
	tmp_vx += val;
	registers[Vx] = tmp_vx % 256;
	return 0;

}

int CPU::add_register_to_register(BYTE Vx, BYTE Vy)
{
	WORD tmp_vx = 0;
	WORD val = 0;
	tmp_vx = registers[Vx];
	tmp_vx += val;
	registers[0xF] = tmp_vx >> 8; // carry
	registers[Vx] = tmp_vx % 256; // incase of overflow, we could use 0x00FF
	return 0;
	
}

void CPU::_bcd(BYTE Vx)
{
	// FX33	Store the binary - coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
	BYTE val = registers[Vx];
	this->Memory[this->I] = val / 100;
	this->Memory[this->I + 1] = (val / 10) % 10;
	this->Memory[this->I + 2] = (val % 100) % 10;


}

void CPU::_sub(BYTE Vx, BYTE Vy, int Mode)
{
	signed short tmp= 0;
	if (Mode) {
		tmp = registers[Vx];
		tmp = registers[Vx] - registers[Vy];
		if (tmp <= 0) {
			registers[0x0F] = 0;
			registers[Vx] = tmp & 0x00FF;
		}
	}
	else {
		tmp = registers[Vy];
		tmp = registers[Vy] - registers[Vx];
		if (tmp <= 0) {
			registers[0x0F] = 0;
			registers[Vy] = tmp & 0x00FF;
		}
	}


}

int CPU::add_vx_to_I(BYTE Vx)
{
	this->I += registers[Vx];
	return 0;


}
