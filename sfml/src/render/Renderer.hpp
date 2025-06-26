#pragma once

#include <SFML/Graphics.hpp>
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include "core/PathFinding.hpp"
#include <vector>

sf::CircleShape createHexagon();

sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t);

void drawVictoryScreen(sf::RenderWindow& window, sf::Font& font, float winTime, int turnCount, sf::Clock& animClock);

void drawAnimatedBackground(sf::RenderWindow& window, sf::Clock& bgClock);

void drawGameTitle(sf::RenderWindow& window, sf::Font& font, sf::Clock& animClock);

void drawModernEnergyBar(sf::RenderWindow& window, const model::Player& player, sf::Font& font, sf::Clock& animClock);

void drawGameInfo(sf::RenderWindow& window, sf::Font& font, int turnCount, sf::Clock& animClock,
                 bool showPathVisualization = false, bool autoSolveMode = false);

void drawModernControls(sf::RenderWindow& window, sf::Font& font, sf::Clock& animClock);

sf::Color getCellColor(model::CellType type, sf::Clock& animClock);

void drawGrid(sf::RenderWindow& window, const model::HexGrid& grid,
    model::Player& player, sf::CircleShape& hexagon,
    sf::Text& texto, sf::Font& font, sf::Clock& animClock, sf::Clock& bgClock,
    std::vector<std::pair<int, int>>& path);