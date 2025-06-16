#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>
#include <utility>
#include "model/HexCell.hpp"

// Crea un objeto sf::Text listo para usar en pantalla
sf::Text createText(
    sf::Font &font,
    int fontSize = 16,
    sf::Color color = sf::Color::Black);

// Devuelve el desplazamiento (dy, dx) causado por una banda transportadora
// Corregido para direcciones hexagonales apropiadas
std::pair<int, int> getConveyorOffset(model::CellType type, bool isOddRow);

#endif