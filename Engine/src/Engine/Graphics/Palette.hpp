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

	using PaletteBank = PaletteEntry[PALETTE_BANK_SIZE];

	extern PaletteEntry* activePalette;

	namespace Palette
	{
		void LoadPaletteBank(PaletteEntry* out, const char* filePath);

		void RotatePalette(uint8 startIndex, uint8 endIndex, bool right);
		void RotatePaletteRel(uint8 startIndex, uint8 count, bool right);
		void SetPaletteColor(uint8 index, uint32 color);
		void CopyPalette(uint8 src, uint8 dest);
	}
}