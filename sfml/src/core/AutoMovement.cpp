#include "AutoMovement.hpp"
#include "../core/TurnSystem.hpp"
#include "../core/GameLogic.hpp"
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

void updateAutoMovement(
    HexGrid& grid,
    Player& player,
    std::vector<std::pair<int,int>>& pathCells,
    int goalRow,
    int goalCol
) {
    // Si no hay ruta o no estamos en modo auto-mov, salimos
    if (!player.isAutoMoving || pathCells.empty()) {
        player.isAutoMoving = false;
        return;
    }

    // Esperamos a que termine cualquier animación de movimiento
    if (player.isMoving) return;

    // Siguiente paso
    auto [nextR, nextC] = pathCells.front();

    // Si es muro…
    if (grid.at(nextR, nextC).type == CellType::WALL) {
        if (player.energy >= TurnSystem::ENERGY_PER_WALL_BREAK) {
            // Rompemos muro, consumimos energía
            grid.at(nextR, nextC).type = CellType::EMPTY;
            player.energy -= TurnSystem::ENERGY_PER_WALL_BREAK;
            // Recalcular ruta desde la posición actual
            auto result = findPath(
                grid, player.row, player.col,
                goalRow, goalCol, player.energy
            );
            pathCells.clear();
            for (auto* cell : result.path)
                pathCells.emplace_back(cell->row, cell->col);
        } else {
            // Sin energía, detenemos
            player.isAutoMoving = false;
        }
        return;
    }

    int dR = nextR - player.row;
    int dC = nextC - player.col;
    Keyboard::Key dir = keyFromDelta(dR, dC, player.row);

    handlePlayerMovement(dir, player, grid);

    // Eliminamos ese paso del vector
    pathCells.erase(pathCells.begin());
}

}
