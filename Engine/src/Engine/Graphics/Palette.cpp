#include "Engine/Core/Engine.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace Soulcast;

namespace Soulcast
{
    PaletteEntry fullPalette[PALETTE_BANK_COUNT][PALETTE_BANK_SIZE];
    PaletteEntry* activePalette = fullPalette[0];
    uint8 activePaletteBank = 0;
}

static std::vector<PaletteEntry> Palette::LoadJASCPalette(const std::string& filename)
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

void Palette::LoadPaletteBank(uint8 bank, const char* filePath)
{
	auto parsedColors = LoadJASCPalette(filePath);

	for (int32 i = 0; i < PALETTE_BANK_SIZE; i++)
	{
        if (i >= parsedColors.size())
            fullPalette[bank][i] = PaletteEntry(0, 0, 0);
        else
            fullPalette[bank][i] = parsedColors[i];
	}
}

void Palette::SetActivePalette(uint8 bank)
{
    activePaletteBank = bank;
    activePalette = fullPalette[bank];
}

void Palette::RotatePalette(uint8 bank, uint8 startIndex, uint8 endIndex, bool right)
{
    if (right)
    {
        auto startColor = fullPalette[bank][endIndex];
        for (int i = endIndex; i > startIndex; --i)
        {
            fullPalette[bank][i] = fullPalette[bank][i - 1];
        }
        fullPalette[bank][startIndex] = startColor;
    }
    else
    {
        auto startColor = fullPalette[bank][startIndex];
        for (int i = startIndex; i < endIndex; ++i)
        {
            fullPalette[bank][i] = fullPalette[bank][i + 1];
        }
        fullPalette[bank][endIndex] = startColor;
    }
}

void Palette::RotatePaletteRel(uint8 bank, uint8 startIndex, uint8 count, bool right)
{
    RotatePalette(bank, startIndex, startIndex + count - 1, right);
}

void Palette::SetPaletteColor(uint8 bank, uint8 index, PaletteEntry color)
{
    fullPalette[bank][index] = color;
}

void Palette::CopyPalette(uint8 src, uint8 dest)
{
    if (src < PALETTE_BANK_SIZE && dest < PALETTE_BANK_SIZE)
    {
    }
}
