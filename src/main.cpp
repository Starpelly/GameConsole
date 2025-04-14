#include "Engine/Core/SoulcastEngine.hpp"
#include "Engine/Core/CPU.hpp"

using namespace Soulcast;

#if SOULCAST_PLATFORM == SOULCAST_WIN && _MSC_VER
	#include "Windows.h"
#endif

static std::vector<Instruction> parseProgram(const std::string& source)
{
    std::vector<Instruction> program;
    std::istringstream iss(source);
    std::string line;

    while (std::getline(iss, line))
    {
        std::istringstream linestream(line);
        std::string opcode, arg1, arg2;
        linestream >> opcode >> arg1;

        // Remove commas
        if (!arg1.empty() && arg1.back() == ',') arg1.pop_back();
        linestream >> arg2;

        // Convert to uppercase opcode
        if (opcode == "LOAD")
            program.push_back({ OpCode::LOAD, arg1, arg2 });
        else if (opcode == "ADD")
            program.push_back({ OpCode::ADD, arg1, arg2 });
        else if (opcode == "RET")
            program.push_back({ OpCode::RET, arg1, "" });
    }

    return program;
}

int main(int argc, char* argv[])
{
    std::string programText = R"(
        LOAD R0, 5
        LOAD R1, 5
        ADD R0, R1
        RET R0
    )";

    auto program = parseProgram(programText);
    CPU cpu;
    cpu.Run(program);
    std::cout << "Returned: " << cpu.GetReturnValue() << std::endl;

	Soulcast::Engine.Init();
	Soulcast::Engine.Run();
	Soulcast::Engine.Shutdown();

	return 0;
}