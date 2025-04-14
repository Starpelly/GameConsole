#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace Soulcast
{

enum class OpCode
{
	LOAD,
	ADD,
	RET
};

struct Instruction
{
	OpCode op;
	std::string arg1;
	std::string arg2;
};

class CPU
{
private:
	std::unordered_map<std::string, int> registers;
	int returnValue = 0;

public:
	CPU();

	void Run(const std::vector<Instruction>& program);

	int GetReturnValue() const
	{
		return returnValue;
	}
};
}