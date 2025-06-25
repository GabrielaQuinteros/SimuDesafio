#pragma once
#include "model/HexCell.hpp" // para acceder a HexCell
#include "model/HexGrid.hpp" // para acceder a HexGrid
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
