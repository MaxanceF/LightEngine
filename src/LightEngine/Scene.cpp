#include "Scene.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "GameManager.h"


int Scene::GetWindowWidth() const
{
	return mpGameManager->mWindowWidth;
}

int Scene::GetWindowHeight() const
{
	return mpGameManager->mWindowHeight;
}

sf::RenderWindow* Scene::GetRenderWindow() const {
	return mpGameManager ? mpGameManager->GetWindow() : nullptr;
}

float Scene::GetDeltaTime() const
{
	return mpGameManager->mDeltaTime;
}

void Scene::CreateGridVisual()
{
	constexpr float TILE_SIZE = 10.f;

	sf::Color gridColor(50, 50, 50);


	// Lignes verticales
	for (int x = 0; x <= GRID_WIDTH; x++)
	{
		sf::RectangleShape line;

		line.setSize(
			sf::Vector2f(1.f, GRID_HEIGHT * TILE_SIZE)
		);

		line.setPosition(x * TILE_SIZE + TILE_SIZE * 0.5f,0);

		line.setFillColor(gridColor);

		mGridLines.push_back(line);
	}


	// Lignes horizontales
	for (int y = 0; y <= GRID_HEIGHT; y++)
	{
		sf::RectangleShape line;

		line.setSize(
			sf::Vector2f(GRID_WIDTH * TILE_SIZE, 1.f)
		);

		line.setPosition(0,y * TILE_SIZE + TILE_SIZE * 0.5f);

		line.setFillColor(gridColor);

		mGridLines.push_back(line);
	}
}

void Scene::DrawGrid(sf::RenderWindow* window)
{
	for(auto& line : mGridLines)
	{
		GetRenderWindow()->draw(line);
	}
}