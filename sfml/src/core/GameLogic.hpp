#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP


#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include <SFML/Window.hpp>
#include <vector>
#include <utility>


using namespace model;

HexCell* findStartCell(HexGrid& grid);

HexCell* findGoalCell(HexGrid& grid);

void handlePlayerMovement(sf::Keyboard::Key key, Player& player, HexGrid& grid);
void handleConveyorMovement(Player& player, const HexGrid& grid);

void handleWallBreak(sf::Keyboard::Key key, Player& player, HexGrid& grid);
std::vector<std::pair<int, int>> findAdjacentWalls(const Player& player, const HexGrid& grid);

std::pair<int, int> getDirectionalOffset(sf::Keyboard::Key key, int currentRow);
bool isValidWallBreakDirection(sf::Keyboard::Key key);
std::pair<int, int> getWallPositionInDirection(const Player& player, sf::Keyboard::Key direction, const HexGrid& grid);


#endif
