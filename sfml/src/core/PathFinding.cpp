#pragma once

#include "Pathfinding.hpp"
#include "model/HexGrid.hpp"
#include <queue>
#include <tuple>
#include <map>
#include <set>
#include <algorithm>

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

// Devuelve el desplazamiento en fila/columna según el tipo de banda
std::pair<int, int> getTransportDirection(model::CellType type, int row) {
    // En hexágonos pointy-top con offset-odd
    bool isOdd = row % 2 != 0;

    switch (type) {
    case model::CellType::UP_RIGHT:     return { -1, isOdd ? 0 : 1 };
    case model::CellType::UP_LEFT:      return { -1, isOdd ? -1 : 0 };
    case model::CellType::RIGHT:        return { 0, 1 };
    case model::CellType::LEFT:         return { 0, -1 };
    case model::CellType::DOWN_RIGHT:   return { 1, isOdd ? 0 : 1 };
    case model::CellType::DOWN_LEFT:    return { 1, isOdd ? -1 : 0 };
    default:                            return { 0, 0 };
    }
}

// Simula deslizamiento por bandas transportadoras
std::tuple<int, int, int> slideThroughBands(model::HexGrid& grid, int row, int col, int energy) {
    while (true) {
        const auto& cell = grid.at(row, col);
        auto type = cell.type;
        if (type != model::CellType::UP_RIGHT &&
            type != model::CellType::UP_LEFT &&
            type != model::CellType::RIGHT &&
            type != model::CellType::LEFT &&
            type != model::CellType::DOWN_RIGHT &&
            type != model::CellType::DOWN_LEFT)
        {
            break; // no es banda
        }

        auto [dr, dc] = getTransportDirection(type, row);
        int nr = row + dr;
        int nc = col + dc;

        if (!grid.inBounds(nr, nc)) break;

        // Simula movimiento automático: gana 1 de energía
        energy = std::min(energy + 1, MAX_ENERGY);
        row = nr;
        col = nc;
    }

    return { row, col, energy };
}

PathfindingResult findPath(
    model::HexGrid& grid,
    int startRow, int startCol,
    int goalRow, int goalCol,
    int initialEnergy
) {
    std::priority_queue<State, std::vector<State>, std::greater<State>> openSet;
    std::map<std::tuple<int, int, int>, std::tuple<int, int, int>> cameFrom;
    std::set<std::tuple<int, int, int>> visited;

    openSet.push({ startRow, startCol, initialEnergy, 0 });
    visited.insert({ startRow, startCol, initialEnergy });

    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();

        // Verifica si llegamos
        if (current.row == goalRow && current.col == goalCol) {
            std::vector<model::HexCell*> path;
            auto key = std::make_tuple(current.row, current.col, current.energy);
            while (cameFrom.count(key)) {
                auto [pr, pc, pe] = cameFrom[key];
                path.push_back(&grid.at(std::get<0>(key), std::get<1>(key)));
                key = std::make_tuple(pr, pc, pe);
            }
            path.push_back(&grid.at(startRow, startCol));
            std::reverse(path.begin(), path.end());
            return { path, true };
        }

        model::HexCell& cell = grid.at(current.row, current.col);
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
            int newEnergy = std::min(current.energy + 1, MAX_ENERGY);

            // Si es pared
            if (neighbor->type == model::CellType::WALL) {
                if (~ current.energy >= MAX_ENERGY) continue;
                newEnergy = 0; // se rompe pared, energía reiniciada
            }

            // Simula bandas desde esa celda
            auto [finalR, finalC, finalEnergy] = slideThroughBands(grid, nr, nc, newEnergy);
            auto key = std::make_tuple(finalR, finalC, finalEnergy);

            if (visited.count(key)) continue;
            visited.insert(key);

            openSet.push({ finalR, finalC, finalEnergy, current.cost + 1 });
            cameFrom[key] = std::make_tuple(current.row, current.col, current.energy);
        }
    }

    return { {}, false }; // no hay camino
}
