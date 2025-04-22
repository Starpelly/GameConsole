#include "Engine/Core/Engine.hpp"

#include <spng/spng.h>

using namespace Soulcast;

size_t getBytesPerPixel(const spng_ihdr* ihdr)
{
    int channels;

    switch (ihdr->color_type)
    {
    case SPNG_COLOR_TYPE_GRAYSCALE:
    case SPNG_COLOR_TYPE_INDEXED:
        channels = 1; // Grayscale or Indexed (1 channel)
        break;
    case SPNG_COLOR_TYPE_TRUECOLOR:
        channels = 3; // RGB (3 channels)
        break;
    case SPNG_COLOR_TYPE_GRAYSCALE_ALPHA:
        channels = 2; // Grayscale + Alpha (2 channels)
        break;
    case SPNG_COLOR_TYPE_TRUECOLOR_ALPHA:
        channels = 4; // RGBA (4 channels)
        break;
    default:
        return 0; // Invalid color type
    }

    // Calculate bits per pixel
    int bits_per_pixel = channels * ihdr->bit_depth;

    // Convert to bytes per pixel
    return (size_t)(bits_per_pixel + 7) / 8; // Add 7 and divide to round up
}

Bitmap::Bitmap()
{
}

Bitmap::~Bitmap()
{
    Dispose();
}

void Bitmap::Load(const char* fileName)
{
    FILE* png_file = fopen(fileName, "rb");
    if (!png_file)
    {
        perror("Failed to open file");
        return;
    }

    spngctx = spng_ctx_new(0);
    if (!spngctx)
    {
        fprintf(stderr, "Failed to create spng context\n");
        fclose(png_file);
    }

    spng_set_png_file(spngctx, png_file);

    // Get bitmap info
    struct spng_ihdr ihdr;
    if (spng_get_ihdr(spngctx, &ihdr))
    {
        fprintf(stderr, "Failed to get IHDR\n");
        spng_ctx_free(spngctx);
        fclose(png_file);
        return;
    }

    // Check for indexed color type
    if (ihdr.color_type != SPNG_COLOR_TYPE_INDEXED)
    {
        fprintf(stderr, "Bitmap is not indexed color\n");
        spng_ctx_free(spngctx);
        fclose(png_file);
        return;
    }

    // Get the palette (PLTE chunk)
    struct spng_plte plte;
    if (spng_get_plte(spngctx, &plte))
    {
        fprintf(stderr, "Failed to get PLTE chunk\n");
        spng_ctx_free(spngctx);
        fclose(png_file);
        return;
    }

    // printf("Palette contains %d entries\n", plte.n_entries);

    // palette = (uint32*)malloc(plte.n_entries * sizeof(uint32));
    for (int i = 0; i < plte.n_entries; i++)
    {
        auto entry = plte.entries[i];
        palette[i] = PaletteEntry(entry.red, entry.green, entry.blue);
    }

    // Allocate memory for raw bitmap data
    size_t image_size;
    spng_decoded_image_size(spngctx, SPNG_FMT_PNG, &image_size);
    if (!image_size)
    {
        fprintf(stderr, "Failed to get bitmap size\n");
        // free(palette);
        spng_ctx_free(spngctx);
        fclose(png_file);
    }

    pixels = (uint8*)malloc(image_size);
    if (!pixels)
    {
        fprintf(stderr, "Failed to allocate memory for bitmap\n");
        spng_ctx_free(spngctx);
        fclose(png_file);
        return;
    }

    // Decode the bitmap
    if (spng_decode_image(spngctx, pixels, image_size, SPNG_FMT_PNG, 0))
    {
        fprintf(stderr, "Failed to decode bitmap\n");
        // free(palette);
        free(pixels);
        spng_ctx_free(spngctx);
        fclose(png_file);
    }

    // printf("Bitmap decoded successfully\n");
    fclose(png_file);

    width = ihdr.width;
    height = ihdr.height;

    bpp = getBytesPerPixel(&ihdr);
    pitch = width * bpp;
    disposed = false;
    // m_ColorFormat = (ColorFormat)ihdr.color_type;
}

void Bitmap::Dispose()
{
    if (disposed) return;

    // free(palette);
    free(pixels);
    spng_ctx_free(spngctx);

    disposed = true;
}
