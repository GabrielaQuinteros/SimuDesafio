// Utils.cpp

#include "Utils.hpp"

sf::Text createText(
    sf::Font& font,
    int fontSize,
    sf::Color color
) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(fontSize);
    text.setFillColor(color);
    return text;
}