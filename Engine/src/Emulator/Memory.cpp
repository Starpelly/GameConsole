#include "Soulcast.hpp"
#include "Emulator/Memory.hpp"

using namespace Soulcast;

namespace Soulcast::Memory
{
	byte memory[MEMORY_SIZE];
}

byte Memory::Peek(uint32 addr, uint32 n)
{
	if (addr < MEMORY_SIZE)
	{
		return memory[addr];
	}

	// Throw or return default value?
	// Probably throw...
	return 0;
}

void Memory::Poke(uint32 addr, byte value)
{
	if (addr < MEMORY_SIZE)
	{
		memory[addr] = value;
	}
}