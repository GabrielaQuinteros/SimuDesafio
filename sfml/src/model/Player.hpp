#pragma once
#include "HexCell.hpp"
#include <SFML/System.hpp>

namespace model
{

    class Player
    {
    public:
        int row;
        int col;
        int energy = 0;
        bool usedWallBreak = false;
        model::CellType lastCellType = model::CellType::EMPTY;

        // Sistema de animación para el movimiento
        bool isMoving = false;
        sf::Vector2f startPosition;
        sf::Vector2f targetPosition;
        sf::Vector2f currentPosition;
        sf::Clock movementClock;
        float movementDuration = 0.3f; // 300ms para el movimiento

        Player(int r, int c);

        // Inicia una animación de movimiento
        void startMovement(sf::Vector2f start, sf::Vector2f target);

        // Actualiza la posición durante la animación
        void updateMovement();

        // Obtiene la posición visual actual del jugador
        sf::Vector2f getVisualPosition(const class HexGrid &grid) const;
    };
}