#ifndef disassembler_h
#define disassembler_h

#include "util.h"

void instruction(uint16_t opcode, cpu *cpu)
{
	switch(first(opcode))
	{
		case 0x0000:
			if(opcode == 0x00E0)
			{
				cpu->OP_00E0();
			}
			if(opcode == 0x00EE)
			{
				cpu->OP_00EE();
			}
			break;

		case 0x1000:
			cpu->OP_1nnn();
			break;
		case 0x2000:
			cpu->OP_2nnn();
			break;
		case 0x3000:
			cpu->OP_3xkk();
			break;
		case 0x4000:
			cpu->OP_4xkk();
			break;
		case 0x5000:
			cpu->OP_5xy0();
			break;
		case 0x6000:
			cpu->OP_6xkk();
			break;
		case 0x7000:
			cpu->OP_7xkk();
			break;

		case 0x8000:
			std::cout << "\n 8000 REACHED \n";
			std::cout << "VAL: " << std::hex << last(opcode);
			std::cout << "\n";
			switch(last(opcode))
			{
				case 0x0:
					cpu->OP_8xy0();
					break;
				case 0x1:
					std::cout << "\n 1 \n";
					cpu->OP_8xy1();
					break;
				case 0x2:
					cpu->OP_8xy2();
					break;
				case 0x3:
					cpu->OP_8xy3();
					break;
				case 0x4:
					cpu->OP_8xy4();
					break;
				case 0x5:
					cpu->OP_8xy5();
					break;
				case 0x6:
					cpu->OP_8xy6();
					break;
				case 0x7:
					cpu->OP_8xy7();
					break;
				case 0xE:
					cpu->OP_8xyE();
					break;
			}

		case 0xA000:
			cpu->OP_Annn();
			break;
		case 0xB000:
			cpu->OP_Bnnn();
			break;
		case 0xC000:
			cpu->OP_Cxkk();
			break;
		case 0xD000:
			cpu->OP_Dxyn();
			break;

		case 0xE000:
			switch(low(opcode))
			{
				case 0x9E:
					cpu->OP_Ex9E();
					break;
				case 0xA1:
					cpu->OP_ExA1();
					break;
			}

		case 0xF000:
			switch(low(opcode))
			{
				case 0x07:
					cpu->OP_Fx07();
					break;
				case 0x15:
					cpu->OP_Fx15();
					break;
				case 0x18:
					cpu->OP_Fx18();
					break;
				case 0x1E:
					cpu->OP_Fx1E();
					break;
				case 0x0A:
					cpu->OP_Fx0A();
					break;
				case 0x29:
					cpu->OP_Fx29();
					break;
				case 0x33:
					cpu->OP_Fx33();
					break;
				case 0x55:
					cpu->OP_Fx55();
					break;
				case 0x65:
					cpu->OP_Fx65();
					break;
			}
			
		default:
			std::cout << "unknown opcode";
	}
}

#endif /* disassembler_h */
