#include "SampleScene.h"

#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>

#include "DummyEntity.h"
#include "DummyEnemy.h"

#include "Debug.h"

void SampleScene::OnInitialize()
{
	CreateGridVisual();
	pEntity1 = CreateEntity<DummyEntity>(sf::Vector2f{10.0f,10.0f}, sf::Color::Green);
	pEntity1->SetPosition(480, 320);
	pEntity1->SetRigidBody(true);

	pEntity2 = CreateEntity<DummyEnemy>(sf::Vector2f{10.0f,10.0f}, sf::Color::Magenta);
	pEntity2->SetPosition(160, 320);
	pEntity2->SetRigidBody(true);

	pEntitySelected = nullptr;
}

void SampleScene::OnEvent(const sf::Event& event)
{
	if (event.type != sf::Event::EventType::MouseButtonPressed)
		return;

	if (event.mouseButton.button == sf::Mouse::Button::Right)
	{
		TrySetSelectedEntity(pEntity1, event.mouseButton.x, event.mouseButton.y);
		//TrySetSelectedEntity(pEntity2, event.mouseButton.x, event.mouseButton.y);
	}

	if (event.mouseButton.button == sf::Mouse::Button::Left)
	{
		if (pEntitySelected != nullptr) 
		{
			pEntitySelected->GoToPosition(event.mouseButton.x, event.mouseButton.y, 100.f);
		}
	}
}


void SampleScene::TrySetSelectedEntity(DummyEntity* pEntity, int x, int y)
{
	if (pEntity->IsInside(x, y) == false)
		return;

	pEntitySelected = pEntity;
}

void SampleScene::OnUpdate()
{
	if(pEntitySelected != nullptr)
	{
		sf::Vector2f position = pEntitySelected->GetPosition();
		Debug::DrawCircle(position.x, position.y, 10, sf::Color::Blue);
	}
}

int& SampleScene::GetCell(int x, int y)
{
	return grid[y * GRID_WIDTH + x];
}

void SampleScene::SetCell(int x, int y, int value)
{
	grid[y * GRID_WIDTH + x] = value;
}

void SampleScene::printGrid() const
{
	for (int y = 0; y < GRID_HEIGHT; y++)
	{
		for (int x = 0; x < GRID_WIDTH; x++)
		{
			switch (grid[y * GRID_WIDTH + x])
			{
			case -1: std::cout << '#'; break; // mur
			case 0:  std::cout << '.'; break; // vide
			case 1:  std::cout << 'A'; break; // joueur 1
			case 2:  std::cout << 'B'; break; // joueur 2
			default: std::cout << '?'; break;
			}
		}

		std::cout << '\n';
	}
}

void SampleScene::SetAllValueCellToWall(int value)
{
	for (int& cell : grid)
	{
		if (cell == value)
		{
			cell = -1;
		}
	}
}

