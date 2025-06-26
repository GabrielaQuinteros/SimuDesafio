#include "Player.hpp"
#include "HexGrid.hpp"
#include <cmath>

namespace model
{
    Player::Player(int r, int c) : row(r), col(c)
    {
        energy = 0;
        canBreakWall = false;
    }

    void Player::gainEnergy()
    {
        if (energy < MAX_ENERGY)
        {
            energy += ENERGY_PER_MOVE;

            if (energy >= MAX_ENERGY)
            {
                canBreakWall = true;
            }
        }
    }

    void Player::resetEnergy()
    {
        energy = 0;
        canBreakWall = false;
    }

    bool Player::isEnergyFull() const
    {
        return energy >= MAX_ENERGY;
    }

    float Player::getEnergyPercentage() const
    {
        return static_cast<float>(energy) / static_cast<float>(MAX_ENERGY);
    }

    void Player::useWallBreak()
    {
        if (canUseWallBreak())
        {
            resetEnergy();
        }
    }

    bool Player::canUseWallBreak() const
    {
        return canBreakWall;
    }

    void Player::startMovement(sf::Vector2f start, sf::Vector2f target)
    {
        isMoving = true;
        startPosition = start;
        targetPosition = target;
        currentPosition = start;
        movementClock.restart();
    }

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