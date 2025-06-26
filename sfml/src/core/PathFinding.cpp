#include "PathFinding.hpp"
#include "model/HexGrid.hpp"
#include "../utils/Utils.hpp"
#include <queue>
#include <tuple>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>

// Energía máxima para romper pared
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
       
        // No simular bandas si estamos en la META
        if (type == model::CellType::GOAL) {
            break;
        }
       
        if (!(type >= model::CellType::UP_RIGHT && type <= model::CellType::DOWN_LEFT)) {
            break;
        }

        auto offset = getTransportDirection(type, row);
        int nr = row + offset.first;
        int nc = col + offset.second;

        // Verificar límites
        if (!grid.inBounds(nr, nc)) {
            break;
        }

        // Verificar si la celda destino es una pared
        const auto& targetCell = grid.at(nr, nc);
        if (targetCell.type == model::CellType::WALL) {
            break;
        }

        // Simula movimiento automático: gana 1 de energía
        energy = std::min(energy + 1, MAX_ENERGY);
        row = nr;
        col = nc;
        
        // Si llegamos a la meta por las bandas, parar aquí
        if (grid.at(row, col).type == model::CellType::GOAL) {
            break;
        }
    }

    return { row, col, energy };
}

// MEJORADA: getStepByStepPath ahora maneja TODOS los casos correctamente
std::vector<std::pair<int, int> > getStepByStepPath(
    model::HexGrid& grid,
    const std::vector<std::pair<int, int> >& keyPoints,
    int initialEnergy
) {
    std::vector<std::pair<int, int> > completePath;
   
    if (keyPoints.empty()) return completePath;
   
    std::cout << "Generando camino paso a paso con " << keyPoints.size() << " puntos clave..." << std::endl;
   
    // Agregar el primer punto (inicio)
    completePath.push_back(keyPoints[0]);
   
    // Para cada par de puntos consecutivos, simular el camino paso a paso
    for (size_t i = 0; i < keyPoints.size() - 1; ++i) {
        int currentRow = keyPoints[i].first;
        int currentCol = keyPoints[i].second;
        int targetRow = keyPoints[i + 1].first;
        int targetCol = keyPoints[i + 1].second;
       
        std::cout << "Procesando desde (" << currentRow << ", " << currentCol << ") hacia (" << targetRow << ", " << targetCol << ")" << std::endl;
       
        // CASO ESPECIAL: Si el target es la META
        if (grid.at(targetRow, targetCol).type == model::CellType::GOAL) {
            std::cout << "Procesando llegada a META: (" << targetRow << ", " << targetCol << ")" << std::endl;
            
            // Verificar si la META ya está en el path
            bool metaAlreadyAdded = false;
            for (const auto& point : completePath) {
                if (point.first == targetRow && point.second == targetCol) {
                    metaAlreadyAdded = true;
                    break;
                }
            }
            
            if (!metaAlreadyAdded) {
                completePath.push_back({targetRow, targetCol});
                std::cout << "META agregada al camino final en posición " << completePath.size() << std::endl;
            } else {
                std::cout << "META ya estaba en el camino." << std::endl;
            }
            continue;
        }
       
        // CASO NORMAL: Buscar conexión entre current y target
        model::HexCell& cell = grid.at(currentRow, currentCol);
        bool foundPath = false;
       
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
           
            // SUBCASO 1: El vecino ES el target directamente (movimiento simple)
            if (nr == targetRow && nc == targetCol) {
                // Verificar si ya está en el path
                bool alreadyAdded = false;
                for (const auto& point : completePath) {
                    if (point.first == nr && point.second == nc) {
                        alreadyAdded = true;
                        break;
                    }
                }
                
                if (!alreadyAdded) {
                    completePath.push_back({nr, nc});
                    std::cout << "Movimiento directo agregado: (" << nr << ", " << nc << ")" << std::endl;
                }
                foundPath = true;
                break;
            }
           
            // SUBCASO 2: Simular desde este vecino para ver si llega al target via bandas transportadoras
            auto [finalR, finalC, finalE] = slideThroughBands(grid, nr, nc, 0);
           
            if (finalR == targetRow && finalC == targetCol) {
                std::cout << "Ruta via bandas encontrada desde (" << nr << ", " << nc << ") hasta (" << finalR << ", " << finalC << ")" << std::endl;
                
                // Agregar el primer paso (entrada a las bandas)
                bool firstStepAdded = false;
                for (const auto& point : completePath) {
                    if (point.first == nr && point.second == nc) {
                        firstStepAdded = true;
                        break;
                    }
                }
                
                if (!firstStepAdded) {
                    completePath.push_back({nr, nc});
                    std::cout << "Entrada a bandas agregada: (" << nr << ", " << nc << ")" << std::endl;
                }
               
                // IMPORTANTE: Simular paso a paso para capturar TODAS las celdas intermedias
                int tempRow = nr, tempCol = nc;
                int stepCount = 0;
               
                while (true) {
                    const auto& tempCell = grid.at(tempRow, tempCol);
                    auto tempType = tempCell.type;
                   
                    // Si no es banda transportadora, parar
                    if (!(tempType >= model::CellType::UP_RIGHT && tempType <= model::CellType::DOWN_LEFT)) {
                        std::cout << "Fin de bandas transportadoras en (" << tempRow << ", " << tempCol << ")" << std::endl;
                        break;
                    }
                   
                    // Calcular siguiente posición por la banda
                    auto offset = getTransportDirection(tempType, tempRow);
                    int newTempRow = tempRow + offset.first;
                    int newTempCol = tempCol + offset.second;
                   
                    // Verificar límites y paredes
                    if (!grid.inBounds(newTempRow, newTempCol)) {
                        std::cout << "Banda lleva fuera de límites" << std::endl;
                        break;
                    }
                    if (grid.at(newTempRow, newTempCol).type == model::CellType::WALL) {
                        std::cout << "Banda bloqueada por pared" << std::endl;
                        break;
                    }
                   
                    // Moverse por la banda
                    tempRow = newTempRow;
                    tempCol = newTempCol;
                    stepCount++;
                    
                    // Verificar si ya está en el path antes de agregar
                    bool stepAlreadyAdded = false;
                    for (const auto& point : completePath) {
                        if (point.first == tempRow && point.second == tempCol) {
                            stepAlreadyAdded = true;
                            break;
                        }
                    }
                    
                    if (!stepAlreadyAdded) {
                        completePath.push_back({tempRow, tempCol});
                        std::cout << "Paso por banda agregado: (" << tempRow << ", " << tempCol << ")" << std::endl;
                    }
                   
                    // Si llegamos al destino, parar
                    if (tempRow == targetRow && tempCol == targetCol) {
                        std::cout << "Llegada al destino via bandas después de " << stepCount << " pasos" << std::endl;
                        break;
                    }
                }
               
                foundPath = true;
                break;
            }
        }
        
        if (!foundPath) {
            std::cout << "¡ADVERTENCIA! No se encontró conexión entre puntos clave." << std::endl;
        }
    }
   
    std::cout << "Camino paso a paso generado con " << completePath.size() << " pasos totales." << std::endl;
    return completePath;
}

PathfindingResult findPath(
    model::HexGrid& grid,
    int startRow, int startCol,
    int goalRow, int goalCol,
    int initialEnergy
) {
    std::cout << "Iniciando pathfinding desde (" << startRow << ", " << startCol << ") hacia (" << goalRow << ", " << goalCol << ")" << std::endl;
    
    std::priority_queue<State, std::vector<State>, std::greater<State> > openSet;
    std::map<std::tuple<int, int, int>, std::tuple<int, int, int> > cameFrom;
    std::set<std::tuple<int, int, int> > visited;

    // Simular bandas desde la posición inicial (pero NO si ya estamos en la meta)
    int actualStartRow = startRow;
    int actualStartCol = startCol;
    int actualStartEnergy = initialEnergy;
    
    if (grid.at(startRow, startCol).type != model::CellType::GOAL) {
        auto startResult = slideThroughBands(grid, startRow, startCol, initialEnergy);
        actualStartRow = std::get<0>(startResult);
        actualStartCol = std::get<1>(startResult);
        actualStartEnergy = std::get<2>(startResult);
        
        if (actualStartRow != startRow || actualStartCol != startCol) {
            std::cout << "Posición inicial ajustada por bandas: (" << actualStartRow << ", " << actualStartCol << ")" << std::endl;
        }
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

            // Si es pared, verificar si se puede romper
            if (neighbor->type == model::CellType::WALL) {
                if (current.energy < MAX_ENERGY) {
                    continue; // No puede romper la pared
                }
                newEnergy = 0; // Se rompe pared, energía reiniciada
            }

            // CASO 1: Si el vecino ES la META directamente
            if (nr == goalRow && nc == goalCol && neighbor->type == model::CellType::GOAL) {
                std::cout << "META encontrada directamente!" << std::endl;
                
                // Reconstruir el camino incluyendo la META
                std::vector<std::pair<int, int> > keyPoints;
                std::tuple<int, int, int> key = std::make_tuple(current.row, current.col, current.energy);
                
                // Luego reconstruir hacia atrás (SIN incluir la meta todavía)
                while (cameFrom.count(key)) {
                    std::tuple<int, int, int> prev = cameFrom[key];
                    keyPoints.push_back(std::make_pair(std::get<0>(key), std::get<1>(key)));
                    key = prev;
                }
                keyPoints.push_back(std::make_pair(actualStartRow, actualStartCol));
                std::reverse(keyPoints.begin(), keyPoints.end());
                
                // AGREGAR la META como último paso DESPUÉS de reconstruir
                keyPoints.push_back(std::make_pair(nr, nc));
                
                // CORREGIDO: SIEMPRE usar getStepByStepPath para capturar todos los pasos
                std::vector<std::pair<int, int> > completePath = getStepByStepPath(grid, keyPoints, initialEnergy);
                
                // Convertir a HexCell*
                std::vector<model::HexCell*> path;
                for (const auto& point : completePath) {
                    path.push_back(&grid.at(point.first, point.second));
                }
                
                // VERIFICAR que la META esté como último elemento
                if (path.empty() || path.back()->row != goalRow || path.back()->col != goalCol) {
                    std::cout << "AÑADIENDO META como último paso del camino" << std::endl;
                    path.push_back(&grid.at(goalRow, goalCol));
                }
                
                std::cout << "Camino directo a META generado con " << path.size() << " pasos." << std::endl;
                std::cout << "Último paso del camino: (" << path.back()->row << ", " << path.back()->col << ")" << std::endl;
                return PathfindingResult{path, true};
            }

            // CASO 2: Simular bandas para otros destinos
            int finalR = nr;
            int finalC = nc;
            int finalEnergy = newEnergy;
            
            if (neighbor->type != model::CellType::GOAL) {
                // Solo simular bandas si NO es la meta
                auto result = slideThroughBands(grid, nr, nc, newEnergy);
                finalR = std::get<0>(result);
                finalC = std::get<1>(result);
                finalEnergy = std::get<2>(result);
                
                // CASO 3: Si después de las bandas llegamos a la META
                if (finalR == goalRow && finalC == goalCol) {
                    std::cout << "META encontrada via bandas transportadoras!" << std::endl;
                    
                    // Reconstruir el camino incluyendo todo el trayecto
                    std::vector<std::pair<int, int> > keyPoints;
                    std::tuple<int, int, int> key = std::make_tuple(current.row, current.col, current.energy);
                    
                    // Luego reconstruir hacia atrás (SIN incluir la meta todavía)
                    while (cameFrom.count(key)) {
                        std::tuple<int, int, int> prev = cameFrom[key];
                        keyPoints.push_back(std::make_pair(std::get<0>(key), std::get<1>(key)));
                        key = prev;
                    }
                    keyPoints.push_back(std::make_pair(actualStartRow, actualStartCol));
                    std::reverse(keyPoints.begin(), keyPoints.end());
                    
                    // AGREGAR la META como último paso DESPUÉS de reconstruir
                    keyPoints.push_back(std::make_pair(finalR, finalC));
                    
                    // IMPORTANTE: SÍ usar getStepByStepPath para capturar todos los pasos via bandas
                    std::vector<std::pair<int, int> > completePath = getStepByStepPath(grid, keyPoints, initialEnergy);
                    
                    // Convertir a HexCell*
                    std::vector<model::HexCell*> path;
                    for (const auto& point : completePath) {
                        path.push_back(&grid.at(point.first, point.second));
                    }
                    
                    // ASEGURAR que la META esté incluida como último paso
                    if (path.empty() || path.back()->row != goalRow || path.back()->col != goalCol) {
                        std::cout << "AÑADIENDO META como último paso del camino via bandas" << std::endl;
                        path.push_back(&grid.at(goalRow, goalCol));
                    }
                    
                    std::cout << "Camino via bandas a META generado con " << path.size() << " pasos." << std::endl;
                    std::cout << "Último paso del camino: (" << path.back()->row << ", " << path.back()->col << ")" << std::endl;
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

    std::cout << "No se encontró camino hacia la META." << std::endl;
    return PathfindingResult{std::vector<model::HexCell*>(), false}; // No hay camino
}