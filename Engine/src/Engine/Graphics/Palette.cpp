#include "Engine/Core/SoulcastEngine.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace Soulcast;

namespace Soulcast
{
    PaletteBank fullPalette;
    PaletteEntry* activePalette = fullPalette;
}

static std::vector<PaletteEntry> LoadJASCPalette(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    std::string line;

    // Header
    std::getline(file, line);
    if (line != "JASC-PAL")
        throw std::runtime_error("Invalid JASC palette header");

    // Version
    std::getline(file, line);
    if (line != "0100")
        throw std::runtime_error("Unsupported JASC palette version");

    // Number of colors
    std::getline(file, line);
    int colorCount = std::stoi(line);

    std::vector<PaletteEntry> palette;
    palette.reserve(colorCount);

    for (int i = 0; i < colorCount; ++i)
    {
        if (!std::getline(file, line))
            throw std::runtime_error("Unexpected end of file in palette data");

        std::istringstream iss(line);
        int r, g, b;
        if (!(iss >> r >> g >> b))
            throw std::runtime_error("Invalid color format on line " + std::to_string(i + 4));

        palette.emplace_back(static_cast<uint8_t>(r),
            static_cast<uint8_t>(g),
            static_cast<uint8_t>(b));
    }

    return palette;
}

void Palette::LoadPaletteBank(PaletteEntry* out, const char* filePath)
{
	auto parsedColors = LoadJASCPalette(filePath);

	for (int32 i = 0; i < PALETTE_BANK_SIZE; i++)
	{
        if (i >= parsedColors.size())
            out[i] = PaletteEntry(0, 0, 0);
        else
		    out[i] = parsedColors[i];
	}
}

void Palette::RotatePalette(uint8 startIndex, uint8 endIndex, bool right)
{
    if (right)
    {
        auto startColor = activePalette[endIndex];
        for (int i = endIndex; i > startIndex; --i)
        {
            activePalette[i] = activePalette[i - 1];
        }
        activePalette[startIndex] = startColor;
    }
    else
    {
        auto startColor = activePalette[startIndex];
        for (int i = startIndex; i < endIndex; ++i)
        {
            activePalette[i] = activePalette[i + 1];
        }
        activePalette[endIndex] = startColor;
    }
}

void Palette::RotatePaletteRel(uint8 startIndex, uint8 count, bool right)
{
    RotatePalette(startIndex, startIndex + count - 1, right);
}

void Palette::SetPaletteColor(uint8 index, PaletteEntry color)
{
    activePalette[index] = color;
}

void Palette::CopyPalette(uint8 src, uint8 dest)
{
    if (src < PALETTE_BANK_SIZE && dest < PALETTE_BANK_SIZE)
    {
    }
}
