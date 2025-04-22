--[[HOTLOADABLE]]

function game_update()
    camX = camX + 1;
end

function game_render()
    soul.setActivePalette(0)
    soul.clearScreen(9)

    soul.setScreenPosition(-camX, camY)

    -- Draw backgrounds
    soul.setActivePalette(0)
    soul.drawBackground(testImage1, 0, 192)

    soul.setActivePalette(1)
    soul.drawBackground(testImage2, 0, 184)

    soul.setActivePalette(2)
    soul.drawSprite(marioSprite, 64 + camX, 168)

    soul.drawRectangle(8, 8, 32, 32, soul.paletteIndexToRGB565(2))
end

--[[END]]