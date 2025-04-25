#include "Engine/Core/Engine.hpp"

int main(int argc, char* argv[])
{
	Soulcast::Engine.Init("D:/Soulcast/test/Sandbox");
	Soulcast::Engine.Run();
	Soulcast::Engine.Release();

	return 0;
}