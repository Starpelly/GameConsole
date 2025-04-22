#include "Engine/Core/Engine.hpp"

using namespace Soulcast;

// ==============================================
// Bus
// ==============================================
Bus::Bus()
{
	for (auto& i : ram) i = 0x00;
}

Bus::~Bus()
{
}

void Bus::write(uint16 addr, uint8 data)
{
	if (addr >= 0x0000 && addr <= 0xFFFF)
		ram[addr] = data;
}

uint8 Bus::read(uint16 addr, bool readOnly)
{
	if (addr >= 0x0000 && addr <= 0xFFFF)
		return ram[addr];

	return 0x00;
}

// ==============================================
// CPU
// ==============================================

CPU::CPU()
{
}

CPU::~CPU()
{
}

void CPU::clock()
{

}