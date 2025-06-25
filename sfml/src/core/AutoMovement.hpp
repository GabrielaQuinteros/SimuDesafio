#ifndef AUTOMOVEMENT_HPP
#define AUTOMOVEMENT_HPP

#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include <vector>

namespace core {

/**
 * @brief Actualiza el movimiento automático del jugador siguiendo un camino predefinido
 * 
 * @param grid La grilla hexagonal del juego
 * @param player Referencia al jugador
 * @param pathCells Vector con las coordenadas del camino a seguir
 * @param goalRow Fila de la meta
 * @param goalCol Columna de la meta
 */
void updateAutoMovement(
    model::HexGrid& grid,
    model::Player& player,
    std::vector<std::pair<int, int>>& pathCells,
    int goalRow,
    int goalCol
);

}

#endif // AUTOMOVEMENT_HPP