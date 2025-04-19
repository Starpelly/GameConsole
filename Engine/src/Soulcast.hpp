#pragma once

// =============
// Standard Libs
// =============
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <filesystem>

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

#if defined(__clang__)
	#define COMPILER_CLANG
#elif defined(__GNUC__)
	#define COMPILER_GCC
#elif defined(_MSC_VER)
	#define COMPILER_MICROSOFT

	#pragma warning(disable:4996) // libc "deprecation" warnings
#else
	#warning "unable to detect compiler"
	#define COMPILER_UNKNOWN
#endif

		/*
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
	#define noinline   __attribute__((noinline))
	#define alwaysinline  inline __attribute__((always_inline))
#elif defined(COMPILER_MICROSOFT)
	#define noinline   __declspec(noinline)
	#define alwaysinline  inline __forceinline
#else
	#define noinline
	#define alwaysinline  inline
#endif
*/

#define SOULCAST_WIN	 (0)
#define SOULCAST_LINUX	 (1)

#if defined(_WIN32)
	#define SOULCAST_PLATFORM (SOULCAST_WIN)
#elif defined(linux) || defined(__linux__)
	#define SOULCAST_PLATFORM (SOULCAST_LINUX)
#else
	#warning "unable to detect platform"
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

// =======================
// Commonly used STL types
// =======================
using std::string;
using std::string_view;

#ifdef SOULCAST_DEBUG
	#if defined(SOULCAST_PLATFORM_WINDOWS)
		#define SOULCAST_DEBUGBREAK() __debugbreak()
	#else
		#error "Platform doesn't support debugbreak yet!"
#endif
	#define SOULCAST_ENABLE_ASSERTS
#else
	#define SOULCAST_DEBUGBREAK()
#endif

#define SOULCAST_EXPAND_MACRO(x) x
#define SOULCAST_STRINGIFY_MACRO(x) #x

#define SOULCAST_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Functional, for App Callbacks
#ifndef SOULCAST_NO_FUNCTIONAL
#include <functional>
namespace Soulcast
{
	template<class Ret, class...Args> using Func = std::function<Ret(Args...)>;
}
#else
namespace Soulcast
{
	template<class Ret, class...Args> using Func = Ret(*)(Args...);
}
#endif

namespace Soulcast
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#define DeleteAndNullify(x) { delete x; x = nullptr; }

// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized with { }
// This is called aggregate initialization (C++11 feature)
#if defined(__cplusplus)
#define CLITERAL(type)      type
#else
#define CLITERAL(type)      (type)
#endif

// ===
// API
// ===

#define SOULCAST_API_FUNCTION(...)