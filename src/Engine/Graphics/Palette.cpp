#include "Engine/Core/SoulcastEngine.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace Soulcast;

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

PaletteBank Palette::LoadPaletteBank(const char* filePath)
{
	auto parsedColors = LoadJASCPalette(filePath);

    PaletteEntry colors[PALETTE_BANK_SIZE]{};
	for (int32 i = 0; i < PALETTE_BANK_SIZE; i++)
	{
        if (i >= parsedColors.size())
            colors[i] = PaletteEntry(0, 0, 0);
        else
		    colors[i] = parsedColors[i];
	}
	return PaletteBank(colors);
}
