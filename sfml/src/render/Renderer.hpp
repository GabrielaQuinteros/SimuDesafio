#pragma once

#include <SFML/Graphics.hpp>
#include "model/HexGrid.hpp"
#include "model/Player.hpp"

// Crea y configura un hexágono base
sf::CircleShape createHexagon();

// Dibuja el grid, los textos y el jugador con animación suave
void drawGrid(sf::RenderWindow &window, const model::HexGrid &grid,
              model::Player &player, sf::CircleShape &hexagon,
              sf::Text &texto);