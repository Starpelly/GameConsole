#include "SoulcastEngine.hpp"

using namespace Soulcast;

CPU::CPU()
{
	ram = new Memory(0x1000000);
	pc = 0;
}

CPU::~CPU()
{
	delete ram;
}

void CPU::Tick()
{
	uint64 i = Fetch();
	uint8 instr = (i & 0xFF00) >> 8;
	uint8 form = (i & 0x00FF);

}

uint64 CPU::Fetch()
{
	return ram->Read(pc++);
}

void Stack::Push(uint64 element)
{
	if (Full())
	{
		// ERROR: STACK OVERFLOW!
		return;
	}

	elements[++top] = element;
}

uint64 Stack::Pop()
{
	if (Empty())
	{
		// ERROR: STACK EMPTY!
		return 0;
	}

	return elements[top--];
}

Memory::Memory(uint64 size)
{
	this->data = (uint64*)malloc(size * sizeof(uint64));
	this->size = size;

	if (data != 0)
	{
		memset(this->data, 0, size * sizeof(uint64));
	}
}

Memory::~Memory()
{
	free(this->data);
	this->size = 0;
}

uint64 Memory::Read(uint64 address) const
{
	return data[address];
}

void Memory::Write(uint64 address, uint64 value)
{
	this->data[address] = value;
}