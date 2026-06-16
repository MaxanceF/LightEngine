#include "Scene.h"

#include "GameManager.h"

int Scene::GetWindowWidth() const
{
	return mpGameManager->mWindowWidth;
}

int Scene::GetWindowHeight() const
{
	return mpGameManager->mWindowHeight;
}

float Scene::GetDeltaTime() const
{
	return mpGameManager->mDeltaTime;
}
sf::RenderWindow* Scene::GetRenderWindow() const {
	return mpGameManager ? mpGameManager->GetWindow() : nullptr;
}
