#pragma once

class Entity
{
public:
	int32 xPos;
	int32 yPos;
};

class TestGame
{
public:
	void Update();
	void Render();

private:
	int32 x = 0;
	int32 y = 0;
};