mario = class:new()

function mario:init(x, y)
    self.x = x;
    self.y = y;

    self.speed = 3;
end

function mario:update()
    if (soul.gamepad.isDown(INPUT_RIGHT)) then
        self.x = self.x + self.speed;
    elseif (soul.gamepad.isDown(INPUT_LEFT)) then
        self.x = self.x - self.speed;
    end

    camX = self.x;
end

--[[HOTLOADABLE]]
function mario:render()
    soul.setActivePalette(2);
    soul.drawSprite(marioSprite, self.x, self.y);
end
--[[END]]