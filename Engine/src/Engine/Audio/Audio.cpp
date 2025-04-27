#include "Engine/Core/Engine.hpp"

#include <iostream>
#include <fstream>

using namespace Soulcast;
using namespace Soulcast::Audio;

SDL_AudioDeviceID AudioDevice::streamDeviceID;
SDL_AudioStream* AudioDevice::audioStream;
SoundChip AudioDevice::soundChip;

static void audioCallback(void* userdata, Uint8* stream, int len)
{
	auto* chip = reinterpret_cast<SoundChip*>(userdata);

	int numSamples = len / sizeof(float) / 2;
	float* floatStream = reinterpret_cast<float*>(stream);

	AudioDevice::GenerateAudio(floatStream, numSamples, AUDIO_SAMPLERATE);
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

PCMChannel Audio::Load4BitPCMFile(const char* filename)
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

	if (index != PCM_CHANNEL_SNAPSHOTS)
	{
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
		AUDIO_SAMPLERATE
	};

	audioStream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec,
		SDL3_AudioCallback,
		&soundChip);
	if (!audioStream)
		throw std::runtime_error(SDL_GetError());
	streamDeviceID = SDL_GetAudioStreamDevice(audioStream);

	// Start audio
	SDL_ResumeAudioDevice(streamDeviceID);
#endif

	for (auto i = 0; i < soundChip.channels.size(); i++)
	{
		soundChip.channels[i].type = i;
	}
}

#if false
void AudioDevice::TestMIDIDraw(std::vector<ScheduledMidiEvent>& queue, AudioState& audio, double totalDuration)
{
	Drawing::ClearScreen(0);

	if (Input::IsButtonPressed(INPUT_DOWN))
	{
		zoom -= 4;
	}
	if (Input::IsButtonPressed(INPUT_UP))
	{
		zoom += 4;
	}

	if (zoom <= 0)
		zoom = 1;

	const float pixelsPerSecond = 32 * zoom;
	const float screenWidthInSeconds = SCREEN_XSIZE / pixelsPerSecond;

	if (Input::IsButtonPressed(INPUT_LEFT))
	{
		xpos -= 1;
	}
	if (Input::IsButtonPressed(INPUT_RIGHT))
	{
		xpos += 1;
	}

	if (xpos < 0)
		xpos = 0;

	const float leftSeconds = xpos;
	const float rightSeconds = xpos + screenWidthInSeconds;

	for (const auto& ev : queue)
	{
		if (!ev.isNoteOn) continue;
		double start = ev.timeInSeconds;
		double end = 0;

		for (const auto& off : queue)
		{
			if (!off.isNoteOn && off.note == ev.note && off.timeInSeconds > start)
			{
				end = off.timeInSeconds;
				break;
			}
		}

		if (start + end < leftSeconds)
			continue;
		if (start > rightSeconds)
			break;

		auto startVisual = start - xpos;
		auto endVisual = end - xpos;

		float x = (startVisual * pixelsPerSecond);
		float width = (endVisual - startVisual) * pixelsPerSecond;
		float y = SCREEN_YSIZE - (ev.note / 127.0f) * SCREEN_YSIZE;
		float height = 2.0f;

		if (width < 0.5)
			continue;

		uint16 color = 0xFFFF;
		/*
		if (eventIndex == i)
		{

		}
		else
		*/
		{
			color = RGB888_TO_RGB565(220, 96, 105);
			if (ev.track == 3)
				color = RGB888_TO_RGB565(141, 187, 110);
			if (ev.track == 4)
				color = RGB888_TO_RGB565(225, 184, 111);
		}

		Drawing::DrawRectangle(x, y, width, height, color);
	}

	float cursorX = ((audio.time - xpos) * pixelsPerSecond);
	Drawing::DrawRectangle(cursorX, 0, 1, SCREEN_YSIZE, 0xFF00);
}
#endif

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

void AudioDevice::GenerateAudio(float* stream, int numSamples, int sampleRate)
{
	memset(stream, 0, sizeof(float) * numSamples * 2); // *2 for stereo

	for (auto& ch : soundChip.channels)
	{
		if (!ch.active)
			continue;

		switch (ch.type)
		{
		case Channel::Pulse0:
		case Channel::Pulse1:
		{
			double phaseIncrement = ch.frequency / sampleRate;

			for (int i = 0; i < numSamples; ++i)
			{
				float value = (std::fmod(ch.phase, 1.0) < 0.5) ? 1.0f : -1.0f;
				value *= 0.2f; // small volume

				stream[i * 2 + 0] += value; // Left
				stream[i * 2 + 1] += value; // Right

				ch.phase += phaseIncrement;
				if (ch.phase >= 1.0)
					ch.phase -= 1.0;
			}
		}
		break;
		case Channel::PCM:
		{
			for (int i = 0; i < numSamples; ++i)
			{
				if (!ch.pcm.empty)
				{
					// Calculate index into 32-sample table
					size_t index = static_cast<size_t>(ch.phase) % 32;
					int16_t value = ch.pcm.data[index];

					// Normalize 4-bit sample (0-15) to float -1.0 to +1.0
					float sample = ((value / 15.0f) * 2.0f) - 1.0f;

					// Equal-power pan
					// Maybe this should be a macro?
					float left = sample * std::sqrtf((1.0f - ch.pan) * 0.5f);
					float right = sample * std::sqrtf((1.0f + ch.pan) * 0.5f);

					// Advance phase
					ch.phase += ch.frequency * 32.0f / sampleRate;
					if (ch.phase >= 32.0f) ch.phase -= 32.0f;

					stream[i * 2 + 0] += left * 0.2f;
					stream[i * 2 + 1] += right * 0.2f;
				}
			}
		}
		break;
		}
	}

	// @FIX - Please look into clamping!

	/*
	for (int i = 0; i < numSamples; i += 2)
	{
		stream[i + 0] = std::clamp(l, -1.0f, 1.0f);
		stream[i + 1] = std::clamp(r, -1.0f, 1.0f);
	}
	*/
}

void AudioDevice::SetChannelFrequency(int channelIndex, double frequency)
{
	if (channelIndex >= 0 && channelIndex < Channel::Count)
	{
		soundChip.channels[channelIndex].frequency = frequency;
	}
}

void AudioDevice::SetChannelActive(int channelIndex, bool active)
{
	if (channelIndex >= 0 && channelIndex < Channel::Count)
	{
		soundChip.channels[channelIndex].active = active;
	}
}
