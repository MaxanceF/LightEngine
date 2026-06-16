#pragma once

#include "Scene.h"
#include <vector>
#include "ScreenMelter.h"

class CircleEntity;

class SampleScene : public Scene
{
	sf::Vector2f FindFreePosition(float radius);
	
	std::vector<CircleEntity*> _entities;

	float mFoodSpawnTimer = 0.f;
	float mFoodSpawnCooldown = 0.16f;
	
	std::vector<float> mPendingEnemyRespawns; 

private:
	int RandomNumber(int min, int max);

public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;
	void DeleteEntity(Entity* entity);
	void SpawnEntity();
	std::vector<CircleEntity*>& GetEntities() { return _entities; }
	void AddEntity(CircleEntity* entity);
	template<typename T>
T* CreateAndRegister(float radius, const sf::Color& color)
	{
		auto* e = CreateEntity<T>(radius, color);
		_entities.push_back(e);
		return e;
	}
};