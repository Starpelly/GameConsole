#pragma once

#include "Engine/Core/Engine.hpp"

namespace Soulcast
{
	namespace Memory
	{
		//----------------------------------------
		// Memory Configuration
		//----------------------------------------

		using byte = uint8;
		constexpr size_t MEMORY_SIZE = 8 * 1024 * 1024; // 8 MiB total memory
		extern byte memory[MEMORY_SIZE];

		// Helper macro to define memory region start based on previous region's end
		#define GET_START(PREVIOUS_END) (PREVIOUS_END)

		//----------------------------------------
		// Framebuffer Region
		//----------------------------------------

		constexpr size_t FRAMEBUFFER_START = 0x0000;
		constexpr size_t FRAMEBUFFER_SIZE  = SCREEN_XSIZE * SCREEN_YSIZE * sizeof(uint16);
		constexpr size_t FRAMEBUFFER_END   = FRAMEBUFFER_START + FRAMEBUFFER_SIZE;

		//----------------------------------------
		// Audio Memory Region
		//----------------------------------------

		constexpr size_t AUDIO_START = GET_START(FRAMEBUFFER_END);
		constexpr size_t AUDIO_SIZE  = 512 * 1024; // 512 KiB
		constexpr size_t AUDIO_END   = AUDIO_START + AUDIO_SIZE;

		//----------------------------------------
		// Sprite (OAM) Region
		//----------------------------------------

		constexpr size_t MAX_SPRITES   = 128;
		constexpr size_t SPRITES_START = GET_START(AUDIO_END);
		constexpr size_t SPRITES_SIZE  = MAX_SPRITES * sizeof(Sprite);
		constexpr size_t SPRITES_END   = SPRITES_START + SPRITES_SIZE;

		//----------------------------------------
		// Memory Access API
		//----------------------------------------

		byte Peek(uint32 addr, uint32 n = 1);
		void Poke(uint32 addr, byte value);

	}
}