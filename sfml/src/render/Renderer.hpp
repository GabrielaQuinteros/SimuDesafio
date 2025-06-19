#pragma once

#include <SFML/Graphics.hpp>
#include "model/HexGrid.hpp"
#include "model/Player.hpp"

// Crea y configura un hexágono base
sf::CircleShape createHexagon();

// Función para interpolación de colores
sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t);

// Dibuja el fondo animado con efectos avanzados
void drawAnimatedBackground(sf::RenderWindow& window, sf::Clock& bgClock);

// Dibuja el título del juego con efectos especiales
void drawGameTitle(sf::RenderWindow& window, sf::Font& font, sf::Clock& animClock);

// Dibuja la barra de energía ultra moderna con efectos avanzados
void drawModernEnergyBar(sf::RenderWindow& window, const model::Player& player, sf::Font& font, sf::Clock& animClock);

// Dibuja información del juego con estilo premium
void drawGameInfo(sf::RenderWindow& window, sf::Font& font, int turnCount);

// Dibuja controles con estilo futurista
void drawModernControls(sf::RenderWindow& window, sf::Font& font);

// Obtiene el color de una celda según su tipo
sf::Color getCellColor(model::CellType type, sf::Clock& animClock);

// Dibuja el grid completo con efectos modernos y profesionales
void drawGrid(sf::RenderWindow& window, const model::HexGrid& grid,
    model::Player& player, sf::CircleShape& hexagon,
    sf::Text& texto, sf::Font& font, sf::Clock& animClock, sf::Clock& bgClock);