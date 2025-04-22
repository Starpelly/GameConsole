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
    soul.loadPalette(0, "Palettes/mariobg.pal")
    soul.loadPalette(1, "Palettes/donutplains.pal")
    soul.loadPalette(2, "Palettes/mario.pal")

    testImage1 = loadBitmap("Sprites/marioworldbg.png")
    testImage2 = loadBitmap("Sprites/donutplains.png")
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