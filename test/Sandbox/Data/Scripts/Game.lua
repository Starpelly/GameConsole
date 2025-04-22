player = {};
camX = 0;
camY = 0;

function game_init()
    player = mario:new(64, 168)
end

--[[HOTLOADABLE]]
function game_update()
    player:update()
end

function game_render()
    soul.setActivePalette(0)
    soul.clearScreen(9)

    -- NEKO_DrawPixel(0, 0)

    --[[
    for y = 0, SCREEN_YSIZE - 1 do
        for x = 0, SCREEN_XSIZE - 1 do
            -- Create a red gradient from left (0) to right (31)
            local red = math.floor((x * 31) / SCREEN_XSIZE)
            local color = (red << 11) -- red in RGB565 format (bits 11–15)

            local pixel_index = y * SCREEN_XSIZE + x
            local addr = FRAMEBUFFER_START + pixel_index * 2

            -- Write lower and upper byte (little-endian)
            soul.memory.write(addr, color & 0xFF)
            soul.memory.write(addr + 1, (color >> 8) & 0xFF)
        end
    end
    --]]

    soul.setScreenPosition(-camX, camY)

    -- Draw backgrounds
    soul.setActivePalette(0)
    soul.drawBackground(testImage1, math.floor((-camX / 2) + 0.5), 192)

    soul.setActivePalette(1)
    soul.drawBackground(testImage2, 0, 184)

    player:render()

    neko.drawPixel(0, 0, 255, 0, 255)
end
--[[END]]