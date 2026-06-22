#include "DummyEntity.h"
#include "Wall.h"

#include <SFML/Graphics.hpp>
#include <random>
#include "SampleScene.h"

DummyEntity::DummyEntity()
{
	mInvincibilityTime = 2.0f;
	mIsInvincible = true;

	mWantedDirection={-1, 0};
	mDirection={-1, 0};
}



void DummyEntity::Crash()
{
	if (!mIsInvincible)
	{
		GetScene<SampleScene>()->SetCell(
		mGridPos.x,
		mGridPos.y,
		0);
		this->Destroy();
	}
}

void DummyEntity::Inputs()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		mWantedDirection = {-1, 0};
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		mWantedDirection = {1, 0};
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		mWantedDirection = {0, -1};
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		mWantedDirection = {0, 1};
	}
}

void DummyEntity::OnUpdate()
{
	Inputs();

	mMoveTimer += GetDeltaTime();
	
	if (mMoveTimer < mMoveDelay)
	{
		return;
	}
	
	mMoveTimer -= mMoveDelay;

	// Vérifie qu'on ne fait pas demi-tour
	bool opposite =
		mWantedDirection.x == -mDirection.x &&
		mWantedDirection.y == -mDirection.y;

	if (!opposite)
	{
		mDirection = mWantedDirection;
	}

	int nextX = mGridPos.x + mDirection.x;
	int nextY = mGridPos.y + mDirection.y;

	int cellValue =
		GetScene<SampleScene>()->GetCell(nextX, nextY);

	if (cellValue != 0)
	{
		// Mur, trace ou joueur
		Crash();
		return;
	}

	GetScene<SampleScene>()->SetCell(
			mGridPos.x,
			mGridPos.y,
			-1
		);
	
	mGridPos.x = nextX;
	mGridPos.y = nextY;

	GetScene<SampleScene>()->SetCell(
		mGridPos.x,
		mGridPos.y,
		1
	);

	
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

void DummyEntity::SetMGridPos(int x, int y)
{
	mGridPos.x = x;
	mGridPos.y = y;
}