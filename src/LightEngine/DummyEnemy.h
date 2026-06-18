#pragma once
#include "DummyEntity.h"
#include "Wall.h"

class DummyEnemy : public DummyEntity
{
	bool IsOpposite(const sf::Vector2f& a, const sf::Vector2f& b);
public:
	void Inputs() override;

};

