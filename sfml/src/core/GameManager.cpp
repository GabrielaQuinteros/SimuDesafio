#include "GameManager.hpp"
#include "../utils/MapLoader.hpp"
#include "../utils/Utils.hpp"
#include "PathFinding.hpp"
#include "TurnSystem.hpp"
#include <iostream>

HexCell* findStartCell(HexGrid& grid);
HexCell* findGoalCell(HexGrid& grid);


bool isPathStillValid(const HexGrid& grid, const std::vector<std::pair<int, int>>& pathCells,
                     int playerRow, int playerCol) {
    if (pathCells.empty()) return false;
   
    bool playerInPath = false;
    for (size_t i = 0; i < pathCells.size(); ++i) {
        if (pathCells[i].first == playerRow && pathCells[i].second == playerCol) {
            playerInPath = true;
            break;
        }
    }
   
    if (!playerInPath && !pathCells.empty()) {
        const auto& firstStep = pathCells[0];
        int distance = abs(firstStep.first - playerRow) + abs(firstStep.second - playerCol);
        if (distance > 2) {
            return false; 
        }
    }
   
    for (const auto& cell : pathCells) {
        CellType cellType = grid.at(cell.first, cell.second).type;
        if (cellType == CellType::WALL) {
            return false;
        }
    }
   
    return true;
}

bool recalculatePath(HexGrid& grid, Player& player, HexCell* goal,
                    std::vector<std::pair<int, int>>& pathCells,
                    bool& showPathVisualization, bool& autoSolveMode) {
   
    std::cout << "Recalculando camino desde main..." << std::endl;
    PathfindingResult newPath = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
   
    if (newPath.success && !newPath.path.empty()) {
        pathCells.clear();
        for (auto *cell : newPath.path) {
            pathCells.emplace_back(cell->row, cell->col);
        }
       
        std::cout << "Camino recalculado exitosamente con " << pathCells.size() << " pasos." << std::endl;
        return true;
    } else {
        std::cout << "No se pudo recalcular el camino. Limpiando estado." << std::endl;
        pathCells.clear();
        showPathVisualization = false;
        autoSolveMode = false;
        player.isAutoMoving = false;
       
        return false;
    }
}

bool loadSelectedMap(const std::string& mapPath, HexGrid*& grid, HexCell*& start, HexCell*& goal, Player*& player) {
    try {
        std::cout << "Intentando cargar mapa: " << mapPath << std::endl;
        
        if (grid) {
            delete grid;
            grid = nullptr;
        }
        if (player) {
            delete player;
            player = nullptr;
        }
        
        HexGrid tempGrid = loadHexGridFromFile(mapPath);
        HexCell* tempStart = findStartCell(tempGrid);
        HexCell* tempGoal = findGoalCell(tempGrid);
        
        if (tempStart && tempGoal) {
            grid = new HexGrid(tempGrid);
            start = findStartCell(*grid);
            goal = findGoalCell(*grid);
            player = new Player(start->row, start->col);
            TurnSystem::resetTurnCounter();
            
            std::cout << "Mapa cargado exitosamente: " << mapPath << std::endl;
            std::cout << "Start: (" << start->row << ", " << start->col << ")" << std::endl;
            std::cout << "Goal: (" << goal->row << ", " << goal->col << ")" << std::endl;
            return true;
        } else {
            std::cout << "Error: Mapa no tiene start y/o goal válidos" << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Excepción al cargar mapa: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cout << "Error desconocido al cargar mapa" << std::endl;
        return false;
    }
}