#include "AutoMovement.hpp"
#include "../core/TurnSystem.hpp"
#include "../core/GameLogic.hpp"
#include "../core/PathFinding.hpp"
#include <SFML/Window.hpp>
#include <iostream>

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

// NUEVA: Función para recalcular automáticamente el camino
bool recalculatePathInAutoMovement(
    HexGrid& grid,
    Player& player,
    std::vector<std::pair<int, int>>& pathCells,
    int goalRow,
    int goalCol
) {
    std::cout << "Recalculando camino automáticamente..." << std::endl;
    
    PathfindingResult newPath = findPath(grid, player.row, player.col, goalRow, goalCol, player.energy);
    
    if (newPath.success && !newPath.path.empty()) {
        // Actualizar el camino con el nuevo path
        pathCells.clear();
        for (auto* cell : newPath.path) {
            pathCells.emplace_back(cell->row, cell->col);
        }
        
        std::cout << "¡Nuevo camino encontrado con " << pathCells.size() << " pasos!" << std::endl;
        return true;
    } else {
        std::cout << "No se pudo encontrar un camino alternativo." << std::endl;
        return false;
    }
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

    // Encontrar dónde estamos en el camino
    int currentIndex = -1;
    for (size_t i = 0; i < pathCells.size(); ++i) {
        if (pathCells[i].first == player.row && pathCells[i].second == player.col) {
            currentIndex = static_cast<int>(i);
            break;
        }
    }

    // Si no estamos en el camino, empezar desde el primer paso
    if (currentIndex == -1) {
        currentIndex = 0;
    } else {
        // Avanzar al siguiente paso
        currentIndex++;
    }

    // Verificar si hemos completado el camino
    if (currentIndex >= static_cast<int>(pathCells.size())) {
        // VERIFICACIÓN FINAL: ¿Realmente llegamos a la meta?
        HexCell* goalCell = &grid.at(goalRow, goalCol);
        if (player.row == goalRow && player.col == goalCol && goalCell->type == CellType::GOAL) {
            std::cout << "¡Camino completado! Jugador confirmado en META (" << player.row << ", " << player.col << ")" << std::endl;
            player.isAutoMoving = false;
            
            // Si por alguna razón no se detectó la victoria, forzarla
            if (!player.hasWon) {
                std::cout << "Victoria no detectada automáticamente. Verificando manualmente..." << std::endl;
                if (grid.at(player.row, player.col).type == CellType::GOAL) {
                    player.hasWon = true;
                    player.winTime = player.winClock.getElapsedTime().asSeconds();
                    std::cout << "¡VICTORIA MANUAL ACTIVADA!" << std::endl;
                }
            }
        } else {
            std::cout << "¡ADVERTENCIA! Camino completado pero jugador NO está en la meta." << std::endl;
            std::cout << "Posición jugador: (" << player.row << ", " << player.col << ")" << std::endl;
            std::cout << "Posición meta: (" << goalRow << ", " << goalCol << ")" << std::endl;
            player.isAutoMoving = false;
        }
        return;
    }

    // Obtener el siguiente paso
    int nextR = pathCells[currentIndex].first;
    int nextC = pathCells[currentIndex].second;

    std::cout << "Auto-movimiento: Paso " << (currentIndex + 1) << "/" << pathCells.size() 
              << " - Ir a (" << nextR << ", " << nextC << ")" << std::endl;
    std::cout << "Posición actual: (" << player.row << ", " << player.col << ")" << std::endl;

    // Verificar si el siguiente paso es la meta
    if (nextR == goalRow && nextC == goalCol && grid.at(nextR, nextC).type == CellType::GOAL) {
        // Es la meta, moverse directamente
        int dR = nextR - player.row;
        int dC = nextC - player.col;
        Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
        
        std::cout << "Moviéndose a la META en (" << nextR << ", " << nextC << ")" << std::endl;
        handlePlayerMovement(dir, player, grid);
        
        // IMPORTANTE: Solo detener auto-movimiento DESPUÉS de verificar si realmente ganó
        // La detección de victoria ocurre dentro de handlePlayerMovement
        if (player.hasWon) {
            player.isAutoMoving = false;
            std::cout << "¡¡¡VICTORIA AUTOMÁTICA CONFIRMADA!!!" << std::endl;
        } else {
            // Si por alguna razón no ganó, verificar manualmente
            if (player.row == goalRow && player.col == goalCol) {
                std::cout << "Jugador en posición de meta pero hasWon no está activo. Verificando..." << std::endl;
            }
        }
        return;
    }

    // Verificar si el siguiente paso es accesible
    if (!grid.inBounds(nextR, nextC)) {
        std::cout << "Paso fuera de límites, recalculando..." << std::endl;
        if (!recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            player.isAutoMoving = false;
        }
        return;
    }

    // MANEJO INTELIGENTE DE PAREDES
    if (grid.at(nextR, nextC).type == CellType::WALL) {
        if (player.energy >= TurnSystem::ENERGY_PER_WALL_BREAK) {
            // CASO 1: Tiene energía suficiente - romper la pared y continuar
            std::cout << "Rompiendo pared en (" << nextR << ", " << nextC << ") - Energía: " << player.energy << std::endl;
            
            grid.at(nextR, nextC).type = CellType::EMPTY;
            player.energy -= TurnSystem::ENERGY_PER_WALL_BREAK;
            
            // Continuar con el movimiento después de romper la pared
            int dR = nextR - player.row;
            int dC = nextC - player.col;
            Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
            handlePlayerMovement(dir, player, grid);
            
            std::cout << "Pared rota. Energía restante: " << player.energy << std::endl;
            
        } else {
            // CASO 2: No tiene energía suficiente - recalcular camino que evite la pared
            std::cout << "Sin energía para romper pared (" << player.energy << "/" << TurnSystem::ENERGY_PER_WALL_BREAK << "). Buscando ruta alternativa..." << std::endl;
            
            if (recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
                std::cout << "Ruta alternativa encontrada. Continuando..." << std::endl;
                // No hacer nada más aquí - el siguiente frame procesará el nuevo camino
            } else {
                std::cout << "No hay ruta alternativa disponible. Deteniendo auto-movimiento." << std::endl;
                player.isAutoMoving = false;
            }
        }
        return;
    }

    // VERIFICAR SI LA CELDA DESTINO SE CONVIRTIÓ EN PARED DESDE LA ÚLTIMA VEZ
    if (grid.at(nextR, nextC).type == CellType::WALL) {
        std::cout << "Nueva pared detectada en el camino. Recalculando..." << std::endl;
        if (!recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            player.isAutoMoving = false;
        }
        return;
    }

    // MOVIMIENTO NORMAL
    int dR = nextR - player.row;
    int dC = nextC - player.col;
    
    // Verificar que el movimiento es válido (células adyacentes en hexágono)
    if (abs(dR) > 1 || abs(dC) > 1) {
        std::cout << "Paso muy lejano detectado. Recalculando camino..." << std::endl;
        if (!recalculatePathInAutoMovement(grid, player, pathCells, goalRow, goalCol)) {
            player.isAutoMoving = false;
        }
        return;
    }
    
    // Ejecutar el movimiento
    Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
    
    std::cout << "Ejecutando movimiento normal: dirección " << static_cast<int>(dir) << std::endl;
    std::cout << "Delta: (" << dR << ", " << dC << ") desde (" << player.row << ", " << player.col << ") hacia (" << nextR << ", " << nextC << ")" << std::endl;
    
    handlePlayerMovement(dir, player, grid);
    
    // Verificar si el movimiento fue exitoso
    if (player.row == nextR && player.col == nextC) {
        std::cout << "Movimiento exitoso a (" << player.row << ", " << player.col << ")" << std::endl;
    } else {
        std::cout << "¡ADVERTENCIA! Movimiento no exitoso. Posición esperada: (" << nextR << ", " << nextC << "), posición actual: (" << player.row << ", " << player.col << ")" << std::endl;
    }
    
    // Debug: mostrar tipo de celda actual
    CellType currentCellType = grid.at(player.row, player.col).type;
    std::cout << "Tipo de celda actual: " << static_cast<int>(currentCellType) << std::endl;
}

}