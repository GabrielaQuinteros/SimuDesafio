// Utils.hpp

#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>

sf::Text createText(
    sf::Font& font,
    int fontSize = 16, // Argumento predeterminado definido aqu�
    sf::Color color = sf::Color::Black
);

#endif // UTILS_HPP