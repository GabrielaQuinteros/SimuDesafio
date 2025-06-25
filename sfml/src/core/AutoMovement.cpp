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
        player.isAutoMoving = false;
        std::cout << "¡Camino completado!" << std::endl;
        return;
    }

    // Obtener el siguiente paso
    int nextR = pathCells[currentIndex].first;
    int nextC = pathCells[currentIndex].second;

    // Verificar si el siguiente paso es la meta
    if (nextR == goalRow && nextC == goalCol && grid.at(nextR, nextC).type == CellType::GOAL) {
        // Es la meta, moverse directamente
        int dR = nextR - player.row;
        int dC = nextC - player.col;
        Keyboard::Key dir = keyFromDelta(dR, dC, player.row);
        
        std::cout << "¡Llegando a la meta!" << std::endl;
        handlePlayerMovement(dir, player, grid);
        
        // Si llegamos a la meta, detener el auto-movimiento
        if (player.hasWon) {
            player.isAutoMoving = false;
            std::cout << "¡¡¡VICTORIA AUTOMÁTICA!!!" << std::endl;
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
    handlePlayerMovement(dir, player, grid);
    
    // Debug: mostrar progreso
    std::cout << "Auto-movimiento: (" << player.row << ", " << player.col << ") -> (" << nextR << ", " << nextC << ")" << std::endl;
}

}