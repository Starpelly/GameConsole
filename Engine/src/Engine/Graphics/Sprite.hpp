#pragma once

struct spng_ctx;

namespace Soulcast
{
	struct Bitmap
	{
		Bitmap();
		~Bitmap();

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

	struct BitmapRegion
	{
		int32 x = 0;
		int32 y = 0;
		int32 w = 0;
		int32 h = 0;

		Bitmap* bitmap = nullptr;
	};

	/// A "sprite" is a 2d graphic drawn onto the screen.
	struct Sprite
	{
		int32 x = 0;
		int32 y = 0;
		int32 tileIndex = 0;
		int32 palette = 0;
		int32 rotation = 0;
		int32 scaleX = 0;
		int32 scaleY = 0;
		bool hFlip = false;
		bool vFlip = false;
		bool visible = false;
	};
}