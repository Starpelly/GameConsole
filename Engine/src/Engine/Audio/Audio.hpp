#pragma once

namespace Soulcast
{
	#define SFX_COUNT		(0x100)
	#define CHANNEL_COUNT	(0x10)

	#define AUDIO_SAMPLERATE	(44100)
	#define AUDIO_CHANNELS	(4)

	#define PCM_CHANNEL_SNAPSHOTS (32)

	#define OCTAVE_BASE_FREQUENCY 110.0

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
		None,		// Nearest-neighbor
		Gaussian,
		Hermite
	};

	struct PCMChannel
	{
		int16 data[PCM_CHANNEL_SNAPSHOTS] = {};
		bool empty = true;

		float phase = 0.0f;
		float frequency = 440.0f;
		float sampleRate = AUDIO_SAMPLERATE;

		float pan = 0.0f; // -1 = left, 0 = center, +1 = right

		std::pair<float, float> GenerateSample()
		{
			return GenerateSample(this->frequency);
		}

		std::pair<float, float> GenerateSample(float freq)
		{
			if (empty) return { 0.0f, 0.0f };

			// Calculate index into 32-sample table
			size_t index = static_cast<size_t>(phase) % 32;
			int16_t value = data[index];

			// Normalize 4-bit sample (0-15) to float -1.0 to +1.0
			float sample = ((value / 15.0f) * 2.0f) - 1.0f;

			// Equal-power pan
			// Maybe this should be a macro?
			float left = sample * std::sqrtf((1.0f - pan) * 0.5f);
			float right = sample * std::sqrtf((1.0f + pan) * 0.5f);

			// Advance phase
			phase += freq * 32.0f / sampleRate;
			if (phase >= 32.0f) phase -= 32.0f;

			return { left, right };
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
			return GenerateSample(this->freq);
		}

		float GenerateSample(float freq)
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

	struct AudioState
	{
		double time = 0.0;

		struct Voice
		{
			bool active = false;
			double freq = 0.0;
			double phase = 0.0;
			int currentNote = -1;
		};

		std::vector<Voice> tracks;
	
		void resize(int numTracks)
		{
			tracks.resize(numTracks);
		}
	};

	struct ScheduledMidiEvent
	{
		double timeInSeconds;
		int track;
		int note;
		bool isNoteOn;
	};

	struct SoundChip
	{
		PulseChannel pulse1, pulse2;
		PCMChannel pcm;
		NoiseChannel noise;

		AudioState state;

		SoundChip() = default;
	};

	class AudioDevice
	{
	public:
		static void Init();
		static void Release();

		static void ProcessMIDI(std::vector<ScheduledMidiEvent>& queue, AudioState& audio, size_t& eventIndex);
		static void TestMIDIDraw(std::vector<ScheduledMidiEvent>& queue, AudioState& audio, double totalDuration);

#if SOULCAST_USING_SDL3
		static SDL_AudioDeviceID streamDeviceID;
		static SDL_AudioStream* audioStream;
#endif
	};

	namespace Audio
	{
		PCMChannel Load4BitPCMFile(const char* filename);

		inline double MidiNoteToFreq(int midiNote)
		{
			return 440.0 * pow(2.0, (midiNote - 69) / 12.0);
		}
	}
}