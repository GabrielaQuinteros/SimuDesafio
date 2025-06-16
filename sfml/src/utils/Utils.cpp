#include "Utils.hpp"
#include "model/HexCell.hpp"
#include <iostream>

using namespace model;

// Crea texto básico con fuente, tamaño y color
sf::Text createText(
    sf::Font &font,
    int fontSize,
    sf::Color color)
{
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(fontSize);
    text.setFillColor(color);
    return text;
}

// Devuelve el desplazamiento que produce una banda transportadora
// Corregido para seguir correctamente las direcciones hexagonales
std::pair<int, int> getConveyorOffset(CellType type, bool isOddRow)
{
    switch (type)
    {
    case CellType::UP_RIGHT: // A - Arriba-Derecha
        return {-1, isOddRow ? 1 : 0};
    case CellType::RIGHT: // B - Derecha
        return {0, 1};
    case CellType::DOWN_RIGHT: // C - Abajo-Derecha
        return {1, isOddRow ? 1 : 0};
    case CellType::DOWN_LEFT: // D - Abajo-Izquierda
        return {1, isOddRow ? 0 : -1};
    case CellType::LEFT: // E - Izquierda
        return {0, -1};
    case CellType::UP_LEFT: // F - Arriba-Izquierda
        return {-1, isOddRow ? 0 : -1};
    default:
        return {0, 0}; // No movimiento automático
    }
}

