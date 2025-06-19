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

        // Sistema de energía
        int energy = 0;
        static const int MAX_ENERGY = 10; // Energía máxima para romper una pared
        static const int ENERGY_PER_MOVE = 1; // Energía ganada por cada movimiento
        bool canBreakWall = false; // Flag que indica si puede romper una pared
        // REMOVIDO: hasUsedWallBreak - ahora puede usar múltiples veces

        model::CellType lastCellType = model::CellType::EMPTY;

        // Sistema de animación para el movimiento
        bool isMoving = false;
        sf::Vector2f startPosition;
        sf::Vector2f targetPosition;
        sf::Vector2f currentPosition;
        sf::Clock movementClock;
        float movementDuration = 0.3f; // 300ms para el movimiento

        Player(int r, int c);

        // Métodos de energía
        void gainEnergy();
        void resetEnergy();
        bool isEnergyFull() const;
        float getEnergyPercentage() const;

        // Método para usar la habilidad de romper pared
        void useWallBreak();
        bool canUseWallBreak() const;

        // Inicia una animación de movimiento
        void startMovement(sf::Vector2f start, sf::Vector2f target);

        // Actualiza la posición durante la animación
        void updateMovement();

        // Obtiene la posición visual actual del jugador
        sf::Vector2f getVisualPosition(const class HexGrid& grid) const;
    };
}