#pragma once


#include <SFML/Graphics.hpp>
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include "core/PathFinding.hpp"
#include <vector>


// Crea y configura un hexágono base
sf::CircleShape createHexagon();


// Función para interpolación de colores
sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t);


// NUEVA: Pantalla de victoria ultra profesional
void drawVictoryScreen(sf::RenderWindow& window, sf::Font& font, float winTime, int turnCount, sf::Clock& animClock);


// Dibuja el fondo animado con efectos ultra avanzados
void drawAnimatedBackground(sf::RenderWindow& window, sf::Clock& bgClock);


// Dibuja el título del juego con efectos épicos
void drawGameTitle(sf::RenderWindow& window, sf::Font& font, sf::Clock& animClock);


// Dibuja la barra de energía ultra moderna con efectos cinematográficos
void drawModernEnergyBar(sf::RenderWindow& window, const model::Player& player, sf::Font& font, sf::Clock& animClock);


// Dibuja información del juego con estilo ultra premium - ACTUALIZADA
void drawGameInfo(sf::RenderWindow& window, sf::Font& font, int turnCount, sf::Clock& animClock,
                 bool showPathVisualization = false, bool autoSolveMode = false);


// Dibuja controles con estilo futurista ultra avanzado
void drawModernControls(sf::RenderWindow& window, sf::Font& font, sf::Clock& animClock);


// Obtiene el color de una celda según su tipo con efectos mejorados
sf::Color getCellColor(model::CellType type, sf::Clock& animClock);


// Dibuja el grid completo con efectos ultra modernos y profesionales
void drawGrid(sf::RenderWindow& window, const model::HexGrid& grid,
    model::Player& player, sf::CircleShape& hexagon,
    sf::Text& texto, sf::Font& font, sf::Clock& animClock, sf::Clock& bgClock,
    std::vector<std::pair<int, int>>& path);
