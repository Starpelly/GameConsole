#include "Engine/Core/SoulcastEngine.hpp"

#include <spng/spng.h>

using namespace Soulcast;

Image::Image(const char* fileName)
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

    // Get image info
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
        fprintf(stderr, "Image is not indexed color\n");
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
        palette[i] = RGB888_TO_RGB565(entry.red, entry.green, entry.blue);
    }

    // Allocate memory for raw image data
    size_t image_size;
    spng_decoded_image_size(spngctx, SPNG_FMT_PNG, &image_size);
    if (!image_size)
    {
        fprintf(stderr, "Failed to get image size\n");
        // free(palette);
        spng_ctx_free(spngctx);
        fclose(png_file);
    }

    pixels = (uint8*)malloc(image_size);
    if (!pixels)
    {
        fprintf(stderr, "Failed to allocate memory for image\n");
        spng_ctx_free(spngctx);
        fclose(png_file);
        return;
    }

    // Decode the image
    if (spng_decode_image(spngctx, pixels, image_size, SPNG_FMT_PNG, 0))
    {
        fprintf(stderr, "Failed to decode image\n");
        // free(palette);
        free(pixels);
        spng_ctx_free(spngctx);
        fclose(png_file);
    }

    // printf("Image decoded successfully\n");
    fclose(png_file);

    width = ihdr.width;
    height = ihdr.height;

    // m_BPP = getBytesPerPixel(&ihdr);
    // m_Pitch = m_Width * m_BPP;

    // m_ColorFormat = (ColorFormat)ihdr.color_type;
}

Image::~Image()
{
    // free(palette);
    free(pixels);
    spng_ctx_free(spngctx);
}