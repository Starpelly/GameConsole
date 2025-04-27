#pragma once

#include <filesystem>

#ifdef SOULCAST_ENABLE_ASSERTS
	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define SOULCAST_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SOULCAST##type##ERROR(msg, __VA_ARGS__); SOULCAST_DEBUGBREAK(); } }
	#define SOULCAST_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SOULCAST_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define SOULCAST_INTERNAL_ASSERT_NO_MSG(type, check) SOULCAST_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SOULCAST_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define SOULCAST_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define SOULCAST_INTERNAL_ASSERT_GET_MACRO(...) SOULCAST_EXPAND_MACRO( SOULCAST_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SOULCAST_INTERNAL_ASSERT_WITH_MSG, SOULCAST_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define SOULCAST_ASSERT(...) SOULCAST_EXPAND_MACRO( SOULCAST_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define SOULCAST_CORE_ASSERT(...) SOULCAST_EXPAND_MACRO( SOULCAST_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )

	#define SOULCAST_ASSERT_RUNNING() SOULCAST_CORE_ASSERT(Engine.running)
#else
	#define SOULCAST_ASSERT(...)
	#define SOULCAST_CORE_ASSERT(...)

	#define SOULCAST_ASSERT_RUNNING() SOULCAST_ASSERT(Engine.initialized)
#endif