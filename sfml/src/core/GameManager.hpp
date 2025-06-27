#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../model/HexGrid.hpp"
#include "../model/HexCell.hpp"
#include "../model/Player.hpp"

using namespace model;


bool isPathStillValid(const HexGrid& grid, const std::vector<std::pair<int, int>>& pathCells,
                     int playerRow, int playerCol);

bool recalculatePath(HexGrid& grid, Player& player, HexCell* goal,
                    std::vector<std::pair<int, int>>& pathCells,
                    bool& showPathVisualization, bool& autoSolveMode);

bool loadSelectedMap(const std::string& mapPath, HexGrid*& grid, HexCell*& start, HexCell*& goal, Player*& player);

#endif 