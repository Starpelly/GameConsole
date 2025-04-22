#pragma once

namespace Soulcast
{
	namespace Memory
	{
		constexpr size_t MEMORY_SIZE = 256 * 1024;	// 256KiB memory
		extern byte memory[MEMORY_SIZE];

		constexpr size_t FRAMEBUFFER_START = 0x0000;
		constexpr size_t FRAMEBUFFER_SIZE = SCREEN_XSIZE * SCREEN_YSIZE * sizeof(uint16);
		constexpr size_t FRAMEBUFFER_END = FRAMEBUFFER_START + FRAMEBUFFER_SIZE;

		// constexpr size_t PALETTE_START = FRAMEBUFFER_START + FRAMEBUFFER_SIZE;
		// constexpr size_t PALETTE_SIZE = PALETTE_BANK_SIZE * PALETTE_BANK_COUNT * sizeof(PaletteEntry);

		constexpr size_t AUDIO_START = FRAMEBUFFER_END;
		constexpr size_t AUDIO_SIZE = 0x1000;
		constexpr size_t AUDIO_END = AUDIO_START + AUDIO_SIZE;

		byte Peek(uint32 addr, uint32 n = 1);
		void Poke(uint32 addr, byte value);
	}
}