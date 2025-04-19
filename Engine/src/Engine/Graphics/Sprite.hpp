#pragma once

struct spng_ctx;

namespace Soulcast
{
	struct Image
	{
		Image(const char* fileName);
		~Image();

		int32 width = 0;
		int32 height = 0;
		PaletteEntry palette[PALETTE_BANK_SIZE];
		uint8* pixels = nullptr;
		size_t bpp = 0;
		size_t pitch = 0;

	private:
		spng_ctx* spngctx = nullptr;
	};
}