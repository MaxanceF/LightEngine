#pragma once

#include "Scene.h"

class CircleEntity;

class SampleScene : public Scene
{
	std::vector<CircleEntity*> _entities;
	
	float mFoodSpawnTimer = 0.f;
	float mFoodSpawnCooldown = 0.16f; 

private:
	int RandomNumber(int min, int max);

public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;

	std::vector<CircleEntity*> GetEntities() { return _entities; }
};


