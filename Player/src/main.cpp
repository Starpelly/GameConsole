#include "Engine/Core/Engine.hpp"
#include "Emulator/Emulator.hpp"

int main(int argc, char* argv[])
{
	Soulcast::Engine.Init("D:/Soulcast/test/Sandbox");

	Soulcast::Emulator emu;
	emu.Init();
	emu.Run();
	emu.Release();

	Soulcast::Engine.Release();

	return 0;
}