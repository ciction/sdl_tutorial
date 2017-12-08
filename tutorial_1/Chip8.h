#pragma once
class Chip8
{
public:
	Chip8();
	~Chip8();
private:
	//current operation code
	unsigned short opcode;
	//total memory
	unsigned char memory[4096];
	//registers 
	unsigned char V[16];
	//index register
	unsigned short I;
	//program counter
	unsigned short pc;

	//grafix pixel buffer
	unsigned char gfx[64 * 32];

	unsigned char delay_timer;
	unsigned char sound_timer;


	//operations stack and stackPointer
	unsigned short stack[16];
	unsigned short sp;

	//HEX based keypad
	unsigned char key[16];
};

