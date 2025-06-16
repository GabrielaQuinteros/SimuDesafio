#include "GameLogic.hpp"
#include "model/HexCell.hpp"
#include "utils/Utils.hpp"
#include "model/Player.hpp"
#include <SFML/System.hpp>
#include <iostream>

using namespace model;
using namespace sf;

HexCell *findStartCell(HexGrid &grid)
{
    for (int y = 0; y < grid.rows(); ++y)
    {
        for (int x = 0; x < grid.cols(); ++x)
        {
            if (grid.at(y, x).type == CellType::START)
                // Devuelve la coordenada de inicio
                return &grid.at(y, x);
        }
    }
    // Si no encuentra ninguna devuelve error
    std::cerr << "No se encontró celda de inicio (START)\n";
    return nullptr;
}

void handlePlayerMovement(Keyboard::Key key, Player &player, const HexGrid &grid)
{
    // No permitir movimiento manual si el jugador está siendo animado
    if (player.isMoving)
        return;

    int row = player.row, col = player.col;
    int newRow = row, newCol = col;
    bool isOdd = row % 2 != 0;
    bool moved = false;

    // Movimiento en el grid según la tecla presionada
    if (key == Keyboard::W)
    {
        newRow = row - 1;
        newCol = isOdd ? col : col - 1;
        moved = true;
    }
    else if (key == Keyboard::E)
    {
        newRow = row - 1;
        newCol = isOdd ? col + 1 : col;
        moved = true;
    }
    else if (key == Keyboard::A)
    {
        newRow = row;
        newCol = col - 1;
        moved = true;
    }
    else if (key == Keyboard::D)
    {
        newRow = row;
        newCol = col + 1;
        moved = true;
    }
    else if (key == Keyboard::Z)
    {
        newRow = row + 1;
        newCol = isOdd ? col : col - 1;
        moved = true;
    }
    else if (key == Keyboard::X)
    {
        newRow = row + 1;
        newCol = isOdd ? col + 1 : col;
        moved = true;
    }

    // Revisa que el siguiente paso no sea 'out of bounds'
    if (moved && newRow >= 0 && newRow < grid.rows() && newCol >= 0 && newCol < grid.cols())
    {
        // Obtiene la celda objetivo a donde se intenta mover el jugador
        const auto &target = grid.at(newRow, newCol);

        // Restringe el movimiento si la celda es de tipo WALL
        //Si es otro tipo de celda deja acceder a ella
        if (target.type != CellType::WALL)
        {
            // Iniciar animación de movimiento desde la posición de inicio hasta el target
            Vector2f startPos = grid.toPixel(player.row, player.col);
            Vector2f targetPos = grid.toPixel(newRow, newCol);
            player.startMovement(startPos, targetPos);

            //Guarda la útlima celda en la que estaba la ficha/jugador
            player.lastCellType = grid.at(player.row, player.col).type;
            //Actualiza las nueva posición en el grid
            player.row = newRow;
            player.col = newCol;
        }
    }
}

void handleConveyorMovement(Player &player, const HexGrid &grid)
{
    // No aplicar banda transportadora si el jugador está siendo animado
    if (player.isMoving)
        return;

    //Obtengo la celda actual del jugador
    const HexCell &current = grid.at(player.row, player.col);

    // Solo aplicar movimiento automático si el jugador está en una banda transportadora
    if (!(current.type >= CellType::UP_RIGHT && current.type <= CellType::DOWN_LEFT))
        return; 

    bool isOdd = player.row % 2 != 0;
    //Calcula el desplazamiento que debe hacerse según cada tipo de banda transportadora
    std::pair<int, int> offset = getConveyorOffset(current.type, isOdd);
    //Nuevas coordenadas después del desplazamiento
    int newRow = player.row + offset.first;
    int newCol = player.col + offset.second;

    //Verifica que las nuevas posiciones no estén fuera de los límites del grid 
    if (newRow >= 0 && newRow < grid.rows() && newCol >= 0 && newCol < grid.cols())
    {
        //Verifica que el target no sea un muro para comenzar la animación
        const auto &target = grid.at(newRow, newCol);
        if (target.type != CellType::WALL)
        {
            // Iniciar animación de movimiento por banda transportadora
            Vector2f startPos = grid.toPixel(player.row, player.col);
            Vector2f targetPos = grid.toPixel(newRow, newCol);
            player.startMovement(startPos, targetPos);

            //Actualiza la posición
            player.lastCellType = current.type;
            player.row = newRow;
            player.col = newCol;
        }
    }
}