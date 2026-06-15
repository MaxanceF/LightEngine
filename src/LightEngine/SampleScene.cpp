#include "SampleScene.h"

#include "Eater.h"

#include "Food.h"

#include "Debug.h"



void SampleScene::OnInitialize()
{
	pEntity1 = CreateEntity<Eater>(100, sf::Color::Red);
	pEntity1->SetPosition(100, 100);
	pEntity1->SetRigidBody(true);

	pEntity2 = CreateEntity<Eater>(50, sf::Color::Green);
	pEntity2->SetPosition(500, 500);
	pEntity2->SetRigidBody(true);

	pEntitySelected = nullptr;
	
	pFood1 = CreateEntity<Food>(10, sf::Color::Blue);
	pFood1->SetPosition(300, 300);
	pFood1->SetRigidBody(true);
	
	pSpike1 = CreateEntity<Spike>(25, sf::Color::Yellow);
	pSpike1->SetPosition(700, 400);
	pSpike1->SetRigidBody(true);
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
			pEntitySelected->MoveAllEaters(event.mouseButton.x, event.mouseButton.y, 100.f);
		}
	}
	
}

void SampleScene::TrySetSelectedEntity(Eater* pEntity, int x, int y)
{
	if (pEntity->IsInside(x, y) == false)
		return;

	pEntitySelected = pEntity;
}

bool m_wasSpacePressed = false;

void SampleScene::OnUpdate()
{
	if(pEntitySelected != nullptr)
	{
		sf::Vector2f position = pEntitySelected->GetPosition();
		Debug::DrawCircle(position.x, position.y, 10, sf::Color::Blue);
	}

	bool isSpacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
	if (isSpacePressed && !m_wasSpacePressed)
	{
		if (pEntitySelected != nullptr) 
		{
			pEntitySelected->Split();
		}
	}
	m_wasSpacePressed = isSpacePressed;
}