local testImage1;
local testImage2;
local marioTexture;

local marioSprite;

local camX = 0;
local camY = 0;

function loadBitmap(path)
    local ret = soul.bitmap.new()
    ret:load(path)

    return ret;
end

function soul.init()
    soul.loadPalette(0, "Palettes/palacebg.pal")
    soul.loadPalette(1, "Palettes/switch.pal")
    soul.loadPalette(2, "Palettes/mario.pal")

    testImage1 = loadBitmap("Sprites/palacebg.png")
    testImage2 = loadBitmap("Sprites/switch.png")
    marioTexture = loadBitmap("Sprites/mario.png")

    marioSprite = soul.sprite.new()
    marioSprite.bitmap = marioTexture;
end

function soul.update()
    camX = camX + 1;
end

function soul.render()
    soul.setActivePalette(0)
    soul.clearScreen(9)

    soul.setScreenPosition(-camX, camY)

    -- Draw backgrounds
    soul.setActivePalette(0)
    soul.drawBackground(testImage1, 0, 192)

    soul.setActivePalette(1)
    soul.drawBackground(testImage2, 0, 184)

    soul.setActivePalette(2)
    soul.drawSprite(marioSprite, 64, 168)

    soul.drawRectangle(8, 8, 32, 32, soul.paletteIndexToRGB565(2))
end