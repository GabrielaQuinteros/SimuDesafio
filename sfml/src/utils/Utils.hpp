#ifndef UTILS_HPP
#define UTILS_HPP


#include <SFML/Graphics.hpp>
#include <utility>
#include "model/HexCell.hpp"

sf::Text createText(
    sf::Font &font,
    int fontSize = 16,
    sf::Color color = sf::Color::Black);

std::pair<int, int> getConveyorOffset(model::CellType type, bool isOddRow);


#endif
