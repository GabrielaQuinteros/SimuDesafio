#include "GameLogic.hpp"
#include "TurnSystem.hpp"
#include "../model/HexCell.hpp"
#include "../utils/Utils.hpp"
#include "../model/Player.hpp"
#include <SFML/System.hpp>
#include <iostream>

using namespace model;
using namespace sf;

HexCell* findStartCell(HexGrid& grid) {
    for (int y = 0; y < grid.rows(); ++y) {
        for (int x = 0; x < grid.cols(); ++x) {
            if (grid.at(y, x).type == CellType::START)
                return &grid.at(y, x);
        }
    }
    std::cerr << "No se encontró celda de inicio (START)\n";
    return nullptr;
}

void handlePlayerMovement(Keyboard::Key key, Player& player, HexGrid& grid) {
    // No permitir movimiento manual si el jugador está siendo animado
    if (player.isMoving)
        return;

    // Manejar la habilidad de romper pared con SPACE
    if (key == Keyboard::Space) {
        handleWallBreak(player, grid);
        return;
    }

    int row = player.row, col = player.col;
    int newRow = row, newCol = col;
    bool isOdd = row % 2 != 0;
    bool moved = false;

    // Movimiento en el grid según la tecla presionada
    if (key == Keyboard::W) {
        newRow = row - 1;
        newCol = isOdd ? col : col - 1;
        moved = true;
    }
    else if (key == Keyboard::E) {
        newRow = row - 1;
        newCol = isOdd ? col + 1 : col;
        moved = true;
    }
    else if (key == Keyboard::A) {
        newRow = row;
        newCol = col - 1;
        moved = true;
    }
    else if (key == Keyboard::D) {
        newRow = row;
        newCol = col + 1;
        moved = true;
    }
    else if (key == Keyboard::Z) {
        newRow = row + 1;
        newCol = isOdd ? col : col - 1;
        moved = true;
    }
    else if (key == Keyboard::X) {
        newRow = row + 1;
        newCol = isOdd ? col + 1 : col;
        moved = true;
    }

    // Revisa que el siguiente paso no sea 'out of bounds'
    if (moved && newRow >= 0 && newRow < grid.rows() && newCol >= 0 && newCol < grid.cols()) {
        const auto& target = grid.at(newRow, newCol);

        if (target.type != CellType::WALL) {
            // Iniciar animación de movimiento
            Vector2f startPos = grid.toPixel(player.row, player.col);
            Vector2f targetPos = grid.toPixel(newRow, newCol);
            player.startMovement(startPos, targetPos);

            player.lastCellType = grid.at(player.row, player.col).type;
            player.row = newRow;
            player.col = newCol;

            // ¡IMPORTANTE! Ganar energía al moverse exitosamente
            player.gainEnergy();

            // Manejar el sistema de turnos después de un movimiento exitoso
            TurnSystem::handleTurn(grid, player);
        }
    }
}

void handleWallBreak(Player& player, HexGrid& grid) {
    if (!player.canUseWallBreak()) {
        if (!player.isEnergyFull()) {
            std::cout << "Energía insuficiente. Necesitas " << Player::MAX_ENERGY
                << " puntos de energía. Actual: " << player.energy << std::endl;
        }
        return;
    }

    // Buscar paredes adyacentes al jugador
    std::vector<std::pair<int, int>> adjacentWalls = findAdjacentWalls(player, grid);

    if (adjacentWalls.empty()) {
        std::cout << "No hay paredes adyacentes para romper." << std::endl;
        return;
    }

    // Por simplicidad, romper la primera pared encontrada
    // En una implementación más avanzada, podrías permitir al jugador elegir
    auto [wallRow, wallCol] = adjacentWalls[0];

    // Romper la pared (convertirla en celda vacía)
    grid.at(wallRow, wallCol).type = CellType::EMPTY;

    // Usar la habilidad (resetea la energía)
    player.useWallBreak();

    std::cout << "¡Pared rota en posición (" << wallRow << ", " << wallCol << ")!" << std::endl;
}

std::vector<std::pair<int, int>> findAdjacentWalls(const Player& player, const HexGrid& grid) {
    std::vector<std::pair<int, int>> walls;

    // Usar el sistema de vecinos del HexGrid
    HexCell currentCell(player.row, player.col, CellType::EMPTY);
    auto neighbors = const_cast<HexGrid&>(grid).neighbors(currentCell);

    for (const auto* neighbor : neighbors) {
        if (neighbor && neighbor->type == CellType::WALL) {
            walls.push_back({ neighbor->row, neighbor->col });
        }
    }

    return walls;
}

void handleConveyorMovement(Player& player, const HexGrid& grid) {
    if (player.isMoving)
        return;

    const HexCell& current = grid.at(player.row, player.col);

    if (!(current.type >= CellType::UP_RIGHT && current.type <= CellType::DOWN_LEFT))
        return;

    bool isOdd = player.row % 2 != 0;
    std::pair<int, int> offset = getConveyorOffset(current.type, isOdd);
    int newRow = player.row + offset.first;
    int newCol = player.col + offset.second;

    if (newRow >= 0 && newRow < grid.rows() && newCol >= 0 && newCol < grid.cols()) {
        const auto& target = grid.at(newRow, newCol);
        if (target.type != CellType::WALL) {
            Vector2f startPos = grid.toPixel(player.row, player.col);
            Vector2f targetPos = grid.toPixel(newRow, newCol);
            player.startMovement(startPos, targetPos);

            player.lastCellType = current.type;
            player.row = newRow;
            player.col = newCol;

            // Ganar energía también al moverse por banda transportadora
            player.gainEnergy();
        }
    }
}