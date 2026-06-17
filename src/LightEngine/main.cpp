
#include <SFML/Graphics.hpp>
#include <iostream>

#include "GameManager.h"
#include "SampleScene.h"

#include <cstdlib>
#include <crtdbg.h>

int main() 
{
    GameManager* pInstance = GameManager::Get();

	pInstance->CreateWindow(640, 640, "SampleScene", 600, sf::Color::Black);
	
	pInstance->LaunchScene<SampleScene>();

	return 0;
}