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
        static const int MAX_ENERGY = 10;
        static const int ENERGY_PER_MOVE = 1;
        bool canBreakWall = false;

        bool isSelectingWall = false;

        bool hasWon = false;
        float winTime = 0.0f;
        sf::Clock winClock;

        model::CellType lastCellType = model::CellType::EMPTY;

        bool isAutoMoving = false;

        bool isMoving = false;
        sf::Vector2f startPosition;
        sf::Vector2f targetPosition;
        sf::Vector2f currentPosition;
        sf::Clock movementClock;
        float movementDuration = 0.3f;

        Player(int r, int c);

        void gainEnergy();
        void resetEnergy();
        bool isEnergyFull() const;
        float getEnergyPercentage() const;

        void useWallBreak();
        bool canUseWallBreak() const;

        void startMovement(sf::Vector2f start, sf::Vector2f target);
        void updateMovement();
        sf::Vector2f getVisualPosition(const class HexGrid& grid) const;
    };
}