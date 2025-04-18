#pragma once

namespace Soulcast
{
	#define SFX_COUNT		(0x100)
	#define CHANNEL_COUNT	(0x10)

	#define AUDIO_FREQUENCY	(44100)
	#define AUDIO_CHANNELS	(4)

	#define PCM_CHANNEL_SNAPSHOTS (32)

	enum class WaveType
	{
		Square,
		Sine,
		PulseWave,
		Triangle,
		Noise
	};

	enum class ChannelType
	{
		Pulse,
		PCM,
		Noise
	};

	enum class AudioInterpolation
	{
		None, // Nearest-neighbor
		Gaussian,
		Hermite
	};

	struct PCMChannel
	{
		int16 data[PCM_CHANNEL_SNAPSHOTS] = {};
		bool empty = true;

		float phase = 0.0f;
		float frequency = 440.0f;
		float sampleRate = AUDIO_FREQUENCY;

		float GenerateSample()
		{
			if (empty) return 0.0f;

			// Calculate index into 32-sample table
			size_t index = static_cast<size_t>(phase) % 32;
			int16_t value = data[index];

			// Normalize 4-bit sample (0-15) to float -1.0 to +1.0
			float sample = ((value / 15.0f) * 2.0f) - 1.0f;

			// Advance phase
			phase += frequency * 32.0f / sampleRate;
			if (phase >= 32.0f) phase -= 32.0f;

			return sample;
		}
	};

	struct PulseChannel
	{
		float phase = 0.0f;
		float freq = 440.0f;
		float duty = 0.5f;
		float sampleRate = 44100.0f;

		float GenerateSample()
		{
			float sample = (fmod(phase, 1.0f) < duty) ? 0.8f : -0.8f;
			phase += freq / sampleRate;
			return sample;
		}
	};

	struct NoiseChannel
	{
		uint32_t lfsr = 0xACE1u;

		float GenerateSample()
		{
			lfsr ^= lfsr >> 7;
			lfsr ^= lfsr << 9;
			lfsr ^= lfsr >> 13;
			return ((lfsr & 1) ? 0.7f : -0.7f);
		}
	};

	struct SoundChip
	{
		PulseChannel pulse1, pulse2;
		PCMChannel pcm;
		NoiseChannel noise;

		SoundChip() = default;
	};

	class AudioDevice
	{
	public:
		static void Init();
		static void Release();

		static void SetPCMFreq(float freq);

#if SOULCAST_USING_SDL3
		static SDL_AudioDeviceID streamDeviceID;
#endif
	};

	PCMChannel load4BitPCMFile(const char* filename);
}