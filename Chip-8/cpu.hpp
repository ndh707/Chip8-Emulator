#ifndef cpu_hpp
#define cpu_hpp

#include <string>
#include <stdio.h>

class cpu
{
private:
	
public:
	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t programCounter{};
	uint16_t stack[16]{};
	uint8_t stackPointer{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[16]{};
	uint32_t video[64 * 32];
	uint16_t opcode{};
	
	//initialize/reset variables
	void init();
	
	//emulate one cycle of the CPU
	void cycle(cpu*);
	
	//takes in file path and stores opcodes into memory
	void loadROM(std::string);
	
	//opcode instructions
	
	void OP_00E0(); //CLS: clear display
	//RET: return from subroutine
	void OP_00EE();
	//JP addr: jump to location nnn
	void OP_1nnn();
	//CALL addr: Call subroutine at nnn
	void OP_2nnn();
	//SE Vx, byte: skip next instruction if Vx = kk
	void OP_3xkk();
	//SNE Vx, byte: skip next instruction if Vx != kk
	void OP_4xkk();
	//SE Vx, Vy: skip next instruction if Vx = Vy
	void OP_5xy0();
	//LD Vx, byte: set Vx = kk
	void OP_6xkk();
	//ADD Vx, byte: set Vx = Vx + kk
	void OP_7xkk();
	//LD Vx, Vy: set Vx = Vy
	void OP_8xy0();
	//OR Vx, Vy: set Vx = Vx OR Vy
	void OP_8xy1();
	//AND Vx, Vy: set Vx = Vx AND Vy
	void OP_8xy2();
	//XOR Vx, Vy: set Vx = Vx XOR Vy
	void OP_8xy3();
	//ADD Vx, Vy: set Vx = Vx + Vy, set VF = carry
	//if result of Vx + Vy is > 255, set VF to 1 for overflow flag
	//LD Vx, Vy: set Vx = Vy
	void OP_8xy4();
	//SUB Vx, Vy: set Vx = Vx - Vy, set VF NOT borrow
	//if Vx > Vy then VF = 1, else 0
	void OP_8xy5();
	//SHR Vx: set Vx = Vx SHR 1
	//if least significant bit of Vx is 1, VF set to 1, else 0
	//then Vx is divided by 2
	void OP_8xy6();
	//SUBN Vx, Vy: set Vx = Vy - Vx, set VF = NOT borrow
	//same as 8xy5 but Vy - Vx
	void OP_8xy7();
	//SHL Vx {, Vy}: set Vx = Vx SHL 1
	//left shift on Vx, first digit saved on VF
	void OP_8xyE();
	//SNE Vx, Vy: skip next instruction if Vx != Vy
	void OP_9xy0();
	//LD I, addr: set I = nnn
	void OP_Annn();
	//JP V0, addr: jump to nnn + V0
	void OP_Bnnn();
	//RND Vx, byte: set Vx to random byte AND kk
	void OP_Cxkk();
	//DRW Vx, Vy, nibble: display sprite of height n (8 width)
	//at memory location, index, at (Vx, Vy), VF = 1 if collision
	void OP_Dxyn();
	//SKP Vx: skip next instruction if key with value Vx is pressed
	void OP_Ex9E();
	//SKNP Vx: skip next instruction if key with value Vx NOT pressed
	void OP_ExA1();
	//LD Vx, DT: set Vx = delay timer
	void OP_Fx07();
	//LD Vx, K: wait for key press and store in Vx
	//"easiest way to 'wait' is to deprement PC by 2 whenever key not detected"
	//this keeps the program in a loop until key detected
	void OP_Fx0A();
	//LD DT, Vx: set delayTimer = Vx
	void OP_Fx15();
	//LD ST, Vx: set sound timer = Vx
	void OP_Fx18();
	//ADD I, Vx: set I = I + Vx
	void OP_Fx1E();
	//LD F, Vx: set I = location of sprite for digit Vx
	void OP_Fx29();
	//LD B, Vx: store BCD representation of Vx in memory I, I+1, I+2
	//takes 3 digit decimal value of Vx
	void OP_Fx33();
	//LD [I], Vx: store registers V0 through Vx in memory starting at I
	void OP_Fx55();
	//LD Vx, [I]: read registers V0 through Vx from memory starting at I
	void OP_Fx65();
};


#endif /* cpu_hpp */
