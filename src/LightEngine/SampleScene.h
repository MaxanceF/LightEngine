#pragma once

#include "Food.h"
#include "Spike.h"
#include "Scene.h"

class Eater;

class SampleScene : public Scene
{
	DummyEntity* pEntity1;
	DummyEntity* pEntity2;

	DummyEntity* pEntitySelected;


private:
	void TrySetSelectedEntity(Eater* pEntity, int x, int y);

public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;
};


