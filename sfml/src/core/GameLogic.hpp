#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include <SFML/Window.hpp>

using namespace model;

// Función para encontrar la celda de inicio
HexCell *findStartCell(HexGrid &grid);

// Funciones de movimiento
void handlePlayerMovement(sf::Keyboard::Key key, Player &player, HexGrid &grid);
void handleConveyorMovement(Player &player, const HexGrid &grid);

#endif 