#pragma once
#include "Entity.h"
#include <Vector>

class Eater : public Entity
{
public:
    Eater();
    void OnCollision(Entity* other) override;
    std::vector<Eater*> Eaters;
    bool IsHisOwnEater(Eater* other);
    void MoveAllEaters(int x,int y,float BaseSpeed);
    void Split();

private:
    void Eat(Entity* other);
    
    
};
