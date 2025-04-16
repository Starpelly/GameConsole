#pragma once

// =============
// Standard Libs
// =============
#include <stdio.h>
#include <string>
#include <cmath>

// ==============
// Standard Types
// ==============
#include <cstdint>

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

// =========
// Platforms
// =========
#define SOULCAST_WIN	(0)
#define SOULCAST_LINUX	(1)

#if defined _WIN32
	#define SOULCAST_PLATFORM (SOULCAST_WIN)
#elif defined __linux__
	#define SOULCAST_PLATFORM (SOULCAST_LINUX)
#endif

#if SOULCAST_PLATFORM == SOULCAST_WIN
	#if SOULCAST_BUILD_DLL
		#define SOULCAST_API __declspec(dllexport)
	#else
		#define SOULCAST_API __declspec(dllimport)
	#endif
#else
	#define SOULCAST_API
#endif