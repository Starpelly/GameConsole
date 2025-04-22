SCREEN_XSIZE = 320;
SCREEN_YSIZE = 240;

FRAMEBUFFER_START = 0x0000

INPUT_UP        = 0
INPUT_DOWN      = 1
INPUT_LEFT      = 2
INPUT_RIGHT     = 3
INPUT_A         = 4
INPUT_B         = 5
INPUT_X         = 6
INPUT_Y         = 7
INPUT_START     = 8
INPUT_SELECT    = 9
INPUT_ANY       = 10
INPUT_MAX       = 11

neko = {}

-- This seems pretty slow, especially in lua
-- But it's just a test!
function neko.drawPixel(x, y, r, g, b)
    -- Clamp coordinates to screen
    if x < 0 or x >= SCREEN_XSIZE or y < 0 or y >= SCREEN_YSIZE then
        return
    end

    -- Convert 8-bit per channel RGB to RGB565 format
    local r5 = math.floor(r * 31 / 255)
    local g6 = math.floor(g * 63 / 255)
    local b5 = math.floor(b * 31 / 255)

    local color = (r5 << 11) | (g6 << 5) | b5

    local pixel_index = y * SCREEN_XSIZE + x
    local addr = FRAMEBUFFER_START + pixel_index * 2

    soul.memory.write(addr, color & 0xFF)         -- low byte
    soul.memory.write(addr + 1, (color >> 8) & 0xFF) -- high byte
end