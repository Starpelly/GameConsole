testImage1 = {};
testImage2 = {};
marioTexture = {};
marioSprite = {};

camX = 0;
camY = 0;

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

    print("init")

    require("Game")
end

function soul.update()
    -- print("update");
    -- camX = camX + -1;

    game_update()
end

function soul.render()
    game_render()
end