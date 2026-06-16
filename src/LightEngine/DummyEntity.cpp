#include "DummyEntity.h"

#include <iostream>
#include <SFML/Graphics.hpp>

#include "SampleScene.h"

void DummyEntity::OnCollision(Entity* other)
{
	//std::cout << "DummyEntity::OnCollision" << std::endl;
}

sf::Clock actionClock;
void DummyEntity::OnUpdate()
{
	if (actionClock.getElapsedTime().asSeconds() >= 0.1f && GetScene<SampleScene>()->pEntitySelected != nullptr)
	{
		actionClock.restart();
		CreateEntity<DummyEntity>(4, sf::Color::White);
		Entity* pEntity = CreateEntity<Entity>(4, sf::Color::White);
		pEntity->SetPosition(this->GetPosition().x, this->GetPosition().y);
	}
}
