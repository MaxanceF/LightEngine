#include "Entity.h"

#include "GameManager.h"
#include "Utils.h"
#include "Debug.h"
#include <iostream>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/CircleShape.hpp>

void Entity::Initialize(int width, int height, const sf::Color& color)
{
	mDirection = sf::Vector2f(0.0f, 0.0f);

	mShape.setOrigin(0.f, 0.f);
	mShape.setSize(sf::Vector2f(width, height));
	mShape.setFillColor(color);
	
	mTarget.isSet = false;

	OnInitialize();
}

static void SetRepulseTranslation(Entity* pe1, Entity* pe2, float transX, float transY) 
{
	if (pe1->IsKinematic()) 
	{
		sf::Vector2f center2 = pe2->GetPosition(0.5f, 0.5f);
		pe2->SetPosition(center2.x - transX, center2.y - transY * 2, 0.5f, 0.5f);
	}
	else if (pe2->IsKinematic()) 
	{
		sf::Vector2f center1 = pe1->GetPosition(0.5f, 0.5f);
		pe1->SetPosition(center1.x + transX, center1.y + transY * 2, 0.5f, 0.5f);
	}
	else
	{
		sf::Vector2f center1 = pe1->GetPosition(0.5f, 0.5f);
		sf::Vector2f center2 = pe2->GetPosition(0.5f, 0.5f);
		pe1->SetPosition(center1.x + transX, center1.y + transY, 0.5f, 0.5f);
		pe2->SetPosition(center2.x - transX, center2.y - transY, 0.5f, 0.5f);
	}
}

void Entity::Repulse(Entity* other) 
{
	// Obtenir les positions des centres des rectangles
	sf::Vector2f center1 = GetPosition(0.5f, 0.5f);
	sf::Vector2f center2 = other->GetPosition(0.5f, 0.5f);

	// Calculer la distance entre les centres
	sf::Vector2f distance = center1 - center2;

	// Obtenir les demi-tailles des rectangles
	sf::Vector2f halfSize1 = GetSize() * 0.5f;
	sf::Vector2f halfSize2 = other->GetSize() * 0.5f;

	// Calculer l'overlap sur chaque axe
	float overlapX = (halfSize1.x + halfSize2.x) - std::abs(distance.x);
	float overlapY = (halfSize1.y + halfSize2.y) - std::abs(distance.y);

	// Détermination de l'axe de séparation le plus faible
	if (overlapX < overlapY)
	{
		float translationX = overlapX * 0.5f * (distance.x > 0 ? 1 : -1);
		SetRepulseTranslation(this, other, translationX, 0.f);
	}
	else
	{
		float translationY = overlapY * 0.5f * (distance.y > 0 ? 1 : -1);
		SetRepulseTranslation(this, other, 0.f, translationY);
	}

	//if collides with bootom set gravoty to 0
	if (center1.y < center2.y || center2.y < center1.y)
	{
		mGravitySpeed = 0.f;
		other->mGravitySpeed = 0.f;
		std::cout << "gravity 0" << std::endl;
	}
}

bool Entity::IsColliding(Entity* other) const
{
	sf::Vector2 pos1 = GetPosition(0.f, 0.f);
	sf::Vector2 pos2 = other->GetPosition(0.f, 0.f);
	sf::Vector2 size1 = GetSize();
	sf::Vector2 size2 = other->GetSize();

	if (pos1.x >= pos2.x + size2.x)
		return false;

	if (pos1.x + size1.x <= pos2.x)
		return false;

	if (pos1.y >= pos2.y + size2.y)
		return false;

	if (pos1.y + size1.y <= pos2.y)
		return false;

	return true;
}

bool Entity::IsInside(float x, float y) const
{
	sf::Vector2f posTopLeft = GetPosition(0.f, 0.f);
	sf::Vector2f posBottomRight = GetPosition(1.f, 1.f);

	if (x < posTopLeft.x)
		return false;

	if (x > posBottomRight.x)
		return false;

	if (y < posTopLeft.y)
		return false;

	if (y > posBottomRight.y)
		return false;

	return true;
}

void Entity::Destroy()
{
	mToDestroy = true;

	OnDestroy();
}

void Entity::SetPosition(float x, float y, float ratioX, float ratioY)
{
	sf::Vector2f size = GetSize();

	x -= size.x * ratioX;
	y -= size.y * ratioY;

	mShape.setPosition(x, y);

	//#TODO Optimise
	if (mTarget.isSet) 
	{
		sf::Vector2f position = GetPosition(0.5f, 0.5f);
		mTarget.distance = Utils::GetDistance(position.x, position.y, mTarget.position.x, mTarget.position.y);
		GoToDirection(mTarget.position.x, mTarget.position.y);
		mTarget.isSet = true;
	}
}

sf::Vector2f Entity::GetPosition(float ratioX, float ratioY) const
{
	sf::Vector2f size = GetSize();
	sf::Vector2f position = mShape.getPosition();

	position.x += size.x * ratioX;
	position.y += size.y * ratioY;

	return position;
}

bool Entity::GoToDirection(int x, int y, float speed)
{
	sf::Vector2f position = GetPosition(0.5f, 0.5f);
	sf::Vector2f direction = sf::Vector2f(x - position.x, y - position.y);
	
	bool success = Utils::Normalize(direction);
	if (success == false)
		return false;

	SetDirection(direction.x, direction.y, speed);

	return true;
}

bool Entity::GoToPosition(int x, int y, float speed)
{
	if (GoToDirection(x, y, speed) == false)
		return false;

	sf::Vector2f position = GetPosition(0.5f, 0.5f);

	mTarget.position = { x, y };
	mTarget.distance = Utils::GetDistance(position.x, position.y, x, y);
	mTarget.isSet = true;

	return true;
}

void Entity::SetDirection(float x, float y, float speed)
{
	if (speed > 0)
		mSpeed = speed;

	mDirection = sf::Vector2f(x, y);
	Utils::Normalize(mDirection);
	mTarget.isSet = false;
}

void Entity::Update()
{
	float dt = GetDeltaTime();

	if (mGravity)
	{
		float k = 980.f;
		mGravitySpeed += k * dt;

		sf::Vector2f translation;
		translation.x = 0.f;
		translation.y = mGravitySpeed * dt;

		mShape.move(translation);
	}

	float distance = dt * mSpeed;
	sf::Vector2f translation = distance * mDirection;

	mShape.move(translation);

	if (mTarget.isSet) 
	{
		float x1 = GetPosition(0.5f, 0.5f).x;
		float y1 = GetPosition(0.5f, 0.5f).y;

		float x2 = x1 + mDirection.x * mTarget.distance;
		float y2 = y1 + mDirection.y * mTarget.distance;

		Debug::DrawLine(x1, y1, x2, y2, sf::Color::Cyan);

		Debug::DrawCircle(mTarget.position.x, mTarget.position.y, 5.f, sf::Color::Magenta);

		mTarget.distance -= distance;

		if (mTarget.distance <= 0.f)
		{
			SetPosition(mTarget.position.x, mTarget.position.y, 0.5f, 0.5f);
			mDirection = sf::Vector2f(0.f, 0.f);
			mTarget.isSet = false;
		}
	}

	OnUpdate();
}

Scene* Entity::GetScene() const
{
	return GameManager::Get()->GetScene();
}

float Entity::GetDeltaTime() const
{
	return GameManager::Get()->GetDeltaTime();
}