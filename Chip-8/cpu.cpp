#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <fstream>
#include "time.h"
#include <string>
#include "cpu.hpp"
#include "util.h"

//0x000 to 0x1FF were reserved for Chip-8 Interpreter
const unsigned int START_ADDRESS = 0x200;

//16 characters, 0-F, 5 bytes each, starting at 0x50
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//function definitions

void cpu::init()
{
	//initialize Program Counter at 0x200
	programCounter = START_ADDRESS;
	
	//load fonts into memory starting from 0x50
	for(unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}
	
}

//emulates one cycle of CPU
void cpu::cycle()
{
	//Fetch
	//opcodes are 16 bit, memory is 8 bit, use OR to combine memory at pc and pc+1
	opcode = (memory[programCounter] << 8u | memory[programCounter + 1]);
	std::cout << "\n" << "opcode: ";
	std::cout << std::hex << opcode;
	std::cout << "\n";
	
	//increment PC
	programCounter += 2;

	//Decode & Execute
	uint16_t val;
	switch(first(opcode))
	{
		case 0x0000:
			if(opcode == 0x00E0)
			{
				OP_00E0();
			}
			if(opcode == 0x00EE)
			{
				OP_00EE();
			}
			break;

		case 0x1000:
			OP_1nnn();
			break;
		case 0x2000:
			OP_2nnn();
			break;
		case 0x3000:
			OP_3xkk();
			break;
		case 0x4000:
			OP_4xkk();
			break;
		case 0x5000:
			OP_5xy0();
			break;
		case 0x6000:
			OP_6xkk();
			break;
		case 0x7000:
			OP_7xkk();
			break;

		case 0x8000:
			std::cout << "\n 8000 REACHED \n";
			std::cout << "VAL: " << std::hex << last(opcode);
			std::cout << "\n";
			switch(last(opcode))
			{
				case 0x0:
					OP_8xy0();
					break;
				case 0x1:
					std::cout << "\n 1 \n";
					OP_8xy1();
					break;
				case 0x2:
					OP_8xy2();
					break;
				case 0x3:
					OP_8xy3();
					break;
				case 0x4:
					OP_8xy4();
					break;
				case 0x5:
					OP_8xy5();
					break;
				case 0x6:
					OP_8xy6();
					break;
				case 0x7:
					OP_8xy7();
					break;
				case 0xE:
					OP_8xyE();
					break;
			}

		case 0xA000:
			OP_Annn();
			break;
		case 0xB000:
			OP_Bnnn();
			break;
		case 0xC000:
			OP_Cxkk();
			break;
		case 0xD000:
			OP_Dxyn();
			break;

		case 0xE000:
			switch(low(opcode))
			{
				case 0x9E:
					OP_Ex9E();
					break;
				case 0xA1:
					OP_ExA1();
					break;
			}

		case 0xF000:
			switch(low(opcode))
			{
				case 0x07:
					OP_Fx07();
					break;
				case 0x15:
					OP_Fx15();
					break;
				case 0x18:
					OP_Fx18();
					break;
				case 0x1E:
					OP_Fx1E();
					break;
				case 0x0A:
					OP_Fx0A();
					break;
				case 0x29:
					OP_Fx29();
					break;
				case 0x33:
					OP_Fx33();
					break;
				case 0x55:
					OP_Fx55();
					break;
				case 0x65:
					OP_Fx65();
					break;
			}
			
		default:
			std::cout << "unknown opcode";
	}

	//decrement delay timer if set
	if(delayTimer > 0)
	{
		delayTimer--;
	}
	//decrement sound timer if set
	if(soundTimer > 0)
	{
		soundTimer--;
	}
}

//takes in file path and stores opcodes into memory
void cpu::loadROM(std::string filepath)
{
	std::cout << "starting load " << filepath << "\n";
	
	init();
	
	//open the file as stream of binary and move file pointer to end
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);

	if(file.is_open())
	{
		//get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		//fill the buffer with file
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		//loop thru buffer (ROM content) and load into memory, starting from 0x200
		//memory from 0x000 to 0x1FF reserved
		for(long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}
		//delete buffer
		delete[] buffer;
	}
	else{std::cout << "file not open";}
	
}

//opcode instructions
//CLS: clear display
void cpu::OP_00E0()
{
	//set all pixels (video array) to 0
	memset(video, 0, sizeof(video));
	std::cout << "Testing OP_00E0";
}

//RET: return from subroutine
void cpu::OP_00EE()
{
	//move Stack Pointer back and point PC to the address
	--stackPointer;
	programCounter = stack[stackPointer];
	std::cout << "Testing OP_00EE";
}

//JP addr: jump to location nnn
void cpu::OP_1nnn()
{
	//AND sets address to 0nnn
	uint16_t address = opcode & 0x0FFFu;
	//set pc to address
	programCounter = address;
	std::cout << "Testing OP_1nnn";
}

//CALL addr: Call subroutine at nnn
void cpu::OP_2nnn()
{
	uint16_t address = opcode & 0x0FFFu;

	//pc currently holds the next instruction
	//we want set stack at sp to the next instruction
	stack[stackPointer] = programCounter;
	//move sp forward
	++stackPointer;
	//set pc to address
	programCounter = address;
	std::cout << "Testing OP_2nnn";
}

//SE Vx, byte: skip next instruction if Vx = kk
void cpu::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if(registers[Vx] == byte)
	{
		//skip next instruction
		programCounter += 2;
	}
	std::cout << "Testing OP_3xkk";
}

//SNE Vx, byte: skip next instruction if Vx != kk
void cpu::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if(registers[Vx] != byte)
	{
		//skip next instruction
		programCounter += 2;
	}
	std::cout << "Testing OP_4xkk";
}

//SE Vx, Vy: skip next instruction if Vx = Vy
void cpu::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if(registers[Vx] == registers[Vy])
	{
		programCounter += 2;
	}
	std::cout << "Testing OP_5xy0";
}

//LD Vx, byte: set Vx = kk
void cpu::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] = byte;
	std::cout << "Testing OP_6xkk";
}

//ADD Vx, byte: set Vx = Vx + kk
void cpu::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] += byte;
	std::cout << "Testing OP_7xkk";
}

//LD Vx, Vy: set Vx = Vy
void cpu::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vy];
	std::cout << "Testing OP_8xy0";
}

//OR Vx, Vy: set Vx = Vx OR Vy
void cpu::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] |= registers[Vy];
	std::cout << "Testing OP_8xy1";
}

//AND Vx, Vy: set Vx = Vx AND Vy
void cpu::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] &= registers[Vy];
	std::cout << "Testing OP_8xy2";
}

//XOR Vx, Vy: set Vx = Vx XOR Vy
void cpu::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] ^= registers[Vy];
	std::cout << "Testing OP_8xy3";
}

//ADD Vx, Vy: set Vx = Vx + Vy, set VF = carry
//if result of Vx + Vy is > 255, set VF to 1 for overflow flag
//LD Vx, Vy: set Vx = Vy
void cpu::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	//sum of Vx and Vy
	uint16_t sum = registers[Vx] + registers[Vy];

	//set flag if sum < 255
	if(sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = sum & 0xFFu;
	std::cout << "Testing OP_8xy4";
}

//SUB Vx, Vy: set Vx = Vx - Vy, set VF NOT borrow
//if Vx > Vy then VF = 1, else 0
void cpu::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if(registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] -= registers[Vy];
	std::cout << "Testing OP_8xy5";
}

//SHR Vx: set Vx = Vx SHR 1
//if least significant bit of Vx is 1, VF set to 1, else 0
//then Vx is divided by 2
void cpu::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	//save last digit in VF
	registers[0xF] = (registers[Vx] & 0x1u);
	//right shift on Vx
	registers[Vx] >>= 1;
	std::cout << "Testing OP_8xy6";
}

//SUBN Vx, Vy: set Vx = Vy - Vx, set VF = NOT borrow
//same as 8xy5 but Vy - Vx
void cpu::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = registers[Vy] - registers[Vx];
	std::cout << "Testing OP_8xy7";
}

//SHL Vx {, Vy}: set Vx = Vx SHL 1
//left shift on Vx, first digit saved on VF
void cpu::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
	registers[Vx] <<= 1;
	std::cout << "Testing OP_8xyE";
}

//SNE Vx, Vy: skip next instruction if Vx != Vy
void cpu::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] != registers[Vy])
	{
		programCounter += 2;
	}
	std::cout << "Testing OP_9xy0";
}

//LD I, addr: set I = nnn
void cpu::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;
	index = address;
	std::cout << "Testing OP_Annn";
}

//JP V0, addr: jump to nnn + V0
void cpu::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;
	programCounter = registers[0] + address;
	std::cout << "Testing OP_Bnnn";
}

//RND Vx, byte: set Vx to random byte AND kk
void cpu::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = rand() & byte;
	std::cout << "Testing OP_Cxkk";
}

//DRW Vx, Vy, nibble: display sprite of height n (8 width)
//at memory location, index, at (Vx, Vy), VF = 1 if collision
void cpu::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	//wrap if going beyond boundaries
	uint8_t xPos = registers[Vx] % 64;
	uint8_t yPos = registers[Vy] % 32;
	//initialize VF to 0
	registers[0xF] = 0;

	//for every row of sprite (height)
	for(unsigned int row = 0; row < height; ++row)
	{
		//grab that row from memory at I + n
		uint8_t spriteByte = memory[index + row];

		//for each column (set at 8 pixels)
		for(unsigned int col = 0; col < 8; ++col)
		{
			//80 >> col cycles through all 8 digits of spriteByte
			//assign appropriate value to each pixel based on column
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			//screen pixel at that row and column
			uint32_t* screenPixel = &video[(yPos + row) * 64 + (xPos + col)];

			if(spritePixel)
			{
				//if both spritePixel and screenPixel are on
				if(*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}
				//XOR wiyh spritePixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
	std::cout << "Testing OP_Dxyn";
}

//SKP Vx: skip next instruction if key with value Vx is pressed
void cpu::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0xF00u) >> 8u;
	uint8_t key = registers[Vx];
	if(keypad[key])
	{
		programCounter += 2;
	}
	std::cout << "Testing OP_Ex9E";
}

//SKNP Vx: skip next instruction if key with value Vx NOT pressed
void cpu::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		programCounter += 2;
	}
	std::cout << "Testing OP_ExA1";
}

//LD Vx, DT: set Vx = delay timer
void cpu::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
	std::cout << "Testing OP_Fx07";
}

//LD Vx, K: wait for key press and store in Vx
//"easiest way to 'wait' is to deprement PC by 2 whenever key not detected"
//this keeps the program in a loop until key detected
void cpu::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		programCounter -= 2;
	}
	std::cout << "Testing OP_Fx0A";
}

//LD DT, Vx: set delayTimer = Vx
void cpu::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delayTimer = registers[Vx];
	std::cout << "Testing OP_Fx15";
}

//LD ST, Vx: set sound timer = Vx
void cpu::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
	std::cout << "Testing OP_Fx18";
}

//ADD I, Vx: set I = I + Vx
void cpu::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
	std::cout << "Testing OP_Fx1E";
}

//LD F, Vx: set I = location of sprite for digit Vx
void cpu::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];
	index = FONTSET_START_ADDRESS + (5 * digit);
	std::cout << "Testing OP_Fx29";
}

//LD B, Vx: store BCD representation of Vx in memory I, I+1, I+2
//takes 3 digit decimal value of Vx
void cpu::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;
	std::cout << "Testing OP_Fx33";
}

//LD [I], Vx: store registers V0 through Vx in memory starting at I
void cpu::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for(uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
	std::cout << "Testing OP_Fx55";
}

//LD Vx, [I]: read registers V0 through Vx from memory starting at I
void cpu::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
	std::cout << "Testing OP_Fx65";
}
