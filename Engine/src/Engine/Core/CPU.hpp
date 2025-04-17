#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace Soulcast
{
	#define CPU_STACK_MAX_SIZE 128

	enum class OpCode
	{
		LOAD,
		ADD,
		RET
	};

	enum class SystemCalls
	{
		Reset = 0x0000,
		Clear,
		Flip
	};

	enum Registers
	{
		R0 = 0,
		R1,
		R2,
		R3,
		R4,
		R5,
		R6,
		R7,
		RCount
	};

	struct Instruction
	{
		OpCode op;
		std::string arg1;
		std::string arg2;
	};

	struct Memory
	{
		Memory(uint64 size);
		~Memory();

		uint64 Read(uint64 address) const;
		void Write(uint64 address, uint64 value);

		uint64* data = nullptr;
		uint64 size = 0;
	};

	class Stack
	{
	public:
		void Push(uint64 element);
		uint64 Pop();
		
		inline bool Empty() const
		{
			return top < 0;
		}

		inline bool Full() const
		{
			return top >= CPU_STACK_MAX_SIZE - 1;
		}

		int8 top;
		uint64 elements[CPU_STACK_MAX_SIZE];
	};

	class CPU
	{
	public:
		CPU();
		~CPU();

		void Tick();
		uint64 Fetch();

		Memory* ram = nullptr;
		uint64 registers[RCount];
		Stack callStack;
		Stack stack;
		uint64 pc;
	};
}