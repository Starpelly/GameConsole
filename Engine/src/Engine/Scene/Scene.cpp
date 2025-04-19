#include "Engine/Core/SoulcastEngine.hpp"

namespace Soulcast
{
	BackgroundLayer backgroundLayers[BACKGROUND_LAYER_COUNT];
	Object objectList[OBJECT_COUNT];
}

using namespace Soulcast;

void Scene::RenderObjects()
{
	for (int32 o = 0; o < OBJECT_COUNT; o++)
	{
		auto* object = &objectList[o];
	}
}