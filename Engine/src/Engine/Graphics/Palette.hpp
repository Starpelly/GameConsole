#pragma once

namespace Soulcast
{
	#define PALETTE_BANK_COUNT	(8)
	#define PALETTE_BANK_SIZE	(256)

	#define PACK_RGBA8888(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)
	#define RGB888_TO_RGB565(r, g, b)  ((b) >> 3) | (((g) >> 2) << 5) | (((r) >> 3) << 11)

	struct PaletteEntry
	{
		uint8 r;
		uint8 g;
		uint8 b;

		PaletteEntry() = default;
		PaletteEntry(uint8 r, uint8 g, uint8 b)
			: r(r), g(g), b(b)
		{
		}

		inline uint16 Packed() const
		{
			return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
		}
	};

	using PaletteBank = PaletteEntry[PALETTE_BANK_SIZE];

	extern PaletteEntry fullPalette[PALETTE_BANK_COUNT][PALETTE_BANK_SIZE];
	extern PaletteEntry* activePalette;

	namespace Palette
	{
		void LoadPaletteBank(uint8 bank, const char* filePath);

		void SetActivePalette(uint8 bank);

		void RotatePalette(uint8 bank, uint8 startIndex, uint8 endIndex, bool right);
		void RotatePaletteRel(uint8 bank, uint8 startIndex, uint8 count, bool right);
		void SetPaletteColor(uint8 bank, uint8 index, PaletteEntry color);

		void CopyPalette(uint8 src, uint8 dest);
	}
}