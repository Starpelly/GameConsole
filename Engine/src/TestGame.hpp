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
	TestGame();
	
	void Update();
	void Render();

private:
	int32 x = 0;
	int32 y = 0;

	float32 pos_x = 0.0f;
	float32 pos_y = 0.0f;
	float32 speed = 0.0f;
};