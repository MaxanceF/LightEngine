	#pragma once

#include "Scene.h"
#include "GameManager.h"
#include "Entity.h"

template<typename T>
T* Scene::CreateEntity(int width, int height, const sf::Color& color)
{
	static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

	T* newEntity = new T();

	Entity* entity = newEntity;
	entity->Initialize(width, height, color);
	
	mpGameManager->mEntitiesToAdd.push_back(newEntity);

	return newEntity;
}
