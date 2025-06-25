#include "PathFinding.hpp"
#include "model/HexGrid.hpp"
#include "../utils/Utils.hpp"  // Para usar getConveyorOffset
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




// CORREGIDO: Direcciones exactas según tu descripción
std::pair<int, int> getTransportDirection(model::CellType type, int row) {
    bool isOddRow = row % 2 != 0;
   
    switch (type) {
    case model::CellType::UP_RIGHT:     // A - Arriba derecha
        return { -1, isOddRow ? 1 : 0 };
    case model::CellType::RIGHT:        // B - Derecha
        return { 0, 1 };
    case model::CellType::DOWN_RIGHT:   // C - Abajo derecha
        return { 1, isOddRow ? 1 : 0 };
    case model::CellType::DOWN_LEFT:    // D - Abajo izquierda
        return { 1, isOddRow ? 0 : -1 };
    case model::CellType::LEFT:         // E - Izquierda
        return { 0, -1 };
    case model::CellType::UP_LEFT:      // F - Arriba izquierda
        return { -1, isOddRow ? 0 : -1 };
    default:
        return { 0, 0 };
    }
}


std::tuple<int, int, int> slideThroughBands(model::HexGrid& grid, int row, int col, int energy) {
    std::cout << "Simulando bandas desde (" << row << ", " << col << ") con energía " << energy << std::endl;
   
    while (true) {
        const auto& cell = grid.at(row, col);
        auto type = cell.type;
       
        if (!(type >= model::CellType::UP_RIGHT && type <= model::CellType::DOWN_LEFT)) {
            std::cout << "  No es banda transportadora, parando en (" << row << ", " << col << ")" << std::endl;
            break;
        }


        std::string bandName;


        bool isOdd = row % 2 != 0;
        auto offset = getTransportDirection(type, row);
        int nr = row + offset.first;
        int nc = col + offset.second;




        std::cout << "    Moviéndose a (" << nr << ", " << nc << ")" << std::endl;




        // Verificar límites
        if (!grid.inBounds(nr, nc)) {
            std::cout << "    Fuera de límites, parando" << std::endl;
            break;
        }




        // Verificar si la celda destino no es una pared
        const auto& targetCell = grid.at(nr, nc);
        if (targetCell.type == model::CellType::WALL) {
            std::cout << "    Destino es pared, parando" << std::endl;
            break;
        }




        // Simula movimiento automático: gana 1 de energía (igual que en GameLogic.cpp)
        energy = std::min(energy + 1, MAX_ENERGY);
        row = nr;
        col = nc;
       
        std::cout << "    Nuevo estado: (" << row << ", " << col << ") energía " << energy << std::endl;
    }




    std::cout << "Simulación terminada en (" << row << ", " << col << ") con energía " << energy << std::endl;
    return { row, col, energy };
}




// NUEVA FUNCIÓN: Obtener el camino completo paso a paso
std::vector<std::pair<int, int> > getStepByStepPath(
    model::HexGrid& grid,
    const std::vector<std::pair<int, int> >& keyPoints,
    int initialEnergy
) {
    std::vector<std::pair<int, int> > completePath;
   
    if (keyPoints.empty()) return completePath;
   
    // Agregar el primer punto
    completePath.push_back(keyPoints[0]);
   
    // Para cada par de puntos consecutivos, simular el camino paso a paso
    for (size_t i = 0; i < keyPoints.size() - 1; ++i) {
        int currentRow = keyPoints[i].first;
        int currentCol = keyPoints[i].second;
        int targetRow = keyPoints[i + 1].first;
        int targetCol = keyPoints[i + 1].second;
       
        std::cout << "Simulando desde (" << currentRow << ", " << currentCol << ") hacia (" << targetRow << ", " << targetCol << ")" << std::endl;
       
        // Encontrar qué vecino lleva al siguiente punto clave
        model::HexCell& cell = grid.at(currentRow, currentCol);
        bool foundPath = false;
       
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
           
            // Simular desde este vecino para ver si llega al target
            auto [finalR, finalC, finalE] = slideThroughBands(grid, nr, nc, 0);
           
            if (finalR == targetRow && finalC == targetCol) {
                std::cout << "  Encontrado camino via vecino (" << nr << ", " << nc << ")" << std::endl;
               
                // Agregar el primer paso
                completePath.push_back({nr, nc});
               
                // Simular paso a paso para capturar TODAS las celdas intermedias
                int tempRow = nr, tempCol = nc;
               
                while (true) {
                    const auto& tempCell = grid.at(tempRow, tempCol);
                    auto tempType = tempCell.type;
                   
                    // Si no es banda transportadora, parar
                    if (!(tempType >= model::CellType::UP_RIGHT && tempType <= model::CellType::DOWN_LEFT)) {
                        break;
                    }
                   
                    // Calcular siguiente posición por la banda
                    auto offset = getTransportDirection(tempType, tempRow);
                    int newTempRow = tempRow + offset.first;
                    int newTempCol = tempCol + offset.second;
                   
                    // Verificar límites y paredes
                    if (!grid.inBounds(newTempRow, newTempCol)) break;
                    if (grid.at(newTempRow, newTempCol).type == model::CellType::WALL) break;
                   
                    // Moverse
                    tempRow = newTempRow;
                    tempCol = newTempCol;
                    completePath.push_back({tempRow, tempCol});
                   
                    std::cout << "    Paso intermedio: (" << tempRow << ", " << tempCol << ")" << std::endl;
                   
                    // Si llegamos al destino, parar
                    if (tempRow == targetRow && tempCol == targetCol) break;
                }
               
                foundPath = true;
                break;
            }
        }
       
        if (!foundPath) {
            std::cout << "  ERROR: No se encontró camino desde (" << currentRow << ", " << currentCol << ") hacia (" << targetRow << ", " << targetCol << ")" << std::endl;
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
    std::cout << "\n=== INICIANDO PATHFINDING ===" << std::endl;
    std::cout << "Inicio: (" << startRow << ", " << startCol << ") -> Meta: (" << goalRow << ", " << goalCol << ")" << std::endl;
    std::cout << "Energía inicial: " << initialEnergy << std::endl;
   
    std::priority_queue<State, std::vector<State>, std::greater<State> > openSet;
    std::map<std::tuple<int, int, int>, std::tuple<int, int, int> > cameFrom;
    std::set<std::tuple<int, int, int> > visited;




    // Simular bandas desde la posición inicial
    auto startResult = slideThroughBands(grid, startRow, startCol, initialEnergy);
    int actualStartRow = std::get<0>(startResult);
    int actualStartCol = std::get<1>(startResult);
    int actualStartEnergy = std::get<2>(startResult);




    std::cout << "Posición real de inicio después de bandas: (" << actualStartRow << ", " << actualStartCol << ")" << std::endl;




    openSet.push(State{actualStartRow, actualStartCol, actualStartEnergy, 0});
    visited.insert(std::make_tuple(actualStartRow, actualStartCol, actualStartEnergy));




    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();




        std::cout << "\nExplorando estado: (" << current.row << ", " << current.col << ") energía " << current.energy << std::endl;




        // Verificar si llegamos a la meta
        if (current.row == goalRow && current.col == goalCol) {
            std::cout << "¡CAMINO ENCONTRADO!" << std::endl;
           
            // Reconstruir puntos clave
            std::vector<std::pair<int, int> > keyPoints;
            std::tuple<int, int, int> key = std::make_tuple(current.row, current.col, current.energy);
           
            while (cameFrom.count(key)) {
                std::tuple<int, int, int> prev = cameFrom[key];
                keyPoints.push_back(std::make_pair(std::get<0>(key), std::get<1>(key)));
                std::cout << "  Punto clave: (" << std::get<0>(key) << ", " << std::get<1>(key) << ")" << std::endl;
                key = prev;
            }
            keyPoints.push_back(std::make_pair(actualStartRow, actualStartCol));
            std::reverse(keyPoints.begin(), keyPoints.end());
           
            // NUEVO: Obtener el camino completo paso a paso
            std::vector<std::pair<int, int> > completePath = getStepByStepPath(grid, keyPoints, initialEnergy);
           
            // Convertir a HexCell* y eliminar duplicados consecutivos
            std::vector<model::HexCell*> path;
            std::pair<int, int> lastPos = std::make_pair(-1, -1);
           
            for (const std::pair<int, int>& pos : completePath) {
                if (pos != lastPos) {
                    path.push_back(&grid.at(pos.first, pos.second));
                    lastPos = pos;
                    std::cout << "  Camino completo: (" << pos.first << ", " << pos.second << ")" << std::endl;
                }
            }
           
            return PathfindingResult{path, true};
        }




        model::HexCell& cell = grid.at(current.row, current.col);
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
            int newEnergy = std::min(current.energy + 1, MAX_ENERGY);




            std::cout << "  Vecino: (" << nr << ", " << nc << ") tipo: " << static_cast<int>(neighbor->type) << std::endl;




            // Si es pared, verificar si se puede romper
            if (neighbor->type == model::CellType::WALL) {
                if (current.energy < MAX_ENERGY) {
                    std::cout << "    Es pared y no hay suficiente energía" << std::endl;
                    continue; // No puede romper la pared
                }
                newEnergy = 0; // Se rompe pared, energía reiniciada
                std::cout << "    Rompiendo pared, energía reiniciada" << std::endl;
            }




            // IMPORTANTE: Simular bandas desde la nueva posición
            auto result = slideThroughBands(grid, nr, nc, newEnergy);
            int finalR = std::get<0>(result);
            int finalC = std::get<1>(result);
            int finalEnergy = std::get<2>(result);
           
            std::tuple<int, int, int> key = std::make_tuple(finalR, finalC, finalEnergy);




            if (visited.count(key)) {
                std::cout << "    Estado ya visitado" << std::endl;
                continue;
            }
            visited.insert(key);




            std::cout << "    Agregando estado: (" << finalR << ", " << finalC << ") energía " << finalEnergy << std::endl;
            openSet.push(State{finalR, finalC, finalEnergy, current.cost + 1});
            cameFrom[key] = std::make_tuple(current.row, current.col, current.energy);
        }
    }




    std::cout << "NO SE ENCONTRÓ CAMINO" << std::endl;
    return PathfindingResult{std::vector<model::HexCell*>(), false}; // No hay camino
}
