#include "Eater.h"

#include "Food.h"
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
    if (dynamic_cast<Eater*>(other) && !IsHisOwnEater(dynamic_cast<Eater*>(other)) && this->GetRadius()>other->GetRadius())
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
    if (Eaters[0]->GetRadius()/2 > 10)
    {
        Eaters.push_back(CreateEntity<Eater>(Eaters[0]->GetRadius()/2, GetShape()->getFillColor()));
        Eaters[Eaters.size()-1]->SetRigidBody(true);
        Eaters[Eaters.size()-1]->SetPosition(Eaters[0]->GetPosition().x+1, Eaters[0]->GetPosition().y+1);
        Eaters[0]->SetRadius(Eaters[Eaters.size()-1]->GetRadius());
    } 
}

void Eater::MoveAllEaters(int x, int y, float BaseSpeed)
{
    for(Eater* oEater : Eaters)
    {
        oEater->GoToPosition(x,y,BaseSpeed/(oEater->GetRadius()/75));
    }
}