#include "DummyEnemy.h"

#include "SampleScene.h"



void DummyEnemy::Inputs()
{
	float speed = 150.0f;
	sf::Vector2f direction = mLastDirection;
	
	if (mLastDirection == sf::Vector2f{0.f, 0.f})
	{
		
		mLastDirection = direction;
		SetDirection(direction.x, direction.y, speed);

		// Mur visuel uniquement
		pEntity = CreateEntity<Wall>(
			sf::Vector2f{20.0f, 20.0f},
			sf::Color::Red
		);

		float offset = 5.0f;

		mCubeStartPos = GetPosition(0.5f, 0.5f);

		mCubeStartPos.x -= mLastDirection.x * offset;
		mCubeStartPos.y -= mLastDirection.y * offset;

		mLastDirection = {1.f, 0.f};
		SetDirection(1.f, 0.f, speed);
		return;
	}

	int gridX = static_cast<int>(GetPosition().x / 10.0f);
	int gridY = static_cast<int>(GetPosition().y / 10.0f);

	auto IsFree = [&](int x, int y)
	{
		// Bord de la grille = mur
		if (x < 0 || x >= GetScene<SampleScene>()->GRID_WIDTH || y < 0 || y >= GetScene<SampleScene>()->GRID_HEIGHT)
			return false;

		return GetScene<SampleScene>()->GetCell(x, y) == 0;
	};

	int lookAhead = 3;

	bool danger = false;

	for (int i = 1; i <= lookAhead; i++)
	{
		int checkX = gridX + static_cast<int>(mLastDirection.x) * i;
		int checkY = gridY + static_cast<int>(mLastDirection.y) * i;

		if (!IsFree(checkX, checkY))
		{
			danger = true;
			break;
		}
	}

	

	if (danger)
	{
		std::vector<sf::Vector2f> choices;

		sf::Vector2f left{
			direction.y,
			-direction.x
		};

		sf::Vector2f right{
			-direction.y,
			direction.x
		};

		if (IsFree(
			gridX + static_cast<int>(left.x),
			gridY + static_cast<int>(left.y)))
		{
			choices.push_back(left);
		}

		if (IsFree(
			gridX + static_cast<int>(right.x),
			gridY + static_cast<int>(right.y)))
		{
			choices.push_back(right);
		}

		if (!choices.empty())
		{
			direction = choices[std::rand() % choices.size()];
		}
		else
		{
			// Coincé
			SetDirection(0.f, 0.f, 0.f);
			return;
		}
	}

	// Changement de direction
	if (direction != mLastDirection)
	{
		mLastDirection = direction;
		SetDirection(direction.x, direction.y, speed);

		// Mur visuel uniquement
		pEntity = CreateEntity<Wall>(
			sf::Vector2f{20.0f, 20.0f},
			sf::Color::Red
		);

		float offset = 5.0f;

		mCubeStartPos = GetPosition(0.5f, 0.5f);

		mCubeStartPos.x -= mLastDirection.x * offset;
		mCubeStartPos.y -= mLastDirection.y * offset;
	}
	GridUpdate(2);
}




