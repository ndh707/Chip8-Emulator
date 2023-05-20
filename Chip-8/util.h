#ifndef util_h
#define util_h

#include <bitset>

uint16_t first(uint16_t opcode)
{
	return (opcode & 0xF000);
}

uint16_t last(uint16_t opcode)
{
	return (opcode & 0x000F);
}


uint16_t low(uint16_t opcode)
{
	return (opcode & 0x00FF);
}

#endif /* util_h */
