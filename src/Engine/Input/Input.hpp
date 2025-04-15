#pragma once

namespace Soulcast
{
    #define PLAYER_COUNT		(4)
    #define INPUTDEVICE_COUNT	(16)

    enum class InputSlotIDs
    {
	    Any,
	    P1,
	    P2,
	    P3,
	    P4
    };

    enum class InputDeviceTypes
    {
	    None,
	    Keyboard,
	    Controller,
	    Unknown
    };

    enum InputButtons
    {
        INPUT_UP,
        INPUT_DOWN,
        INPUT_LEFT,
        INPUT_RIGHT,
        INPUT_A,
        INPUT_B,
        INPUT_X,
        INPUT_Y,
        INPUT_START,
        INPUT_SELECT,
        INPUT_ANY,
        INPUT_MAX,
    };

    struct InputData
    {
        bool up;
        bool down;
        bool left;
        bool right;
        bool A;
        bool B;
        bool X;
        bool Y;
        bool start;
    };

    struct InputButton
    {
        bool press;
        bool hold;
    
        int keyMappings;
        int padMappings;

        inline void setHeld()
        {
            press = !hold;
            hold  = true;
        }

        inline void setReleased()
        {
            press = false;
            hold  = false;
        }

        inline bool down() const { return (press || hold); }
    };

    namespace Input
    {
        void Init();
        void Clear();
        void Process();
        void Release();

        bool CheckButtonDown(InputButtons button);

        extern InputButton inputDevice[INPUT_MAX];
    }
}