#pragma once

#include <vector>
#include <array>

#include <Midifile.h>
using namespace smf;

namespace Soulcast
{
	#define SFX_COUNT				(0x100)
    // #define CHANNEL_COUNT		(0x10)

	#define AUDIO_SAMPLERATE		(44100)
	#define AUDIO_CHANNELS			(4)

	#define PCM_CHANNEL_SNAPSHOTS	(32)

	#define OCTAVE_BASE_FREQUENCY	(110.0)

namespace Audio
{
	using ChannelType = uint16;

	namespace Channel
	{
		enum : ChannelType
		{
			Pulse0,
			Pulse1,
			PCM,
			Noise,
			Count
		};
	}

    static const std::array<const char*, Channel::Count> ChannelTypeNames = {
        "Pulse 1",
        "Pulse 2",
        "PCM",
        "Noise"
    };

	enum class AudioInterpolation
	{
		None,		// Nearest-neighbor
		Gaussian,
		Hermite
	};

	struct PCMChannel
	{
		int16 data[PCM_CHANNEL_SNAPSHOTS] = {};
		bool empty = true;
		float sampleRate = AUDIO_SAMPLERATE;
	};

	struct BaseChannel
	{
		ChannelType type = Channel::Pulse0;
		bool active = false;
		double frequency = 440.0;
		double phase = 0.0;
		double pan = 0.0f; // -1 = left, 0 = center, +1 = right

		PCMChannel pcm; // Only used if type == Channel::PCM
	};

	SOULCAST_API PCMChannel Load4BitPCMFile(const char* filename);

	inline double MidiNoteToFreq(int midiNote)
	{
		return 440.0 * pow(2.0, (midiNote - 69) / 12.0);
	}
}

struct SoundChip
{
	std::array<Audio::BaseChannel, Audio::Channel::Count> channels;

	SoundChip() = default;
};

class SOULCAST_API AudioDevice
{
public:
	static void Init();
	static void Release();

	static void GenerateAudio(float* stream, int numSamples, int sampleRate);

	static void SetChannelFrequency(int channelIndex, double frequency);
	static void SetChannelActive(int channelIndex, bool active);

#if SOULCAST_USING_SDL3
	static SDL_AudioDeviceID streamDeviceID;
	static SDL_AudioStream* audioStream;
#endif

	static SoundChip soundChip;
};

}
