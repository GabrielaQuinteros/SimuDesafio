#pragma once
#include "model/HexCell.hpp" 
#include "model/HexGrid.hpp" 
#include <vector>


struct PathfindingResult {
    std::vector<model::HexCell*> path;
    bool success;
};

PathfindingResult findPath(
    model::HexGrid& grid,
    int startRow, int startCol,
    int goalRow, int goalCol,
    int initialEnergy
);
