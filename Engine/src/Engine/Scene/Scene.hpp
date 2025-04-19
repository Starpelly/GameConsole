#pragma once

namespace Soulcast
{
#define BACKGROUND_LAYER_COUNT	(8)
#define OBJECT_COUNT			(512)

	struct Object
	{
		int32 x = 0;
		int32 y = 0;

		int32 priority = 0;

		bool hflip = false;
		bool vflip = false;

		Image* tile = nullptr;
	};

	struct BackgroundLayer
	{
		Vector2 position;
	};

	namespace Scene
	{
		extern BackgroundLayer backgroundLayers[BACKGROUND_LAYER_COUNT];
		extern Object objectList[OBJECT_COUNT];

		void RenderObjects();
	}
}