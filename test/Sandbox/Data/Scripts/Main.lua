testImage1 = {};
testImage2 = {};
marioTexture = {};
marioSprite = {};

package.path = package.path .. ";./scripts/?.lua"

local function loadBitmap(path)
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

    require("Neko")
    require("Class")
    require("Game")

    require("Actors/Mario")

    game_init()
end

function soul.update()
    game_update()
end

function soul.render()
    game_render()
end