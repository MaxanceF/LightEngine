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
	float speed = 150.0f;
	sf::Vector2f direction = {0.0f, 0.0f};

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		SetDirection(0.0f, 0.0f, 0.0f); // Arrête le mouvement
		mLastDirection = {0.0f, 0.0f}; // Réinitialise la dernière direction
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		direction = {-1.0f, 0.0f}; // Gauche
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		direction = {1.0f, 0.0f}; // Droite
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		direction = {0.0f, -1.0f}; // Haut
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		direction = {0.0f, 1.0f}; // Bas
	}

	if ((direction.x != 0.0f || direction.y != 0.0f) && direction != mLastDirection)
	{
		SetDirection(direction.x, direction.y, speed);
		mLastDirection = direction;
		sf::Color randomColor(
			std::rand() % 256,  // R (0-255)
			std::rand() % 256,  // G (0-255)
			std::rand() % 256   // B (0-255)
		);
		pEntity = CreateEntity<Wall>(sf::Vector2f{20.0f, 20.0f}, sf::Color::Cyan);
		float offset = 5.0f;

		mCubeStartPos = GetPosition(0.5f, 0.5f);

		mCubeStartPos.x -= mLastDirection.x * offset;
		mCubeStartPos.y -= mLastDirection.y * offset;

		//pEntity->SetPosition(mCubeStartPos.x, mCubeStartPos.y);
		//std::cout << GetPosition().x << " " << GetPosition().y << std::endl;
		
	}
	GridUpdate(1);
}

void DummyEntity::OnUpdate()
{
	Inputs();
	if (pEntity)
	{
		float trailOffset = 5.0f; // longueur de la traînée derrière le joueur

		sf::Vector2f playerPos = GetPosition(0.5f, 0.5f);
		sf::Vector2f trailPos = playerPos;

		// Décale l'extrémité du stretch derrière le joueur
		trailPos.x -= mLastDirection.x * trailOffset;
		trailPos.y -= mLastDirection.y * trailOffset;

		float distanceX = trailPos.x - mCubeStartPos.x;
		float distanceY = trailPos.y - mCubeStartPos.y;

		if (mLastDirection.x != 0.0f)
		{
			float width = std::abs(distanceX);

			pEntity->SetSize({width, 10.0f});

			float centerX = mCubeStartPos.x + distanceX * 0.5f;
			pEntity->SetPosition(centerX, mCubeStartPos.y);
		}
		else if (mLastDirection.y != 0.0f)
		{
			float height = std::abs(distanceY);

			pEntity->SetSize({10.0f, height});

			float centerY = mCubeStartPos.y + distanceY * 0.5f;
			pEntity->SetPosition(mCubeStartPos.x, centerY);
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
