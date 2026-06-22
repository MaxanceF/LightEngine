#pragma once
#include "DummyEntity.h"
#include "Wall.h"
#include <queue>
#include <vector>
#include <algorithm>

class DummyEnemy : public DummyEntity
{
	bool IsOpposite(const sf::Vector2f& a, const sf::Vector2f& b);
public:
	void OnUpdate() override;
	int CountReachableCells(int startX, int startY);
	DummyEnemy();
};

