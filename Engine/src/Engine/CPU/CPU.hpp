#pragma once

#include <array>

namespace Soulcast
{
	class Bus
	{
	public:
		Bus();
		~Bus();

	public:
		std::array<uint8, 64 * 1024> ram;

	public:
		void write(uint16 addr, uint8 data);
		uint8 read(uint16 addr, bool readOnly);
	};

	class CPU
	{
	public:
		CPU();
		~CPU();

		void clock();

	private:
		Bus* bus = nullptr;
	};
}