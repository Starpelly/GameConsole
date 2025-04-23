mario = class:new()

function mario:init(x, y)
    self.x = x;
    self.y = y;

    self.vel_x = 0;

    self.sprite_flip = FLIP_NONE;

    self.speed = 0.1;
    self.anim_frame = 0;
end

--[[HOTLOADABLE]]

function mario:update()

    if (soul.gamepad.isDown(INPUT_RIGHT)) then
        self.sprite_flip = FLIP_NONE;
        self.vel_x = self.vel_x + self.speed;
    elseif (soul.gamepad.isDown(INPUT_LEFT)) then
        self.sprite_flip = FLIP_X;
        self.vel_x = self.vel_x - self.speed;
    end

    if (soul.gamepad.isPressed(INPUT_RIGHT) or soul.gamepad.isPressed(INPUT_LEFT)) then
        self.anim_frame = 1;
    end

    -- animation
    local anim_speed = math.abs(self.vel_x);
    if (anim_speed < 1.0 and anim_speed > 0.2) then
        anim_speed = 1.0
    end

    self.anim_frame = self.anim_frame + (1.0 / 32.0) * (math.abs(anim_speed) * 2.5);
    if (self.anim_frame > 2 or math.abs(self.vel_x) < 0.2) then
        self.anim_frame = 0;
    end

    self.x = self.x + self.vel_x;
    -- camX = math.floor(self.x + 0.5) - 100;

    camX = camX - 1
end

function mario:render()
    soul.setActivePalette(2);

    local sprite_frame = math.floor(self.anim_frame);
    soul.drawSpriteRegion(marioSprite, math.floor(self.x + 0.5), math.floor(self.y + 0.5), 16 * sprite_frame, 0, 16, 32, self.sprite_flip);
end
--[[END]]