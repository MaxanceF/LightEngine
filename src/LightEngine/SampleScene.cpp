#include "SampleScene.h"

#include "DummyEntity.h"

#include "Debug.h"

void SampleScene::OnInitialize()
{
	pEntity1 = CreateEntity<DummyEntity>(100, 200, sf::Color::Red);
	pEntity1->SetPosition(100, 100);
	pEntity1->SetRigidBody(true);
	pEntity1->SetGravity(true);

	pEntity2 = CreateEntity<DummyEntity>(1000, 50, sf::Color::Green);
	pEntity2->SetPosition(500, 500);
	pEntity2->SetRigidBody(true);
	pEntity2->SetKinematic(true);

	DummyEntity* pEntity3;
	pEntity3 = CreateEntity<DummyEntity>(1000, 50, sf::Color::Green);
	pEntity3->SetPosition(1000, 200);
	pEntity3->SetRigidBody(true);
	pEntity3->SetKinematic(true);

	pEntitySelected = nullptr;
}

void SampleScene::OnEvent(const sf::Event& event)
{
	if (event.type != sf::Event::EventType::MouseButtonPressed)
		return;

	if (event.mouseButton.button == sf::Mouse::Button::Right)
	{
		TrySetSelectedEntity(pEntity1, event.mouseButton.x, event.mouseButton.y);
		TrySetSelectedEntity(pEntity2, event.mouseButton.x, event.mouseButton.y);
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

bool mAPressed = false;
bool mAlreadyPressed = false;

#include <iostream>
void SampleScene::OnUpdate()
{
	if(pEntitySelected != nullptr)
	{
		sf::Vector2f position = pEntitySelected->GetPosition();
		Debug::DrawCircle(position.x, position.y, 10, sf::Color::Blue);
	}

	float stickX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
	float stickY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

	if (std::abs(stickX) < 15)
		stickX = 0;

	pEntity1->SetDirection(stickX, 0, 200);

	mAPressed = sf::Joystick::isButtonPressed(0, 0);
	if (mAPressed == false)
		mAlreadyPressed = false;

	if (mAPressed && mAlreadyPressed == false)
	{
		pEntity1->mGravitySpeed = -500.f;
		std::cout << "A pressed" << std::endl;
		mAlreadyPressed = true;
	}
}