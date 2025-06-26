#include "PathFinding.hpp"
#include "model/HexGrid.hpp"
#include "../utils/Utils.hpp"
#include <queue>
#include <tuple>
#include <map>
#include <set>
#include <algorithm>

constexpr int MAX_ENERGY = 10;

struct State {
    int row, col;
    int energy;
    int cost;

    bool operator>(const State& other) const {
        return cost > other.cost;
    }
};

std::pair<int, int> getTransportDirection(model::CellType type, int row) {
    bool isOddRow = row % 2 != 0;
   
    switch (type) {
    case model::CellType::UP_RIGHT:
        return { -1, isOddRow ? 1 : 0 };
    case model::CellType::RIGHT:
        return { 0, 1 };
    case model::CellType::DOWN_RIGHT:
        return { 1, isOddRow ? 1 : 0 };
    case model::CellType::DOWN_LEFT:
        return { 1, isOddRow ? 0 : -1 };
    case model::CellType::LEFT:
        return { 0, -1 };
    case model::CellType::UP_LEFT:
        return { -1, isOddRow ? 0 : -1 };
    default:
        return { 0, 0 };
    }
}

std::tuple<int, int, int> slideThroughBands(model::HexGrid& grid, int row, int col, int energy) {
    while (true) {
        const auto& cell = grid.at(row, col);
        auto type = cell.type;
       
        if (type == model::CellType::GOAL) {
            break;
        }
       
        if (!(type >= model::CellType::UP_RIGHT && type <= model::CellType::DOWN_LEFT)) {
            break;
        }

        auto offset = getTransportDirection(type, row);
        int nr = row + offset.first;
        int nc = col + offset.second;

        if (!grid.inBounds(nr, nc)) {
            break;
        }

        const auto& targetCell = grid.at(nr, nc);
        if (targetCell.type == model::CellType::WALL) {
            break;
        }

        energy = std::min(energy + 1, MAX_ENERGY);
        row = nr;
        col = nc;
        
        if (grid.at(row, col).type == model::CellType::GOAL) {
            break;
        }
    }

    return { row, col, energy };
}

std::vector<std::pair<int, int> > getStepByStepPath(
    model::HexGrid& grid,
    const std::vector<std::pair<int, int> >& keyPoints,
    int initialEnergy
) {
    std::vector<std::pair<int, int> > completePath;
   
    if (keyPoints.empty()) return completePath;
   
    completePath.push_back(keyPoints[0]);
   
    for (size_t i = 0; i < keyPoints.size() - 1; ++i) {
        int currentRow = keyPoints[i].first;
        int currentCol = keyPoints[i].second;
        int targetRow = keyPoints[i + 1].first;
        int targetCol = keyPoints[i + 1].second;
       
        if (grid.at(targetRow, targetCol).type == model::CellType::GOAL) {
            bool metaAlreadyAdded = false;
            for (const auto& point : completePath) {
                if (point.first == targetRow && point.second == targetCol) {
                    metaAlreadyAdded = true;
                    break;
                }
            }
            
            if (!metaAlreadyAdded) {
                completePath.push_back({targetRow, targetCol});
            }
            continue;
        }
       
        model::HexCell& cell = grid.at(currentRow, currentCol);
        bool foundPath = false;
       
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
           
            if (nr == targetRow && nc == targetCol) {
                bool alreadyAdded = false;
                for (const auto& point : completePath) {
                    if (point.first == nr && point.second == nc) {
                        alreadyAdded = true;
                        break;
                    }
                }
                
                if (!alreadyAdded) {
                    completePath.push_back({nr, nc});
                }
                foundPath = true;
                break;
            }
           
            auto [finalR, finalC, finalE] = slideThroughBands(grid, nr, nc, 0);
           
            if (finalR == targetRow && finalC == targetCol) {
                bool firstStepAdded = false;
                for (const auto& point : completePath) {
                    if (point.first == nr && point.second == nc) {
                        firstStepAdded = true;
                        break;
                    }
                }
                
                if (!firstStepAdded) {
                    completePath.push_back({nr, nc});
                }
               
                int tempRow = nr, tempCol = nc;
               
                while (true) {
                    const auto& tempCell = grid.at(tempRow, tempCol);
                    auto tempType = tempCell.type;
                   
                    if (!(tempType >= model::CellType::UP_RIGHT && tempType <= model::CellType::DOWN_LEFT)) {
                        break;
                    }
                   
                    auto offset = getTransportDirection(tempType, tempRow);
                    int newTempRow = tempRow + offset.first;
                    int newTempCol = tempCol + offset.second;
                   
                    if (!grid.inBounds(newTempRow, newTempCol)) {
                        break;
                    }
                    if (grid.at(newTempRow, newTempCol).type == model::CellType::WALL) {
                        break;
                    }
                   
                    tempRow = newTempRow;
                    tempCol = newTempCol;
                    
                    bool stepAlreadyAdded = false;
                    for (const auto& point : completePath) {
                        if (point.first == tempRow && point.second == tempCol) {
                            stepAlreadyAdded = true;
                            break;
                        }
                    }
                    
                    if (!stepAlreadyAdded) {
                        completePath.push_back({tempRow, tempCol});
                    }
                   
                    if (tempRow == targetRow && tempCol == targetCol) {
                        break;
                    }
                }
               
                foundPath = true;
                break;
            }
        }
    }
   
    return completePath;
}

PathfindingResult findPath(
    model::HexGrid& grid,
    int startRow, int startCol,
    int goalRow, int goalCol,
    int initialEnergy
) {
    std::priority_queue<State, std::vector<State>, std::greater<State> > openSet;
    std::map<std::tuple<int, int, int>, std::tuple<int, int, int> > cameFrom;
    std::set<std::tuple<int, int, int> > visited;

    int actualStartRow = startRow;
    int actualStartCol = startCol;
    int actualStartEnergy = initialEnergy;
    
    if (grid.at(startRow, startCol).type != model::CellType::GOAL) {
        auto startResult = slideThroughBands(grid, startRow, startCol, initialEnergy);
        actualStartRow = std::get<0>(startResult);
        actualStartCol = std::get<1>(startResult);
        actualStartEnergy = std::get<2>(startResult);
    }

    openSet.push(State{actualStartRow, actualStartCol, actualStartEnergy, 0});
    visited.insert(std::make_tuple(actualStartRow, actualStartCol, actualStartEnergy));

    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();

        model::HexCell& cell = grid.at(current.row, current.col);
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
            int newEnergy = std::min(current.energy + 1, MAX_ENERGY);

            if (neighbor->type == model::CellType::WALL) {
                if (current.energy < MAX_ENERGY) {
                    continue;
                }
                newEnergy = 0;
            }

            if (nr == goalRow && nc == goalCol && neighbor->type == model::CellType::GOAL) {
                std::vector<std::pair<int, int> > keyPoints;
                std::tuple<int, int, int> key = std::make_tuple(current.row, current.col, current.energy);
                
                while (cameFrom.count(key)) {
                    std::tuple<int, int, int> prev = cameFrom[key];
                    keyPoints.push_back(std::make_pair(std::get<0>(key), std::get<1>(key)));
                    key = prev;
                }
                keyPoints.push_back(std::make_pair(actualStartRow, actualStartCol));
                std::reverse(keyPoints.begin(), keyPoints.end());
                
                keyPoints.push_back(std::make_pair(nr, nc));
                
                std::vector<std::pair<int, int> > completePath = getStepByStepPath(grid, keyPoints, initialEnergy);
                
                std::vector<model::HexCell*> path;
                for (const auto& point : completePath) {
                    path.push_back(&grid.at(point.first, point.second));
                }
                
                if (path.empty() || path.back()->row != goalRow || path.back()->col != goalCol) {
                    path.push_back(&grid.at(goalRow, goalCol));
                }
                
                return PathfindingResult{path, true};
            }

            int finalR = nr;
            int finalC = nc;
            int finalEnergy = newEnergy;
            
            if (neighbor->type != model::CellType::GOAL) {
                auto result = slideThroughBands(grid, nr, nc, newEnergy);
                finalR = std::get<0>(result);
                finalC = std::get<1>(result);
                finalEnergy = std::get<2>(result);
                
                if (finalR == goalRow && finalC == goalCol) {
                    std::vector<std::pair<int, int> > keyPoints;
                    std::tuple<int, int, int> key = std::make_tuple(current.row, current.col, current.energy);
                    
                    while (cameFrom.count(key)) {
                        std::tuple<int, int, int> prev = cameFrom[key];
                        keyPoints.push_back(std::make_pair(std::get<0>(key), std::get<1>(key)));
                        key = prev;
                    }
                    keyPoints.push_back(std::make_pair(actualStartRow, actualStartCol));
                    std::reverse(keyPoints.begin(), keyPoints.end());
                    
                    keyPoints.push_back(std::make_pair(finalR, finalC));
                    
                    std::vector<std::pair<int, int> > completePath = getStepByStepPath(grid, keyPoints, initialEnergy);
                    
                    std::vector<model::HexCell*> path;
                    for (const auto& point : completePath) {
                        path.push_back(&grid.at(point.first, point.second));
                    }
                    
                    if (path.empty() || path.back()->row != goalRow || path.back()->col != goalCol) {
                        path.push_back(&grid.at(goalRow, goalCol));
                    }
                    
                    return PathfindingResult{path, true};
                }
            }
           
            std::tuple<int, int, int> key = std::make_tuple(finalR, finalC, finalEnergy);

            if (visited.count(key)) {
                continue;
            }
            visited.insert(key);

            openSet.push(State{finalR, finalC, finalEnergy, current.cost + 1});
            cameFrom[key] = std::make_tuple(current.row, current.col, current.energy);
        }
    }

    return PathfindingResult{std::vector<model::HexCell*>(), false};
}