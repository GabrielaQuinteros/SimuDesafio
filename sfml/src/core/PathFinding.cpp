#pragma once

#include "Pathfinding.hpp"
#include <queue>
#include <tuple>
#include <set>
#include <map>
#include <algorithm> // Para std::reverse


struct State {
    int row, col;
    int turnsPassed;
    int priority;

    bool operator>(const State& other) const {
        return priority > other.priority;
    }
};

PathfindingResult findPathWithCooldown(
    model::HexGrid& grid,
    int startRow, int startCol,
    int goalRow, int goalCol,
    int cooldown
) {
    std::priority_queue<State, std::vector<State>, std::greater<State>> openSet;
    std::map<std::tuple<int,int,int>, std::tuple<int,int,int>> cameFrom;
    std::set<std::tuple<int,int,int>> visited;

    openSet.push({startRow, startCol, 0, 0});
    visited.insert({startRow, startCol, 0});

    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();

        if (current.row == goalRow && current.col == goalCol) {
            // Reconstrucción del camino
            std::vector<model::HexCell*> path;
            auto key = std::make_tuple(goalRow, goalCol, current.turnsPassed % cooldown);

            while (cameFrom.count(key)) {
                auto [r, c, t] = key;
                path.push_back(&grid.at(r, c));
                key = cameFrom[key];
            }
            path.push_back(&grid.at(startRow, startCol));
            std::reverse(path.begin(), path.end());
            return { path, true };
        }

        model::HexCell& cell = grid.at(current.row, current.col);
        for (model::HexCell* neighbor : grid.neighbors(cell)) {
            int nr = neighbor->row;
            int nc = neighbor->col;
            int nextTurns = current.turnsPassed + 1;

            bool puedeRomper = (nextTurns % cooldown == 0);

            if (neighbor->type == model::CellType::WALL && !puedeRomper) continue;

            auto key = std::make_tuple(nr, nc, nextTurns % cooldown);
            if (visited.count(key)) continue;
            visited.insert(key);
            int priority = nextTurns; // heurística básica; podés añadir distancia al objetivo
            openSet.push({ nr, nc, nextTurns, priority });
            cameFrom[key] = std::make_tuple(current.row, current.col, current.turnsPassed % cooldown);
        }
    }

    return {{}, false}; // No se encontró camino
}

