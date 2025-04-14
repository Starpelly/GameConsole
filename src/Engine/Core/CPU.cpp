#include "CPU.hpp"

using namespace Soulcast;

CPU::CPU()
{
	registers["R0"] = 0;
	registers["R1"] = 0;
	registers["R2"] = 0;
	registers["RET"] = 0;
}

void CPU::Run(const std::vector<Instruction>& program)
{
    for (const auto& instr : program) {
        if (instr.op == OpCode::LOAD) {
            registers[instr.arg1] = std::stoi(instr.arg2);
        }
        else if (instr.op == OpCode::ADD) {
            registers[instr.arg1] += registers[instr.arg2];
        }
        else if (instr.op == OpCode::RET) {
            returnValue = registers[instr.arg1];
            registers["RET"] = returnValue;
            break;
        }
    }
}