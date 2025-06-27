#ifndef SCREEN_RENDERER_HPP
#define SCREEN_RENDERER_HPP

#include <SFML/Graphics.hpp>

using namespace sf;

void drawVictoryScreen(RenderWindow& window, Font& font, float winTime, int turnCount, Clock& animClock);

void drawGameTitle(RenderWindow& window, Font& font, Clock& animClock);

void drawAnimatedBackground(RenderWindow& window, Clock& bgClock);

void drawIntroScreen(RenderWindow& window, Font& font, Clock& animClock);

#endif