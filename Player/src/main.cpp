#include "Engine/Core/Engine.hpp"

int main(int argc, char* argv[])
{
	Soulcast::Engine.Init();
	Soulcast::Engine.Run();
	Soulcast::Engine.Release();

	return 0;
}