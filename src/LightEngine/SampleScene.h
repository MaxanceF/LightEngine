#pragma once

#include "Food.h"
#include "Spike.h"
#include "Scene.h"

class Eater;

class SampleScene : public Scene
{
	Eater* pEntity1;
	Eater* pEntity2;

	Eater* pEntitySelected;

	Food* pFood1;
	Spike* pSpike1;

private:
	void TrySetSelectedEntity(Eater* pEntity, int x, int y);

public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;
};


