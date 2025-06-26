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
    std::cerr << "No se encontro celda de inicio (START)\n";
    return nullptr;
}

HexCell* findGoalCell(HexGrid& grid) {
    for (int y = 0; y < grid.rows(); ++y) {
        for (int x = 0; x < grid.cols(); ++x) {
            if (grid.at(y, x).type == CellType::GOAL)
                return &grid.at(y, x);
        }
    }
    std::cerr << "No se encontro celda de meta (GOAL)\n";
    return nullptr;
}

void handlePlayerMovement(Keyboard::Key key, Player& player, HexGrid& grid) {
    if (player.isMoving)
        return;

    if (player.isSelectingWall) {
        if (isValidWallBreakDirection(key)) {
            handleWallBreak(key, player, grid);
            return;
        }
        else if (key == Keyboard::Escape || key == Keyboard::Space) {
            player.isSelectingWall = false;
            std::cout << "Seleccion de pared cancelada." << std::endl;
            return;
        }
    }

    if (key == Keyboard::Space) {
        if (player.canUseWallBreak()) {
            player.isSelectingWall = true;
            std::cout << "Modo seleccion de pared activado!" << std::endl;
            std::cout << "Usa W/E (arriba), A/D (lados), Z/X (abajo) para elegir que pared romper." << std::endl;
            std::cout << "Presiona ESC para cancelar." << std::endl;
            return;
        } else {
            if (!player.isEnergyFull()) {
                std::cout << "Energia insuficiente. Necesitas " << Player::MAX_ENERGY
                    << " puntos de energia. Actual: " << player.energy << std::endl;
            }
            return;
        }
    }

    int row = player.row, col = player.col;
    int newRow = row, newCol = col;
    bool isOdd = row % 2 != 0;
    bool moved = false;

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

    if (moved && newRow >= 0 && newRow < grid.rows() && newCol >= 0 && newCol < grid.cols()) {
        const auto& target = grid.at(newRow, newCol);

        if (target.type != CellType::WALL) {
            int oldRow = player.row;
            int oldCol = player.col;
            
            Vector2f startPos = grid.toPixel(player.row, player.col);
            Vector2f targetPos = grid.toPixel(newRow, newCol);
            player.startMovement(startPos, targetPos);

            player.lastCellType = grid.at(player.row, player.col).type;
            
            player.row = newRow;
            player.col = newCol;

            std::cout << "Jugador movido de (" << oldRow << ", " << oldCol << ") a (" << player.row << ", " << player.col << ")" << std::endl;

            if (target.type == CellType::GOAL) {
                if (player.row == newRow && player.col == newCol) {
                    player.hasWon = true;
                    player.winTime = player.winClock.getElapsedTime().asSeconds();
                    std::cout << "VICTORIA CONFIRMADA! Jugador en META (" << player.row << ", " << player.col << ") en " << player.winTime << " segundos!" << std::endl;
                    return;
                } else {
                    std::cout << "ERROR: target es GOAL pero jugador no esta en la posicion correcta!" << std::endl;
                }
            }

            if (!player.hasWon) {
                player.gainEnergy();
                TurnSystem::handleTurn(grid, player);
                std::cout << "Movimiento completado. Energia: " << player.energy << ", Celda: " << static_cast<int>(target.type) << std::endl;
            }
        } else {
            std::cout << "Movimiento bloqueado por pared en (" << newRow << ", " << newCol << ")" << std::endl;
        }
    } else {
        std::cout << "Movimiento fuera de limites o invalido" << std::endl;
    }
}

void handleWallBreak(Keyboard::Key key, Player& player, HexGrid& grid) {
    if (!player.canUseWallBreak() || !player.isSelectingWall) {
        return;
    }

    auto [wallRow, wallCol] = getWallPositionInDirection(player, key, grid);

    if (wallRow == -1 || wallCol == -1) {
        std::cout << "No hay pared en esa direccion." << std::endl;
        return;
    }

    if (wallRow < 0 || wallRow >= grid.rows() || wallCol < 0 || wallCol >= grid.cols()) {
        std::cout << "Posicion fuera de los limites del grid." << std::endl;
        return;
    }

    if (grid.at(wallRow, wallCol).type != CellType::WALL) {
        std::cout << "No hay una pared en esa posicion." << std::endl;
        return;
    }

    grid.at(wallRow, wallCol).type = CellType::EMPTY;
    player.useWallBreak();
    player.isSelectingWall = false;

    std::cout << "Pared rota en posicion (" << wallRow << ", " << wallCol << ")!" << std::endl;
}

std::pair<int, int> getDirectionalOffset(sf::Keyboard::Key key, int currentRow) {
    bool isOdd = currentRow % 2 != 0;
   
    switch (key) {
        case Keyboard::W:
            return {-1, isOdd ? 0 : -1};
        case Keyboard::E:
            return {-1, isOdd ? 1 : 0};
        case Keyboard::A:
            return {0, -1};
        case Keyboard::D:
            return {0, 1};
        case Keyboard::Z:
            return {1, isOdd ? 0 : -1};
        case Keyboard::X:
            return {1, isOdd ? 1 : 0};
        default:
            return {0, 0};
    }
}

bool isValidWallBreakDirection(sf::Keyboard::Key key) {
    return (key == Keyboard::W || key == Keyboard::E ||
            key == Keyboard::A || key == Keyboard::D ||
            key == Keyboard::Z || key == Keyboard::X);
}

std::pair<int, int> getWallPositionInDirection(const Player& player, sf::Keyboard::Key direction, const HexGrid& grid) {
    auto [deltaRow, deltaCol] = getDirectionalOffset(direction, player.row);
   
    int wallRow = player.row + deltaRow;
    int wallCol = player.col + deltaCol;
   
    if (wallRow < 0 || wallRow >= grid.rows() || wallCol < 0 || wallCol >= grid.cols()) {
        return {-1, -1};
    }
   
    return {wallRow, wallCol};
}

std::vector<std::pair<int, int>> findAdjacentWalls(const Player& player, const HexGrid& grid) {
    std::vector<std::pair<int, int>> walls;

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
    if (player.isMoving || player.hasWon)
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
            int oldRow = player.row;
            int oldCol = player.col;
            
            Vector2f startPos = grid.toPixel(player.row, player.col);
            Vector2f targetPos = grid.toPixel(newRow, newCol);
            player.startMovement(startPos, targetPos);

            player.lastCellType = current.type;
            
            player.row = newRow;
            player.col = newCol;
            
            std::cout << "Banda transportadora: (" << oldRow << ", " << oldCol << ") -> (" << player.row << ", " << player.col << ")" << std::endl;

            if (target.type == CellType::GOAL) {
                if (player.row == newRow && player.col == newCol) {
                    player.hasWon = true;
                    player.winTime = player.winClock.getElapsedTime().asSeconds();
                    std::cout << "VICTORIA POR BANDA TRANSPORTADORA! Jugador en META (" << player.row << ", " << player.col << ")!" << std::endl;
                    return;
                }
            }

            if (!player.hasWon) {
                player.gainEnergy();
            }
        }
    }
}