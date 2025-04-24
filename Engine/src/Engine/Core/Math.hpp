#pragma once

namespace Soulcast
{
	#define PI (3.1415927f)

	#define INT_TO_VOID(x) (void *)(size_t)(x)
	#define VOID_TO_INT(x) (int32)(size_t)(x)

	#define TO_FIXED(x)   ((x) << 16)
	#define FROM_FIXED(x) ((x) >> 16)

	enum class Endian
	{
		Little,
		Big
	};

	namespace Math
	{
		constexpr float32 TAU = PI * 2.0f;
		constexpr float32 RIGHT = 0;
		constexpr float32 LEFT = PI;
		constexpr float32 UP = PI / -2;
		constexpr float32 DOWN = PI / 2;

		constexpr float32 mod(float32 x, float32 m) { return x - (int)(x / m) * m; }
		inline float32 sin(float32 x) { return ::sinf(x); }
		inline float32 cos(float32 x) { return ::cosf(x); }
		inline float32 tan(float32 x) { return ::tanf(x); }
		inline float32 atan2(float32 y, float32 x) { return ::atan2f(y, x); }
		inline float32 round(float32 x) { return ::roundf(x); }
		inline float32 floor(float32 x) { return ::floorf(x); }
		inline float32 ceiling(float32 x) { return ::ceilf(x); }
		inline float32 pow(float32 x, float32 n) { return ::powf(x, n); }
		inline float32 sqrt(float32 x) { return ::sqrtf(x); }

		template<class T>
		constexpr T sign(T x)
		{
			return static_cast<T>(x == 0 ? 0 : (x < 0 ? -1 : 1));
		}

		template<class T>
		constexpr T abs(T x)
		{
			return x < 0 ? -x : x;
		}

		template<class T, class TMin, class TMax>
		constexpr T clamp(T value, TMin min, TMax max)
		{
			return value < min ? static_cast<T>(min) : (value > max ? static_cast<T>(max) : value);
		}

		template<class T>
		constexpr T min(T a, T b)
		{
			return  (T)(a < b ? a : b);
		}

		template<class T, typename ... Args>
		constexpr T min(const T& a, const T& b, const Args&... args)
		{
			return Math::min(a, Math::min(b, args...));
		}

		template<class T>
		constexpr T max(T a, T b)
		{
			return (T)(a > b ? a : b);
		}

		template<class T, typename ... Args>
		constexpr T max(const T& a, const T& b, const Args&... args)
		{
			return Math::max(a, Math::max(b, args...));
		}

		template<class T, typename ... Args>
		constexpr T repeat(const T& value, const T& length)
		{
			return Math::clamp(value - (T)floor(value / length) * length, 0.0f, length);
		}

		constexpr float32 approach(float32 t, float32 target, float32 delta)
		{
			return t < target ? Math::min(t + delta, target) : Math::max(t - delta, target);
		}

		constexpr float32 map(float32 t, float32 old_min, float32 old_max, float32 new_min, float32 new_max)
		{
			return new_min + ((t - old_min) / (old_max - old_min)) * (new_max - new_min);
		}

		constexpr float32 clamped_map(float32 t, float32 old_min, float32 old_max, float32 new_min, float32 new_max)
		{
			return map(Math::clamp(t, old_min, old_max), old_min, old_max, new_min, new_max);
		}

		constexpr float32 lerp(float32 a, float32 b, float32 t)
		{
			return a + (b - a) * t;
		}

		constexpr float32 angle_diff(float32 radians_a, float32 radians_b)
		{
			return Math::mod((radians_b - radians_a) + PI, TAU) - PI;
		}

		constexpr float32 angle_lerp(float32 radians_a, float32 radians_b, float32 p)
		{
			const auto shortest_angle = Math::mod(Math::mod(radians_b - radians_a, TAU) + (TAU + PI), TAU) - PI;
			return radians_a + Math::mod(shortest_angle * p, TAU);
		}

		template<class T>
		constexpr void swap_endian(T* value)
		{
			for (int i = 0; i < sizeof(T) / 2; i++)
			{
				char* _ptr = (char*)&value;
				char _temp = *(_ptr + i);
				*(_ptr + i) = *(_ptr + sizeof(T) - i - 1);
				*(_ptr + sizeof(T) - i - 1) = _temp;
			}
		}

		inline bool is_big_endian()
		{
			return (*((short*)"AB") == 0x4243);
		}

		inline bool is_little_endian()
		{
			return (*((short*)"AB") != 0x4243);
		}

		inline bool is_endian(const Endian& endian)
		{
			return
				(endian == Endian::Little && is_little_endian()) ||
				(endian == Endian::Big && is_big_endian());
		}

        inline float round2Nearest(float a, float interval)
        {
            return a - fmod(a, interval);
        }
	}
}
