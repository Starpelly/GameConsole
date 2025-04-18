#include "Engine/Core/SoulcastEngine.hpp"

#include <iostream>
#include <fstream>

using namespace Soulcast;

SDL_AudioDeviceID AudioDevice::streamDeviceID;

static void audioCallback(void* userdata, Uint8* stream, int len)
{
	float* out = reinterpret_cast<float*>(stream);
	int totalSamples = len / sizeof(float); // stereo float = 2 per frame

	for (int i = 0; i < totalSamples; i += 2)
	{
		float l = 0.0f, r = 0.0f;

		// l += Engine.soundChip.pulse1.GenerateSample();
		// r += Engine.soundChip.pulse2.GenerateSample();

		float pcmSample = Engine.soundChip.pcm.GenerateSample();
		l += pcmSample;
		r += pcmSample;

		// float noiseSample = Engine.soundChip.noise.GenerateSample() * 0.5f;
		// l += noiseSample;
		// r += noiseSample;

		out[i + 0] = std::clamp(l, -1.0f, 1.0f);
		out[i + 1] = std::clamp(r, -1.0f, 1.0f);
	}
}

#if SOULCAST_USING_SDL3
static void SDLCALL SDL3_AudioCallback(void* userdata, SDL_AudioStream* stream, int additional_amount, int totalAmount)
{
	(void)userdata;

	if (additional_amount > 0)
	{
		Uint8* data = SDL_stack_alloc(Uint8, additional_amount);
		if (data) {
			audioCallback(userdata, data, additional_amount);
			SDL_PutAudioStreamData(stream, data, additional_amount);
			SDL_stack_free(data);
		}
	}
}
#endif

PCMChannel Soulcast::load4BitPCMFile(const char* filename)
{
	PCMChannel channel;
	std::ifstream in(filename, std::ios::binary);
	if (!in) {
		std::cerr << "Failed to open 4-bit PCM file: " << filename << "\n";
		return channel;
	}

	uint8 byte = 0;
	uint8 index = 0;
	while (index < PCM_CHANNEL_SNAPSHOTS && in.read(reinterpret_cast<char*>(&byte), 1))
	{
		// Each byte contains two 4-bit samples (high nibble and low nibble)
		int16 high = (byte >> 4) & 0x0F;
		int16 low = byte & 0x0F;

		channel.data[index++] = high;
		if (index < PCM_CHANNEL_SNAPSHOTS)
			channel.data[index++] = low;
	}

	if (index != PCM_CHANNEL_SNAPSHOTS) {
		std::cerr << "Warning: Expected 32 4-bit samples, got " << index << "\n";
	}

	channel.empty = false;

	return channel;
}

void AudioDevice::Init()
{
#if SOULCAST_USING_SDL3
	if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
		throw std::runtime_error(SDL_GetError());

	CLITERAL(const SDL_AudioSpec) spec
	{
		SDL_AUDIO_F32,
		2,
		AUDIO_FREQUENCY
	};

	SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec,
		SDL3_AudioCallback,
		nullptr);
	if (!stream)
		throw std::runtime_error(SDL_GetError());
	streamDeviceID = SDL_GetAudioStreamDevice(stream);

	// Start audio
	SDL_ResumeAudioDevice(streamDeviceID);
#endif

	// Engine.soundChip.pcm = load4BitPCMFile("C:/Users/Braedon/Downloads/programmable_wave_samples/03.pcm");
}

void AudioDevice::Release()
{
#if SOULCAST_USING_SDL3
	if (streamDeviceID)
	{
		SDL_CloseAudioDevice(streamDeviceID);
	}
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
#endif
}

void Soulcast::AudioDevice::SetPCMFreq(float freq)
{
	Engine.soundChip.pcm.frequency = freq;
}