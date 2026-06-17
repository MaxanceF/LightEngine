#pragma once

#include "Scene.h"

class DummyEntity;

class SampleScene : public Scene
{

public:
	static constexpr int GRID_WIDTH = 64;
	static constexpr int GRID_HEIGHT = 64;

	std::vector<int> grid;

	SampleScene()
		: grid(GRID_WIDTH * GRID_HEIGHT, 0)
	{
	}
	
	int& GetCell(int x, int y);
	void SetCell(int x, int y, int value);
	void printGrid() const;
	void SetAllValueCellToWall(int value);
	
private:
	
	DummyEntity* pEntity1;
	DummyEntity* pEntity2;
	
	DummyEntity* pEntitySelected;

	

private:
	void TrySetSelectedEntity(DummyEntity* pEntity, int x, int y);

public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;
};