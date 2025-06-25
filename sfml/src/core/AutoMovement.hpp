#pragma once


#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include "../core/PathFinding.hpp"
#include "../core/TurnSystem.hpp"
#include <vector>


namespace core {


void updateAutoMovement(
    model::HexGrid& grid,
    model::Player& player,
    std::vector<std::pair<int,int>>& pathCells,
    int goalRow,
    int goalCol
);


}
