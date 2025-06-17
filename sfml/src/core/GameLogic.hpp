#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include <SFML/Window.hpp>
#include <vector>
#include <utility>

using namespace model;

// Función para encontrar la celda de inicio
HexCell *findStartCell(HexGrid &grid);

// Funciones de movimiento
void handlePlayerMovement(sf::Keyboard::Key key, Player &player, HexGrid &grid);
void handleConveyorMovement(Player &player, const HexGrid &grid);

// Funciones del sistema de turnos y generación de muros
void handleTurnSystem(HexGrid &grid, const Player &player);
void generateRandomWall(HexGrid &grid, const Player &player);
std::vector<std::pair<int, int>> findAvailableCells(const HexGrid &grid, const Player &player);

// Funciones auxiliares para el manejo de turnos
int getCurrentTurnCount();
void resetTurnCounter();

#endif // GAMELOGIC_HPP