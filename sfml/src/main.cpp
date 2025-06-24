#include <string>
#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "utils/Utils.hpp"
#include "model/HexCell.hpp"
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include "core/GameLogic.hpp"
#include "core/TurnSystem.hpp"
#include "render/Renderer.hpp"
#include "core/PathFinding.hpp"
#include "core/AutoMovement.hpp"
#include <iostream>
#include <vector> // CAMBIO: vector en lugar de set

// Definición de constantes para la ventana y recursos
#define WINDOW_WIDTH 900  // Aumentado para mejor UI
#define WINDOW_HEIGHT 600 // Aumentado para mejor UI
#define MAP_PATH "resources/map.txt"
#define FONT_PATH "resources/arial.ttf"

using namespace model;
using namespace sf;

int main()
{
    // Cargar mapa desde archivo
    HexGrid grid = loadHexGridFromFile(MAP_PATH);

    // Obtener celda de inicio
    HexCell *start = findStartCell(grid);
    // Obtener celda de meta
    HexCell *goal = findGoalCell(grid);

    if (!start)
        return 1;
    if (!goal)
        return 1;

    // Crear el jugador en la posición inicial
    Player player(start->row, start->col);

    // Inicializar el sistema de turnos
    TurnSystem::resetTurnCounter();

    // Crear la ventana de renderizado con resolución mejorada
    RenderWindow window({WINDOW_WIDTH, WINDOW_HEIGHT},
                        "HexEscape: Fabrica de Rompecabezas Elite",
                        Style::Titlebar | Style::Close);
    window.setFramerateLimit(60); // 60 FPS para animaciones ultra suaves

    // Cargar la fuente para el texto
    Font font;
    if (!font.loadFromFile(FONT_PATH))
    {
        return 1;
    }

    // Crear elementos gráficos
    Text texto = createText(font, 16, Color::White);
    CircleShape hexagon = createHexagon();

    // Relojes para animaciones ultra precisas
    Clock animationClock;
    Clock backgroundClock;
    Clock victoryClock; // Para efectos de la pantalla de victoria

    // Variables de estado del juego
    bool gameWon = false;
    bool showVictoryScreen = false;
    std::vector<std::pair<int, int>> pathCells; // CAMBIO: vector en lugar de set

    // Bucle principal del juego ultra optimizado
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
            {
                // Si ya ganó, solo permitir salir con ESC
                if (event.key.code == Keyboard::Escape)
                {
                    if (showVictoryScreen)
                    {
                        window.close();
                    }
                }
                else if (event.key.code == Keyboard::P)
                {
                    PathfindingResult path = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);

                    for (const auto &cell : path.path)
                    {
                        std::cout << "(" << cell->row << ", " << cell->col << ") ";
                    }
                    std::cout << "\n";

                    // CORREGIDO: Limpiar y llenar el vector en el orden correcto
                    pathCells.clear();

                    pathCells.clear();
                    for (auto *cell : path.path)
                    {
                        int r = cell->row, c = cell->col;
                        // Ignoramos el primer paso si es la posición actual
                        if (r == player.row && c == player.col)
                            continue;
                        pathCells.emplace_back(r, c);
                    }
                }
                if (!pathCells.empty())
                {
                    player.isAutoMoving = true;
                    std::cout << "AutoMovement activado con " << pathCells.size() << " pasos.\n";
                }

                else
                {
                    // Juego normal
                    handlePlayerMovement(event.key.code, player, grid);
                }
            }
        }

        core::updateAutoMovement(
            grid, player, pathCells,
            goal->row, goal->col);

        // Verificar condición de victoria
        if (!gameWon && player.hasWon)
        {
            gameWon = true;
            showVictoryScreen = true;
            victoryClock.restart(); // Iniciar efectos de victoria
        }

        // Aplicar efectos de bandas transportadoras solo si no ha ganado
        if (!showVictoryScreen)
        {
            handleConveyorMovement(player, grid);
        }

        // Limpiar ventana con color de fondo ultra moderno
        window.clear(Color(5, 10, 20)); // Fondo más oscuro y profesional

        if (showVictoryScreen)
        {
            // Mostrar pantalla de victoria épica
            drawVictoryScreen(window, font, player.winTime,
                              TurnSystem::getCurrentTurnCount(), victoryClock);
        }
        else
        {
            // Dibujar el juego normal con interfaz ultra moderna
            drawGrid(window, grid, player, hexagon, texto, font, animationClock, backgroundClock, pathCells);

            // Dibujar UI ultra profesional
            drawModernEnergyBar(window, player, font, animationClock);
            drawGameInfo(window, font, TurnSystem::getCurrentTurnCount(), animationClock);
            drawModernControls(window, font, animationClock);
        }

        window.display();
    }

    return 0;
}