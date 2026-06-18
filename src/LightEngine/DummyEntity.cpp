#include "DummyEntity.h"
#include "Wall.h"
#include "Debug.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include "SampleScene.h"

DummyEntity::DummyEntity()
{
	GetScene<SampleScene>()->SetCell(0, 0, 1);
	mInvincibilityTime = 2.0f;
	mIsInvincible = true;
}

void DummyEntity::OnCollision(Entity* other)
{
	
}

void DummyEntity::Crash()
{
	if (!mIsInvincible)
	{
		this->Destroy();
	}
}

void DummyEntity::Inputs()
{
	constexpr float speed = 150.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		mWantedDirection = {-1.f, 0.f};
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		mWantedDirection = {1.f, 0.f};
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		mWantedDirection = {0.f, -1.f};
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		mWantedDirection = {0.f, 1.f};
	}

	if (!CanTurnNow())
	{
		GridUpdate(1);
		return;
	}

	bool opposite =
		mWantedDirection.x == -mLastDirection.x &&
		mWantedDirection.y == -mLastDirection.y;

	if (mWantedDirection != sf::Vector2f{0.f, 0.f} &&
		!opposite &&
		mWantedDirection != mLastDirection)
	{
		mLastDirection = mWantedDirection;

		SetDirection(
			mLastDirection.x,
			mLastDirection.y,
			speed
		);

		pEntity = CreateEntity<Wall>(
			sf::Vector2f{20.f, 20.f},
			sf::Color::Cyan
		);

		mCubeStartPos = GetPosition(0.5f, 0.5f);
		constexpr float TILE_SIZE = 10.f;

		mCubeStartPos.x =
			std::round(mCubeStartPos.x / TILE_SIZE) * TILE_SIZE;

		mCubeStartPos.y =
			std::round(mCubeStartPos.y / TILE_SIZE) * TILE_SIZE;
	}

	GridUpdate(1);
}

void DummyEntity::OnUpdate()
{
	Inputs();
	constexpr float TILE_SIZE = 10.0f;

	if (pEntity)
	{
		// Position du joueur convertie en coordonnées de grille
		int gridX = static_cast<int>((GetPosition().x - 5) / TILE_SIZE);
		int gridY = static_cast<int>((GetPosition().y - 5)/ TILE_SIZE);

			sf::Vector2f playerPos(
				gridX * TILE_SIZE + TILE_SIZE * 0.5f,
				gridY * TILE_SIZE + TILE_SIZE * 0.5f
			);

		sf::Vector2f trailPos = playerPos;

		// L'extrémité de la traînée reste sur la grille
		trailPos.x -= mLastDirection.x * (TILE_SIZE * 0.5f);
		trailPos.y -= mLastDirection.y * (TILE_SIZE * 0.5f);

		// On aligne également le point de départ
		sf::Vector2f startPos(
			std::round(mCubeStartPos.x / TILE_SIZE) * TILE_SIZE + TILE_SIZE * 0.5f,
			std::round(mCubeStartPos.y / TILE_SIZE) * TILE_SIZE + TILE_SIZE * 0.5f
		);

		float distanceX = trailPos.x - startPos.x;
		float distanceY = trailPos.y - startPos.y;

		if (mLastDirection.x != 0.0f)
		{
			float width = std::abs(distanceX);

			// Évite une taille nulle
			width = std::max(width, TILE_SIZE);

			pEntity->SetSize({width, TILE_SIZE});

			float centerX = startPos.x + distanceX * 0.5f;
			centerX = std::round(centerX);

			pEntity->SetPosition(
				centerX - TILE_SIZE * 0.5f,
				startPos.y - TILE_SIZE * 0.5f
			);
		}
		else if (mLastDirection.y != 0.0f)
		{
			float height = std::abs(distanceY);

			// Évite une taille nulle
			height = std::max(height, TILE_SIZE);

			pEntity->SetSize({TILE_SIZE, height});

			float centerY = startPos.y + distanceY * 0.5f;
			centerY = std::round(centerY);

			pEntity->SetPosition(
				startPos.x - TILE_SIZE * 0.5f,
				centerY - TILE_SIZE * 0.5f
			);
		}
	}

	if (mIsInvincible)
	{
		mInvincibilityTime -= GetDeltaTime();

		if (mInvincibilityTime <= 0.0f)
		{
			mInvincibilityTime = 0.0f;
			mIsInvincible = false;
		}
	}
	


	
}

void DummyEntity::GridUpdate(int PlayerNum)
{
	if (GetPosition().x/10 < GetScene<SampleScene>()->GRID_WIDTH && GetPosition().x/10 >= 0 && GetPosition().y/10 < GetScene<SampleScene>()->GRID_HEIGHT && GetPosition().y/10 >= 0)
	{
		if (GetScene<SampleScene>()->GetCell(GetPosition().x/10,GetPosition().y/10) == 0)
		{
			GetScene<SampleScene>()->SetAllValueCellToWall(PlayerNum);
			GetScene<SampleScene>()->SetCell(GetPosition().x/10,GetPosition().y/10, PlayerNum);
			//GetScene<SampleScene>()->printGrid();
			//std::cout<< '\n' << std::endl;
		}
		else if (GetScene<SampleScene>()->GetCell(GetPosition().x/10,GetPosition().y/10) == -1)
		{
			Crash();
		}
	}
	else
	{
		Crash();
	}
}

bool DummyEntity::CanTurnNow() const
{
	constexpr float TILE_SIZE = 10.f;
	constexpr float TOLERANCE = 1.f;

	float centerX =
		std::round(GetPosition().x / TILE_SIZE) * TILE_SIZE;

	float centerY =
		std::round(GetPosition().y / TILE_SIZE) * TILE_SIZE;

	return
		std::abs(GetPosition().x - centerX) < TOLERANCE &&
		std::abs(GetPosition().y - centerY) < TOLERANCE;
}