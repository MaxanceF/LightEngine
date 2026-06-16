#include "Entity.h"

#include "GameManager.h"

template<typename T>
T* Entity::GetScene() const
{
	T* pScene = dynamic_cast<T*>(GetScene());

	_ASSERT(pScene != nullptr);

	return pScene;
}


template<typename T>
T* Entity::CreateEntity(sf::Vector2f size, const sf::Color& color)
{
	return GetScene()->CreateEntity<T>(size, color);
}