#include "TurnSystem.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>


using namespace std;
using namespace model;


// Inicialización de variables estáticas
int TurnSystem::turnCounter = 0;
const int TurnSystem::TURNS_PER_WALL;
bool TurnSystem::randomInitialized = false;


void TurnSystem::initializeRandom() {
    if (!randomInitialized) {
        srand(static_cast<unsigned int>(time(nullptr)));
        randomInitialized = true;
    }
}


// FUNCIÓN MEJORADA: Solo considera celdas EMPTY para generar paredes
vector<pair<int, int>> TurnSystem::findAvailableCells(const HexGrid &grid, const Player &player) {
    vector<pair<int, int>> availableCells;
   
    for (int row = 0; row < grid.rows(); ++row) {
        for (int col = 0; col < grid.cols(); ++col) {
            const HexCell &cell = grid.at(row, col);
           
            // MEJORADO: Solo celdas EMPTY pueden convertirse en paredes
            // No pueden ser: WALL, START, posición del jugador, o bandas transportadoras
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
        cout << "No hay celdas EMPTY disponibles para colocar un nuevo muro.\n";
        return;
    }
   
    // Seleccionar una celda aleatoria
    int randomIndex = rand() % availableCells.size();
    auto [row, col] = availableCells[randomIndex];
   
    // Cambiar el tipo de celda a WALL
    grid.at(row, col).type = CellType::WALL;
   
    cout << "¡Nuevo muro generado en posición (" << row << ", " << col << ")!\n";
}


void TurnSystem::handleTurn(HexGrid &grid, const Player &player) {
    turnCounter++;
   
    cout << "Turno: " << turnCounter << endl;
   
    // Verificar si es momento de generar un nuevo muro
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
    cout << "Advertencia: TURNS_PER_WALL es constante y no puede ser modificado.\n";
}


int TurnSystem::getTurnsPerWall() {
    return TURNS_PER_WALL;
}


bool TurnSystem::shouldGenerateWall() {
    return (turnCounter % TURNS_PER_WALL == 0);
}
