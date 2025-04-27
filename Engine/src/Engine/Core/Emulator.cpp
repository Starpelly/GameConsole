#include "Engine/Core/Engine.hpp"

using namespace Soulcast;

static MidiFile midifile;
double duration = 0.0;
size_t eventIndex = 0;

bool Emulator::Init(SDL_Window* window)
{
#if SOULCAST_USING_SDL3
    if (!this->windowContained)
    {
        SDL_Init(SDL_INIT_EVENTS);
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync ? "1" : "0");
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

    uint8 flags = 0;

#if SOULCAST_USING_OPENGL
    flags |= SDL_WINDOW_OPENGL;

#if SOULCAST_PLATFORM != SOULCAST_ANDROID
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
#endif

    flags |= SDL_WINDOW_RESIZABLE;
    if (this->windowContained)
    {
        flags |= SDL_WINDOW_HIDDEN;
    }
    if (this->borderless)
    {
        flags |= SDL_WINDOW_BORDERLESS;
    }

    int32 windowWidth = SCREEN_XSIZE * windowScale;
    int32 windowHeight = SCREEN_YSIZE * windowScale;

    if (window == nullptr)
    {
        this->window = SDL_CreateWindow("Soulcast", windowWidth, windowHeight, SDL_WINDOW_HIGH_PIXEL_DENSITY | flags);
    }
    else
    {
        this->window = window;
    }

    if (!this->window)
    {
        printf("ERROR: Failed to created window!\nError Message: %s", SDL_GetError());
        return 0;
    }

#if !SOULCAST_USING_OPENGL
    this->renderer = SDL_CreateRenderer(this->window, NULL);

    if (!this->renderer)
    {
        printf("ERROR: Failed to create renderer!\nError Message: %s", SDL_GetError());
        return 0;
    }

    SDL_SetRenderLogicalPresentation(this->renderer, SCREEN_XSIZE, SCREEN_YSIZE, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);

    // Game screen buffer texture
    {
        Drawing::InitScreenInfo(&this->gameScreen, SCREEN_XSIZE, SCREEN_YSIZE, true);

        this->gameScreen.screenTexture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, SCREEN_XSIZE, SCREEN_YSIZE);
        SDL_SetTextureScaleMode(this->gameScreen.screenTexture, SDL_SCALEMODE_NEAREST);

        if (!this->gameScreen.screenTexture)
        {
            printf("ERROR: Failed to create screen buffer!\nError Message: %s", SDL_GetError());
        }
    }
#endif

    if (this->borderless)
    {
        SDL_RestoreWindow(window);
        SDL_SetWindowBordered(window, false);
    }

    SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    // SDL_SetWindowResizable(Engine.window, !Engine.debugMode);

    SDL_DisplayID winID = SDL_GetDisplayForWindow(this->window);
    const SDL_DisplayMode* disp = SDL_GetCurrentDisplayMode(winID);
    if (disp != nullptr)
    {
        this->screenRefreshRate = disp->refresh_rate;
    }
    else
    {
        printf("ERROR: %s", SDL_GetError());
    }
#endif

    this->initialized = true;
    this->running = true;
    this->mode = EMU_MAINGAME;

    this->targetFreq = SDL_GetPerformanceFrequency() / this->refreshRate;
    this->curTicks = 0;
    this->prevTicks = 0;

    midifile.read("Data/Music/smw.mid");
    midifile.doTimeAnalysis();

    duration = midifile.getFileDurationInSeconds();

    // this->soundChip.state.resize(midifile.getTrackCount());

    Engine.runningEmulator = this;

    // Active screen is the game screen by default
    Drawing::SetActiveScreen(&gameScreen);

    return 0;
}

void Emulator::Run()
{
    while (this->running)
    {
        DoOneFrame();
    }
}

void Emulator::DoOneFrame()
{
    if (!this->vsync)
    {
        curTicks = SDL_GetPerformanceCounter();
        if (curTicks < prevTicks + targetFreq)
            return;
        prevTicks = curTicks;
    }

    this->time = SDL_GetTicks() / 1000.0;

#if SOULCAST_USING_SDL3
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        this->running = ProcessEvent(event);
    }
#endif

    Drawing::SetActiveScreen(&this->gameScreen);

    for (int s = 0; s < this->gameSpeed; ++s)
    {
        Input::Process();

        if (!masterPaused || this->frameStep)
        {
            switch (this->mode)
            {
            case EMU_MAINGAME:
                ScriptingEngine::UpdateScripts();
                ScriptingEngine::RenderScripts();

                // AudioDevice::ProcessMIDI(eventQueue, Engine.soundChip.state, eventIndex);
                // AudioDevice::TestMIDIDraw(eventQueue, Engine.soundChip.state, duration);

                // Drawing::DrawRectangle(Input::mouseX / Engine.windowScale, Input::mouseY / Engine.windowScale, 32, 32, 0xFFFF);
                break;

            case EMU_EXITGAME:
                this->running = false;
                break;

            case EMU_PAUSE:
            case EMU_WAIT:
                break;

            default: break;
            }
        }
    }

#if SOULCAST_USING_SDL3
    // Clear screen
    SDL_SetRenderDrawColor(Engine.runningEmulator->renderer, 0, 0, 0, 255);
    SDL_RenderClear(Engine.runningEmulator->renderer);
#endif

    // Render Drawing output to physical screen!
    Drawing::Present(this->gameScreen, 0, 0, SCREEN_XSIZE, SCREEN_YSIZE);

#if SOULCAST_USING_SDL3
    SDL_RenderPresent(Engine.runningEmulator->renderer);
#endif

#if SOULCAST_USING_OPENGL && SOULCAST_USING_SDL3
    SDL_GL_SwapWindow(Engine.window);
#endif

    this->frameStep = false;
}

void Emulator::Release()
{
#if SOULCAST_USING_SDL3
    SDL_DestroyTexture(gameScreen.screenTexture);

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
#endif

    Engine.runningEmulator = nullptr;
}

void Emulator::ResetSystem()
{
    ScriptingEngine::Reset();
}

#if SOULCAST_USING_SDL3
bool Emulator::ProcessEvent(const SDL_Event& event)
#else
bool Emulator::ProcessEvent()
#endif
{
    switch (event.type)
    {
    case SDL_EVENT_TERMINATING:
        this->mode = EMU_EXITGAME; return false;
    case SDL_EVENT_QUIT:
        this->mode = EMU_EXITGAME; return false;

    case SDL_EVENT_KEY_DOWN:
    {
        auto key = event.key.scancode;

        if (key == SDL_SCANCODE_R) // I'm lazy
        {
            ResetSystem();
        }
        if (key == SDL_SCANCODE_A)
        {
            AudioDevice::SetChannelActive(Audio::Channel::PCM, true);
            AudioDevice::SetChannelFrequency(Audio::Channel::PCM, 440);
        }
        if (key == SDL_SCANCODE_S)
        {
            AudioDevice::SetChannelActive(Audio::Channel::Pulse0, true);
            AudioDevice::SetChannelFrequency(Audio::Channel::Pulse0, 440);
        }
    }
        break;

    case SDL_EVENT_KEY_UP:
    {
        auto key = event.key.scancode;

        if (key == SDL_SCANCODE_A)
        {
            AudioDevice::SetChannelActive(Audio::Channel::PCM, false);
        }
        if (key == SDL_SCANCODE_S)
        {
            AudioDevice::SetChannelActive(Audio::Channel::Pulse0, false);
        }
    }
        break;
    }

    return true;
}
