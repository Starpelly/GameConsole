#pragma once

struct spng_ctx;

namespace Soulcast
{
	struct Image
	{
		Image();
		~Image();

		void Load(const char* fileName);
		void Dispose();

		int32 width = 0;
		int32 height = 0;
		PaletteEntry palette[PALETTE_BANK_SIZE]{};
		uint8* pixels = nullptr;
		size_t bpp = 0;
		size_t pitch = 0;

	private:
		spng_ctx* spngctx = nullptr;
		bool disposed = false;
	};

	/// A "sprite" is a rectangular region of the global texture page.
	struct Sprite
	{
		int32 x = 0;
		int32 y = 0;
		int32 w = 0;
		int32 h = 0;

		Image* image = nullptr;
	};
}