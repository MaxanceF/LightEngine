#include "SampleScene.h"

#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "CircleEnemy.h"

#include "Debug.h"

#include <random>

int SampleScene::RandomNumber(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dist(min, max);

	return dist(gen);
}
void SampleScene::OnInitialize()
{
	// INIT FOOD
	for (int i = 0; i < 100; i++)
	{
		auto* circle = CreateEntity<CircleEntity>(
			RandomNumber(3, 6),
			sf::Color::Red
		);
		circle->SetPosition(RandomNumber(0,1280), RandomNumber(0,720));
		circle->SetTag(1);
		_entities.push_back(circle);
	}

	//INIT ENEMIES
	for (int i = 0; i < 5; i++)
	{
		auto* circle = CreateEntity<CircleEnemy>(
			RandomNumber(20, 25),
			sf::Color::Blue
		);
		circle->SetPosition(RandomNumber(0,1280), RandomNumber(0,720));
		circle->SetTag(2);
		_entities.push_back(circle);
	}

	// INIT PLAYER
	auto* circle = CreateEntity<CirclePlayer>(
		23,
		sf::Color::White
	);
	circle->SetPosition(RandomNumber(0,1280), RandomNumber(0,720));
	circle->SetTag(3);
	_entities.push_back(circle);

}

void SampleScene::OnEvent(const sf::Event& event)
{
	
}

void SampleScene::OnUpdate()
{
	float dt = GetDeltaTime();

	mFoodSpawnTimer += dt;
	
	if (mFoodSpawnTimer >= mFoodSpawnCooldown)
	{
		mFoodSpawnTimer = 0.f;

		auto* circle = CreateEntity<CircleEntity>(
			RandomNumber(3, 6),
			sf::Color::Red
		);

		circle->SetPosition(
			RandomNumber(0, 1280),
			RandomNumber(0, 720)
		);

		circle->SetTag(1);

		_entities.push_back(circle);
	}
}