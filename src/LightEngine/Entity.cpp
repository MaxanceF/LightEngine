#include "Entity.h"

#include "GameManager.h"
#include "Utils.h"
#include "Debug.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/CircleShape.hpp>

void Entity::Initialize(sf::Vector2f size, const sf::Color& color)
{
	mDirection = sf::Vector2f(0.0f, 0.0f);

	mShape.setOrigin(0.f, 0.f);
	mShape.setSize(size);
	mShape.setFillColor(color);
	
	mTarget.isSet = false;

	OnInitialize();
}

void Entity::Repulse(Entity* other)
{
    // Récupère les bounds des deux entités
    sf::FloatRect bounds1 = mShape.getGlobalBounds();
    sf::FloatRect bounds2 = other->mShape.getGlobalBounds();

    // Calcule le chevauchement sur X et Y
    float overlapX = std::min(bounds1.left + bounds1.width, bounds2.left + bounds2.width) -
                     std::max(bounds1.left, bounds2.left);
    float overlapY = std::min(bounds1.top + bounds1.height, bounds2.top + bounds2.height) -
                     std::max(bounds1.top, bounds2.top);

    // Si pas de chevauchement, on sort
    if (overlapX <= 0 || overlapY <= 0)
        return;

    // Détermine la direction de la séparation (la plus petite distance)
    if (overlapX < overlapY)
    {
        // Séparation sur l'axe X
        float translationX = overlapX * 0.5f;
        if (bounds1.left < bounds2.left)
        {
            // Déplace bounds1 vers la gauche et bounds2 vers la droite
            SetPosition(GetPosition().x - translationX, GetPosition().y);
            other->SetPosition(other->GetPosition().x + translationX, other->GetPosition().y);
        }
        else
        {
            // Déplace bounds1 vers la droite et bounds2 vers la gauche
            SetPosition(GetPosition().x + translationX, GetPosition().y);
            other->SetPosition(other->GetPosition().x - translationX, other->GetPosition().y);
        }
    }
    else
    {
        // Séparation sur l'axe Y
        float translationY = overlapY * 0.5f;
        if (bounds1.top < bounds2.top)
        {
            // Déplace bounds1 vers le haut et bounds2 vers le bas
            SetPosition(GetPosition().x, GetPosition().y - translationY);
            other->SetPosition(other->GetPosition().x, other->GetPosition().y + translationY);
        }
        else
        {
            // Déplace bounds1 vers le bas et bounds2 vers le haut
            SetPosition(GetPosition().x, GetPosition().y + translationY);
            other->SetPosition(other->GetPosition().x, other->GetPosition().y - translationY);
        }
    }
}

bool Entity::IsColliding(Entity* other) const
{
	sf::Vector2f pos1 = GetPosition();
	sf::Vector2f pos2 = other->GetPosition();
	sf::Vector2f size1 = mShape.getSize();
	sf::Vector2f size2 = other->mShape.getSize();

	// Vérifie le chevauchement sur X et Y
	bool overlapX = (pos1.x < pos2.x + size2.x) && (pos1.x + size1.x > pos2.x);
	bool overlapY = (pos1.y < pos2.y + size2.y) && (pos1.y + size1.y > pos2.y);

	return overlapX && overlapY;
}

bool Entity::IsInside(float x, float y) const
{
	sf::Vector2f position = GetPosition(0.5f, 0.5f);

	float dx = x - position.x;
	float dy = y - position.y;
	

	return dx * dx < mShape.getSize().x ||  dy * dy < mShape.getSize().y;
}

void Entity::Destroy()
{
	mToDestroy = true;

	OnDestroy();
}

void Entity::SetPosition(float x, float y, float ratioX, float ratioY)
{

	x -= mShape.getSize().x * ratioX;
	y -= mShape.getSize().y * ratioY;

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
	
	sf::Vector2f position = mShape.getPosition();

	position.x += mShape.getSize().x * ratioX;
	position.y += mShape.getSize().y * ratioY;

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
	sf::Vector2f direction(x, y);
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (length > 0.0f)
	{
		direction /= length; // Normalisation
	}
	mDirection = direction;
	mTarget.isSet = false;
}

void Entity::Update()
{
	float dt = GetDeltaTime();
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