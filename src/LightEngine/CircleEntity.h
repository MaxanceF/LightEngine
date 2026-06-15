#pragma once
#include "Entity.h"

class CircleEntity : public Entity
{
private:
	
public:
	void OnCollision(Entity* other) override;
	void OnInitialize();
};

