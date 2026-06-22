#pragma once
#include "vector"

namespace sf
{
	class RectangleShape;
	class RenderWindow;
}

class GameManager;

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Color.hpp>

class Scene
{
	static constexpr int GRID_WIDTH = 64;
	static constexpr int GRID_HEIGHT = 64;
private:
	GameManager* mpGameManager;

private:
	void SetGameManager(GameManager* pGameManager) { mpGameManager = pGameManager; }
	
protected:
	Scene() = default;

	virtual void OnInitialize() = 0;
	virtual void OnEvent(const sf::Event& event) = 0;
	virtual void OnUpdate() = 0;

	void CreateGridVisual();
	void DrawSquareGrid();
	std::vector<sf::RectangleShape> mGridLines;
	std::vector<sf::RectangleShape> mGridSquares;

public:
	template<typename T>
	T* CreateEntity(sf::Vector2f size, const sf::Color& color);

	float GetDeltaTime() const;

	int GetWindowWidth() const;
	int GetWindowHeight() const;
	sf::RenderWindow* GetRenderWindow() const;
	void DrawBGGrid();
	
	friend GameManager;
};

#include "Scene.inl"