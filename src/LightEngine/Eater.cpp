#include "Eater.h"

#include <iostream>
#include <ostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Food.h"
#include "SampleScene.h"
#include "Spike.h"


Eater::Eater()
{
    Eaters.push_back(this);
}

void Eater::OnCollision(Entity* other)
{
    if (dynamic_cast<Food*>(other))
    {
        Eat(other);
    }
    if (dynamic_cast<Eater*>(other) && !other->IsTag(this->mTag) && this->GetRadius()>other->GetRadius())
    {
        Eat(other);
    }
    if (dynamic_cast<Spike*>(other) && this->GetRadius()>other->GetRadius())
    {
        Split();
        other->Destroy();
    }
    
}

bool Eater::IsHisOwnEater(Eater* other)
{
    for (Eater* oEater : Eaters)
    {
        if(other == oEater)
        {
            return true;
        }
    }
    return false;
}

void Eater::Eat(Entity* other)
{
    SetRadius(this->GetRadius()+other->GetRadius());
    other->Destroy();
}

void Eater::Split()
{
    int Size = Eaters.size();
    for (size_t i = 0; i < Size; ++i)
    {
        Eater* oEater = Eaters[i];
        if ( oEater != nullptr && oEater->GetRadius() > 20)
        {
            float newRadius = oEater->GetRadius() / 2.0f;
            Eater* newEater = CreateEntity<Eater>(newRadius, oEater->GetShape()->getFillColor());
            newEater->SetRigidBody(true);

            sf::Vector2i mousePos = sf::Mouse::getPosition(*GetScene()->GetRenderWindow());
            sf::Vector2f vec(mousePos.x- oEater->GetPosition().x, mousePos.y - oEater->GetPosition().y);
            float length = std::sqrt(vec.x * vec.x + vec.y * vec.y);
            if (length != 0)
            {
                vec.x /= length;
                vec.y /= length;
            }
            std::cout<<std::to_string(vec.x) + " " +std::to_string(vec.y) <<std::endl;
            newEater->SetPosition(oEater->GetPosition().x, oEater->GetPosition().y);
            
            newEater->GoToPosition(oEater->GetPosition().x + vec.x*150, oEater->GetPosition().y + vec.y*150, 300);
            newEater->SetTag(oEater->mTag);
            
            Eaters.push_back(newEater);

            oEater->SetRadius(newRadius);
        }
    }
}

void Eater::MoveAllEaters(int x, int y, float BaseSpeed)
{
    for(Eater* oEater : Eaters)
    {
        oEater->GoToPosition(x,y,BaseSpeed/(oEater->GetRadius()/75));
    }
}

void Eater::SplitAll()
{
    Split();
}