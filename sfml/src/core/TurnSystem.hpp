#ifndef TURNSYSTEM_HPP
#define TURNSYSTEM_HPP


#include "../model/HexGrid.hpp"
#include "../model/Player.hpp"
#include <vector>
#include <utility>


using namespace model;
using namespace std;


class TurnSystem
{
private:
    static int turnCounter;
    static const int TURNS_PER_WALL = 5;
    static bool randomInitialized;

    static vector<pair<int, int>> findAvailableCells(const HexGrid &grid, const Player &player);
    static void generateRandomWall(HexGrid &grid, const Player &player);
    static void initializeRandom();


public:
    static constexpr int ENERGY_PER_WALL_BREAK = 5;
    static void handleTurn(HexGrid &grid, const Player &player);


    static int getCurrentTurnCount();
    static void resetTurnCounter();
    static void setTurnsPerWall(int turns);
    static int getTurnsPerWall();

    static bool shouldGenerateWall();
};


#endif
