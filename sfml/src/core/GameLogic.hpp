#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP


#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include <SFML/Window.hpp>
#include <vector>
#include <utility>


using namespace model;


// Función para encontrar la celda de inicio
HexCell* findStartCell(HexGrid& grid);


// Función para encontrar la celda de meta
HexCell* findGoalCell(HexGrid& grid);


// Funciones de movimiento
void handlePlayerMovement(sf::Keyboard::Key key, Player& player, HexGrid& grid);
void handleConveyorMovement(Player& player, const HexGrid& grid);


// Funciones del sistema de energía y romper paredes MEJORADAS
void handleWallBreak(sf::Keyboard::Key key, Player& player, HexGrid& grid);
std::vector<std::pair<int, int>> findAdjacentWalls(const Player& player, const HexGrid& grid);


// NUEVAS FUNCIONES para selección direccional de paredes
std::pair<int, int> getDirectionalOffset(sf::Keyboard::Key key, int currentRow);
bool isValidWallBreakDirection(sf::Keyboard::Key key);
std::pair<int, int> getWallPositionInDirection(const Player& player, sf::Keyboard::Key direction, const HexGrid& grid);


#endif
