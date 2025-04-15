#pragma once

namespace Soulcast
{
	#define PALETTE_BANK_COUNT	(8)
	#define PALETTE_BANK_SIZE	(256)

	#define RGB888_TO_RGB565(r, g, b)  ((b) >> 3) | (((g) >> 2) << 5) | (((r) >> 3) << 11)

	struct PaletteEntry
	{
		uint8 r;
		uint8 g;
		uint8 b;

		PaletteEntry() = default;
		PaletteEntry(uint8 r, uint8 g, uint8 b)
			: r(r), g(g), b(b) { }
	};

	struct PaletteBank
	{
		PaletteEntry Colors[PALETTE_BANK_SIZE];

		PaletteBank() = default;

		inline PaletteBank(PaletteEntry colors[PALETTE_BANK_SIZE])
		{
			for (int32 i = 0; i < PALETTE_BANK_SIZE; i++)
			{
				Colors[i] = colors[i];
			}
		}
	};

	namespace Palette
	{
		PaletteBank LoadPaletteBank(const char* filePath);
	}
}