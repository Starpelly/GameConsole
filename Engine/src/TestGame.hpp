#pragma once

class Entity
{
public:
	uint32 entityIndex = 0;

	int32 x;
	int32 y;

	virtual void Update() = 0;
	virtual void Render() = 0;
};

class TestGame
{
public:
	TestGame();
	~TestGame();
	
	void Update();
	void Render();

private:
	int32 x = 0;
	int32 y = 0;

	float32 pos_x = 0.0f;
	float32 pos_y = 0.0f;
	float32 speed = 0.0f;
};

class PlayerEntity : Entity
{
public:
	void Update() override;
	void Render() override;
};