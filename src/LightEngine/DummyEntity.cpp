#include "DummyEntity.h"
#include "Wall.h"
#include "Debug.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include "SampleScene.h"

DummyEntity::DummyEntity()
{
	mInvincibilityTime = 2.0f;
	mIsInvincible = true;
}

void DummyEntity::OnCollision(Entity* other)
{
	if (other == pEntity)
		return;
	if (dynamic_cast<Wall*>(other))
	{

		std::cout << std::to_string(other->GetPosition().x) + " " + std::to_string(other->GetPosition().y) + " & " + std::to_string(GetPosition().x) + " " + std::to_string(GetPosition().y)<< std::endl;
		Debug* debug = new Debug();
		debug->DrawRectangle(GetPosition().x, GetPosition().y, other->GetPosition().x - GetPosition().x, other->GetPosition().y - GetPosition().y, sf::Color::Red);
		if (!mIsInvincible)
		{
			//this->Destroy();
		}
	}
}

void DummyEntity::OnUpdate()
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
		pEntity = CreateEntity<Wall>(sf::Vector2f{20.0f, 20.0f}, randomColor);
		float offset = 10.0f;

		mCubeStartPos = GetPosition(0.5f, 0.5f);

		mCubeStartPos.x -= mLastDirection.x * offset;
		mCubeStartPos.y -= mLastDirection.y * offset;

		//pEntity->SetPosition(mCubeStartPos.x, mCubeStartPos.y);
	}
	
	if (pEntity)
	{
		float trailOffset = 10.0f; // longueur de la traînée derrière le joueur

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

			pEntity->SetSize({width, 20.0f});

			float centerX = mCubeStartPos.x + distanceX * 0.5f;
			pEntity->SetPosition(centerX, mCubeStartPos.y);
		}
		else if (mLastDirection.y != 0.0f)
		{
			float height = std::abs(distanceY);

			pEntity->SetSize({20.0f, height});

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


