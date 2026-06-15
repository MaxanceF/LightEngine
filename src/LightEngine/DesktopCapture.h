#pragma once
#include <SFML/Graphics.hpp>

namespace DesktopCapture
{
	sf::Image        Capture();
	void             MakeOverlay(sf::RenderWindow& overlay);
}