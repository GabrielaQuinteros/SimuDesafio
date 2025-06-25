#include "Player.hpp"
#include "HexGrid.hpp"
#include <cmath>
#include <iostream>      


namespace model
{


    // Constructor de la clase Player
    Player::Player(int r, int c) : row(r), col(c)
    {
        // Inicializa las coordenadas del jugador en el grid (fila y columna)
        energy = 0;
        canBreakWall = false;
    }


    // Método para ganar energía al moverse
    void Player::gainEnergy()
    {
        if (energy < MAX_ENERGY)
        {
            energy += ENERGY_PER_MOVE;
            std::cout << "Energía ganada! Energía actual: " << energy << "/" << MAX_ENERGY << std::endl;


            // Actualizar flag cuando la energía esté llena
            if (energy >= MAX_ENERGY)
            {
                canBreakWall = true;
                std::cout << "¡Barra de energía llena! Puedes romper una pared presionando SPACE." << std::endl;
            }
        }
    }


    // Resetear energía después de usar la habilidad
    void Player::resetEnergy()
    {
        energy = 0;
        canBreakWall = false;
    }


    // Verificar si la energía está llena
    bool Player::isEnergyFull() const
    {
        return energy >= MAX_ENERGY;
    }


    // Obtener porcentaje de energía para la barra visual
    float Player::getEnergyPercentage() const
    {
        return static_cast<float>(energy) / static_cast<float>(MAX_ENERGY);
    }


    // Usar la habilidad de romper pared - AHORA PUEDE USARSE MÚLTIPLES VECES
    void Player::useWallBreak()
    {
        if (canUseWallBreak())
        {
            resetEnergy(); // Solo resetea la energía, no marca como "usado para siempre"
            std::cout << "¡Has roto una pared! Puedes volver a usar la habilidad cuando tu energía se llene de nuevo." << std::endl;
        }
    }


    // Verificar si puede usar la habilidad de romper pared - SOLO REQUIERE ENERGÍA LLENA
    bool Player::canUseWallBreak() const
    {
        return canBreakWall; // Solo verifica que tenga energía llena
    }


    // Método para iniciar una animación de movimiento
    void Player::startMovement(sf::Vector2f start, sf::Vector2f target)
    {
        isMoving = true;
        startPosition = start;
        targetPosition = target;
        currentPosition = start;
        movementClock.restart();
    }


    // Método para actualizar la posición del jugador durante la animación
    void Player::updateMovement()
    {
        if (!isMoving)
            return;


        float elapsed = movementClock.getElapsedTime().asSeconds();
        float progress = elapsed / movementDuration;


        if (progress >= 1.0f)
        {
            isMoving = false;
            currentPosition = targetPosition;
        }
        else
        {
            float easedProgress = 0.5f * (1.0f - cos(progress * 3.14159f));
            currentPosition.x = startPosition.x + (targetPosition.x - startPosition.x) * easedProgress;
            currentPosition.y = startPosition.y + (targetPosition.y - startPosition.y) * easedProgress;
        }
    }


    // Método para obtener la posición visual actual del jugador
    sf::Vector2f Player::getVisualPosition(const HexGrid& grid) const
    {
        if (isMoving)
        {
            return currentPosition;
        }
        else
        {
            return grid.toPixel(row, col);
        }
    }
}
