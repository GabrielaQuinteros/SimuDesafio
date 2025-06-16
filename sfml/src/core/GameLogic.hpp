#pragma once
//Permite manejar las teclas presionadas
#include <SFML/Window/Keyboard.hpp>
//Clase que representa el grid
#include "model/HexGrid.hpp"
//Clase que representa el jugador
#include "model/Player.hpp"

using namespace model;
using namespace sf;

// Busca la celda de inicio en el grid (tipo START)
//Devuelve un puntero hacia la celda de inicio si es que existe
HexCell *findStartCell(HexGrid &grid);

// Mueve al jugador si la tecla presionada corresponde a una dirección válida
void handlePlayerMovement(Keyboard::Key key, Player &player, const HexGrid &grid);

// Aplica el efecto de banda transportadora si la celda del jugador tiene una
void handleConveyorMovement(Player &player, const HexGrid &grid);
