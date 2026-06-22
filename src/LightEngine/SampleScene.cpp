#include "SampleScene.h"

#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>

#include "DummyEntity.h"
#include "DummyEnemy.h"

#include "Debug.h"

void SampleScene::OnInitialize()
{
	CreateGridVisual();
	
	pEntity1 = CreateEntity<DummyEntity>(sf::Vector2f{10.0f,10.0f}, sf::Color::Transparent);
	pEntity1->SetPosition(480, 320);
	SetCell(48,32,1);
	pEntity1->SetRigidBody(true);
	pEntity1->SetMGridPos(48,32);

	pEntity2 = CreateEntity<DummyEnemy>(sf::Vector2f{10.0f,10.0f}, sf::Color::Transparent);
	pEntity2->SetPosition(160, 320);
	SetCell(16,32,2);
	pEntity2->SetRigidBody(true);
	pEntity2->SetMGridPos(16,32);

	pEntitySelected = nullptr;
}

void SampleScene::OnEvent(const sf::Event& event)
{
	if (event.type != sf::Event::EventType::MouseButtonPressed)
		return;

	if (event.mouseButton.button == sf::Mouse::Button::Right)
	{
		printGrid();
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
	UpdateGridSquareVisual();
}

int& SampleScene::GetCell(int x, int y)
{
	for (int i=0; i<grid.size(); i++)
	{
		if (grid[i].x == x && grid[i].y == y)
		{
			return grid[i].value;
		}
	}
}


void SampleScene::SetCell(int x, int y, int value)
{
	for (int i=0; i<grid.size(); i++)
	{
		if (grid[i].x == x && grid[i].y == y)
		{
			grid[i].value = value;
		}
	}
	
}

void SampleScene::printGrid() const
{
	int LastY = 0;
	for (int i = 0; i < grid.size(); i++)
	{
			switch (grid[i].value)
			{
			case -1: std::cout << '#'; break; // mur
			case 0:  std::cout << '.'; break; // vide
			case 1:  std::cout << 'A'; break; // joueur 1
			case 2:  std::cout << 'B'; break; // joueur 2
			default: std::cout << '?'; break;
			}
		if (LastY != grid[i].y)
		{
			std::cout << '\n';
			LastY = grid[i].y;
		}
		
	}
}

void SampleScene::SetAllValueCellToWall(int value)
{
	for (Case& cell : grid)
	{
		if (cell.value == value)
		{
			cell.value = -1;
		}
	}
}



void SampleScene::UpdateGridSquareVisual()
{
	constexpr float TILE_SIZE = 10.f;
	mGridSquares.clear();
	for (int x = 0; x <= GRID_WIDTH-1; x++)
	{
		for (int y = 0; y <= GRID_HEIGHT-1; y++)
		{
			if (GetCell(x,y) == -1)
			{
				sf::RectangleShape square;

				square.setSize(
					sf::Vector2f(TILE_SIZE,  TILE_SIZE)
				);

				square.setFillColor(sf::Color::White);

				square.setPosition(x * TILE_SIZE + TILE_SIZE * 0.5f,y * TILE_SIZE + TILE_SIZE * 0.5f);
				mGridSquares.push_back(square);
			}
			else if (GetCell(x,y) == 1)
			{
				sf::RectangleShape square;

				square.setSize(
					sf::Vector2f(TILE_SIZE,  TILE_SIZE)
				);

				square.setFillColor(sf::Color::Green);

				square.setPosition(x * TILE_SIZE + TILE_SIZE * 0.5f,y * TILE_SIZE + TILE_SIZE * 0.5f);
				mGridSquares.push_back(square);
			}
			else if (GetCell(x,y) == 2)
			{
				sf::RectangleShape square;

				square.setSize(
					sf::Vector2f(TILE_SIZE,  TILE_SIZE)
				);

				square.setFillColor(sf::Color::Magenta);

				square.setPosition(x * TILE_SIZE + TILE_SIZE * 0.5f,y * TILE_SIZE + TILE_SIZE * 0.5f);
				mGridSquares.push_back(square);
			}
		}
	}
}

SampleScene::SampleScene()
{
	for (int i=0; i < GRID_HEIGHT; i++)
	{
		for (int j=0; j < GRID_WIDTH; j++)
		{
			Case c;

			c.x = j;
			c.y = i;
			if (i == GRID_HEIGHT-1 || j == GRID_WIDTH-1 || i == 0 || j == 0)
			{
				c.value =-1;
			} else
			{
				c.value = 0;
			}
			
			grid.push_back(c);
		}
	}
}

int SampleScene::GetPlayerGridX()
{
	for (int i=0; i<grid.size(); i++)
	{
		if (grid[i].value == 1)
		{
			return grid[i].x;
		}
	}
}

int SampleScene::GetPlayerGridY()
{
	for (int i=0; i<grid.size(); i++)
	{
		if (grid[i].value == 1)
		{
			return grid[i].y;
		}
	}
}