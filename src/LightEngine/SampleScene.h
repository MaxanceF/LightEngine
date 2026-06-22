#pragma once

#include "Scene.h"

class DummyEntity;

struct Case
{
	int x;
	int y;
	int value;
};

class SampleScene : public Scene
{

public:
	static constexpr int GRID_WIDTH = 64;
	static constexpr int GRID_HEIGHT = 64;

	
	std::vector<Case> grid;

	SampleScene();
	
	int& GetCell(int x, int y);
	void SetCell(int x, int y, int value);
	void printGrid() const;
	void SetAllValueCellToWall(int value);
	int GetPlayerGridX();
	int GetPlayerGridY();
	
private:
	
	DummyEntity* pEntity1;
	DummyEntity* pEntity2;
	
	DummyEntity* pEntitySelected;
	
	void TrySetSelectedEntity(DummyEntity* pEntity, int x, int y);
	
	void UpdateGridSquareVisual();
	
	

public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;
};