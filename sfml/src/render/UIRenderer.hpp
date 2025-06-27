#ifndef UI_RENDERER_HPP
#define UI_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include "model/Player.hpp"

using namespace sf;
using namespace model;

void drawModernEnergyBar(RenderWindow& window, const Player& player, Font& font, Clock& animClock);

void drawGameInfo(RenderWindow& window, Font& font, int turnCount, Clock& animClock,
                 bool showPathVisualization, bool autoSolveMode);

void drawModernControls(RenderWindow& window, Font& font, Clock& animClock);

#endif