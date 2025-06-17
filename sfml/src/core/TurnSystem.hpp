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

    // Métodos privados para la lógica interna
    static vector<pair<int, int>> findAvailableCells(const HexGrid &grid, const Player &player);
    static void generateRandomWall(HexGrid &grid, const Player &player);
    static void initializeRandom();

public:
    // Método principal para manejar el sistema de turnos
    static void handleTurn(HexGrid &grid, const Player &player);

    // Métodos de utilidad
    static int getCurrentTurnCount();
    static void resetTurnCounter();
    static void setTurnsPerWall(int turns);
    static int getTurnsPerWall();

    // Método para verificar si es momento de generar un muro
    static bool shouldGenerateWall();
};

#endif