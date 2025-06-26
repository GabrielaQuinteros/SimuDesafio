#ifndef AUTOMOVEMENT_HPP
#define AUTOMOVEMENT_HPP

#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include <vector>

namespace core {

/**
 * @brief 
 * 
 * @param grid 
 * @param player 
 * @param pathCells 
 * @param goalRow 
 * @param goalCol 
 */
void updateAutoMovement(
    model::HexGrid& grid,
    model::Player& player,
    std::vector<std::pair<int, int>>& pathCells,
    int goalRow,
    int goalCol
);

}

#endif 