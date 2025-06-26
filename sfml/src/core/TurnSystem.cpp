#include "TurnSystem.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace model;

int TurnSystem::turnCounter = 0;
const int TurnSystem::TURNS_PER_WALL;
bool TurnSystem::randomInitialized = false;

void TurnSystem::initializeRandom() {
    if (!randomInitialized) {
        srand(static_cast<unsigned int>(time(nullptr)));
        randomInitialized = true;
    }
}

vector<pair<int, int>> TurnSystem::findAvailableCells(const HexGrid &grid, const Player &player) {
    vector<pair<int, int>> availableCells;
   
    for (int row = 0; row < grid.rows(); ++row) {
        for (int col = 0; col < grid.cols(); ++col) {
            const HexCell &cell = grid.at(row, col);
           
            if (cell.type == CellType::EMPTY &&
                !(row == player.row && col == player.col)) {
                availableCells.push_back({row, col});
            }
        }
    }
   
    return availableCells;
}

void TurnSystem::generateRandomWall(HexGrid &grid, const Player &player) {
    initializeRandom();
   
    vector<pair<int, int>> availableCells = findAvailableCells(grid, player);
   
    if (availableCells.empty()) {
        return;
    }
   
    int randomIndex = rand() % availableCells.size();
    auto [row, col] = availableCells[randomIndex];
   
    grid.at(row, col).type = CellType::WALL;
}

void TurnSystem::handleTurn(HexGrid &grid, const Player &player) {
    turnCounter++;
   
    if (shouldGenerateWall()) {
        generateRandomWall(grid, player);
    }
}

int TurnSystem::getCurrentTurnCount() {
    return turnCounter;
}

void TurnSystem::resetTurnCounter() {
    turnCounter = 0;
}

void TurnSystem::setTurnsPerWall(int turns) {
    // Funcion vacia - TURNS_PER_WALL es constante
}

int TurnSystem::getTurnsPerWall() {
    return TURNS_PER_WALL;
}

bool TurnSystem::shouldGenerateWall() {
    return (turnCounter % TURNS_PER_WALL == 0);
}