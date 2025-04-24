#include "Engine/Core/Engine.hpp"

#include <iostream>
#include <fstream>

#include <Midifile.h>

using namespace Soulcast;

SDL_AudioDeviceID AudioDevice::streamDeviceID;
SDL_AudioStream* AudioDevice::audioStream;

static void audioCallback(void* userdata, Uint8* stream, int len)
{
	float* out = reinterpret_cast<float*>(stream);
	int totalSamples = len / sizeof(float); // stereo float = 2 per frame

	for (int i = 0; i < totalSamples; i += 2)
	{
		float l = 0.0f, r = 0.0f;

		// l += Engine.soundChip.pulse1.GenerateSample();
		// r += Engine.soundChip.pulse2.GenerateSample();

		// auto[pcmL, pcmR] = Engine.soundChip.pcm.GenerateSample();
		// l += pcmL;
		// r += pcmR;

		// float noiseSample = Engine.soundChip.noise.GenerateSample() * 0.5f;
		// l += noiseSample;
		// r += noiseSample;

		// for (auto& voice : Engine.soundChip.state.tracks)
		for (int track = 0; track < Engine.soundChip.state.tracks.size(); track++)
		{
			auto& voice = Engine.soundChip.state.tracks[track];
			if (!voice.active) continue;

			switch (track - 2)
			{
			case 0:
			case 1:
			default:
			{
				// @HACK - The first two tracks are pulse waves, not square waves,
				// Fix it, you retard!!!

				float value = static_cast<float>((fmod(voice.phase, 1.0) < 0.5 ? 1.0 : -1.0));
				l += value * 0.9;
				r += value * 0.9;

				// double value = Engine.soundChip.pulse1.GenerateSample(voice.freq);
				// sample += value * 0.2f;
			}
			break;
			case 2:
			{
				auto [pcmL, pcmR] = Engine.soundChip.pcm.GenerateSample(voice.freq);
				l += pcmL;
				r += pcmR;
			}
			break;
			}

			voice.phase += voice.freq / AUDIO_SAMPLERATE;
		}

		l *= 0.2;
		r *= 0.2;

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
		AUDIO_SAMPLERATE
	};

	audioStream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec,
		SDL3_AudioCallback,
		nullptr);
	if (!audioStream)
		throw std::runtime_error(SDL_GetError());
	streamDeviceID = SDL_GetAudioStreamDevice(audioStream);

	// Start audio
	SDL_ResumeAudioDevice(streamDeviceID);
#endif
}

static void NoteOn(AudioState& audio, int track, int note)
{
	if (track < 0 || track >= (int)audio.tracks.size()) return;
	double freq = Audio::MidiNoteToFreq(note);

#if SOULCAST_USING_SDL3
	SDL_LockAudioStream(AudioDevice::audioStream);
	audio.tracks[track] = { true, freq, 0.0, note };
	SDL_UnlockAudioStream(AudioDevice::audioStream);
#endif
}

static void NoteOff(AudioState& audio, int track, int note)
{
	if (track < 0 || track >= (int)audio.tracks.size()) return;

#if SOULCAST_USING_SDL3
	SDL_LockAudioStream(AudioDevice::audioStream);
	if (audio.tracks[track].currentNote == note)
	{
		audio.tracks[track].active = false;
	}
	SDL_UnlockAudioStream(AudioDevice::audioStream);
#endif
}

double zoom = 1;
double xpos = 0;

void AudioDevice::TestMIDIDraw(std::vector<ScheduledMidiEvent>& queue, AudioState& audio, double totalDuration)
{
	PPU::ClearScreen(0);

	if (Input::IsButtonPressed(INPUT_DOWN))
	{
		zoom -= 1;
	}
	if (Input::IsButtonPressed(INPUT_UP))
	{
		zoom += 1;
	}

	if (zoom <= 0)
		zoom = 1;

	if (Input::IsButtonDown(INPUT_LEFT))
	{
		xpos -= 1 * zoom;
	}
	if (Input::IsButtonDown(INPUT_RIGHT))
	{
		xpos += 1 * zoom;
	}

	if (xpos < 0)
		xpos = 0;

	double screenWidth = SCREEN_XSIZE * zoom;

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

		float x = ((start / totalDuration) * screenWidth) - xpos;
		float width = ((end - start) / totalDuration) * screenWidth;
		float y = SCREEN_YSIZE - (ev.note / 127.0f) * SCREEN_YSIZE;
		float height = 2.0f;

		int color = RGB888_TO_RGB565(220, 96, 105);
		if (ev.track == 3)
			color = RGB888_TO_RGB565(141, 187, 110);
		if (ev.track == 4)
			color = RGB888_TO_RGB565(225, 184, 111);

		PPU::DrawRectangle(x, y, width, height, color);
	}

	float cursorX = ((audio.time / totalDuration) * screenWidth) - xpos
		;
	PPU::DrawRectangle(cursorX, 0, 1, SCREEN_YSIZE, 0xFF00);
}

void AudioDevice::ProcessMIDI(std::vector<ScheduledMidiEvent>& queue, AudioState& audio, size_t& eventIndex)
{
	audio.time = Engine.time - 1.0f;

	while (eventIndex < queue.size() && queue[eventIndex].timeInSeconds <= audio.time)
	{
		const auto& ev = queue[eventIndex];
		if (ev.isNoteOn)
		{
			NoteOn(audio, ev.track, ev.note);
		}
		else
		{
			NoteOff(audio, ev.track, ev.note);
		}
		++eventIndex;
	}
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