#include "AutoMovement.hpp"
#include "../core/TurnSystem.hpp"
#include "../core/GameLogic.hpp"
#include "../core/PathFinding.hpp"
#include <SFML/Window.hpp>

using namespace model;
using namespace sf;

namespace core {

static Keyboard::Key keyFromDelta(int dRow, int dCol, int curRow) {
    bool odd = (curRow % 2) != 0;
    if (dRow == -1 && dCol == (odd ? 0 : -1)) return Keyboard::W;
    if (dRow == -1 && dCol == (odd ? 1 : 0)) return Keyboard::E;
    if (dRow ==  0 && dCol == -1)              return Keyboard::A;
    if (dRow ==  0 && dCol ==  1)              return Keyboard::D;
    if (dRow ==  1 && dCol == (odd ? 1 : 0))   return Keyboard::X;
    if (dRow ==  1 && dCol == (odd ? 0 : -1))  return Keyboard::Z;
    return Keyboard::A;
}

bool recalculatePathInAutoMovement(
    HexGrid& grid,
    Player& player,
    std::vector<std::pair<int, int>>& pathCells,
    int goalRow,
    int goalCol
) {
    PathfindingResult newPath = findPath(grid, player.row, player.col, goalRow, goalCol, player.energy);
    
    if (newPath.success && !newPath.path.empty()) {
        pathCells.clear();
        for (auto* cell : newPath.path) {
            pathCells.emplace_back(cell->row, cell->col);
        }
        return true;
    }
    return false;
}

void updateAutoMovement(
    HexGrid& grid,
    Player& player,
    std::vector<std::pair<int,int>>& pathCells,
    int goalRow,
    int goalCol
) {
    if (!player.isAutoMoving || pathCells.empty()) {
        player.isAutoMoving = false;
        return;
    }

    if (player.isMoving) return;

    int currentIndex = -1;
    for (size_t i = 0; i < pathCells.size(); ++i) {
        if (pathCells[i].first == player.row && pathCells[i].second == player.col) {
            currentIndex = static_cast<int>(i);
            break;
        }
    }

    if (currentIndex == -1) {
        currentIndex = 0;
    } else {
        currentIndex++;
    }

    if (currentIndex >= static_cast<int>(pathCells.size())) {
        HexCell* goalCell = &grid.at(goalRow, goalCol);
        if (player.row == goalRow && player.col == goalCol && goalCell->type == CellType::GOAL) {
            player.isAutoMoving = false;
            
            if (!player.hasWon) {
                if (grid.at(player.row, player.col).type == CellType::GOAL) {
                    player.hasWon = true;
                    player.winTime = player.winClock.getElapsedTime().asSeconds();
                }
            }
        } else {
            player.isAutoMoving = false;
        }
        return;
    }

    int nextR = pathCells[currentIndex].first;
    int nextC = pathCells[currentIndex].second;

    if (nextR == goalRow && nextC == goalCol && grid.at(nextR, nextC).type == CellType::GOAL) {
        int dR = nextR - player.row;
        int dC = nextC - player.col;
        Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
        
        handlePlayerMovement(dir, player, grid);
        
        if (player.hasWon) {
            player.isAutoMoving = false;
        } else {
            if (player.row == goalRow && player.col == goalCol) {
            }
        }
        return;
    }

    if (!grid.inBounds(nextR, nextC)) {
        if (!recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            player.isAutoMoving = false;
        }
        return;
    }

    if (grid.at(nextR, nextC).type == CellType::WALL) {
        if (player.energy >= TurnSystem::ENERGY_PER_WALL_BREAK) {
            grid.at(nextR, nextC).type = CellType::EMPTY;
            player.energy -= TurnSystem::ENERGY_PER_WALL_BREAK;
            
            int dR = nextR - player.row;
            int dC = nextC - player.col;
            Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
            handlePlayerMovement(dir, player, grid);
        } else {
            if (recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            } else {
                player.isAutoMoving = false;
            }
        }
        return;
    }

    if (grid.at(nextR, nextC).type == CellType::WALL) {
        if (!recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            player.isAutoMoving = false;
        }
        return;
    }

    int dR = nextR - player.row;
    int dC = nextC - player.col;
    
    if (abs(dR) > 1 || abs(dC) > 1) {
        if (!recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            player.isAutoMoving = false;
        }
        return;
    }
    
    Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
    handlePlayerMovement(dir, player, grid);
}

}