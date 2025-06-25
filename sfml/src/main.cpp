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
#include <vector>

// Definición de constantes para la ventana y recursos
#define WINDOW_WIDTH 1400   
#define WINDOW_HEIGHT 900   
#define MAP_PATH "resources/map.txt"
#define FONT_PATH "resources/arial.ttf"

using namespace model;
using namespace sf;

// MEJORADA: Verificar si el camino actual sigue siendo válido
bool isPathStillValid(const HexGrid& grid, const std::vector<std::pair<int, int>>& pathCells, 
                     int playerRow, int playerCol) {
    if (pathCells.empty()) return false;
    
    // Verificar que el jugador esté en algún punto del camino o cerca del inicio
    bool playerInPath = false;
    for (size_t i = 0; i < pathCells.size(); ++i) {
        if (pathCells[i].first == playerRow && pathCells[i].second == playerCol) {
            playerInPath = true;
            break;
        }
    }
    
    // Si el jugador no está en el camino, verificar si está cerca del inicio
    if (!playerInPath && !pathCells.empty()) {
        const auto& firstStep = pathCells[0];
        int distance = abs(firstStep.first - playerRow) + abs(firstStep.second - playerCol);
        if (distance > 2) {
            return false; // El jugador está muy lejos del camino
        }
    }
    
    // Verificar que no haya paredes bloqueando el camino
    for (const auto& cell : pathCells) {
        CellType cellType = grid.at(cell.first, cell.second).type;
        if (cellType == CellType::WALL) {
            return false;
        }
    }
    
    return true;
}

// MEJORADA: Recalcular camino con mejor manejo de estados
bool recalculatePath(HexGrid& grid, Player& player, HexCell* goal,
                    std::vector<std::pair<int, int>>& pathCells,
                    bool& showPathVisualization, bool& autoSolveMode) {
   
    std::cout << "Recalculando camino desde main..." << std::endl;
    PathfindingResult newPath = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
   
    if (newPath.success && !newPath.path.empty()) {
        // Actualizar el camino
        pathCells.clear();
        for (auto *cell : newPath.path) {
            pathCells.emplace_back(cell->row, cell->col);
        }
        
        std::cout << "Camino recalculado exitosamente con " << pathCells.size() << " pasos." << std::endl;
        return true;
    } else {
        // No hay camino disponible - limpiar todo
        std::cout << "No se pudo recalcular el camino. Limpiando estado." << std::endl;
        pathCells.clear();
        showPathVisualization = false;
        autoSolveMode = false;
        player.isAutoMoving = false;
        
        return false;
    }
}

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

    // VENTANA MÁS GRANDE Y RESPONSIVE
    RenderWindow window({WINDOW_WIDTH, WINDOW_HEIGHT},
                        "HexEscape: Fabrica de Rompecabezas Elite",
                        Style::Titlebar | Style::Close | Style::Resize);
    window.setFramerateLimit(60);

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
    Clock victoryClock;

    // Variables de estado del juego
    bool gameWon = false;
    bool showVictoryScreen = false;
    std::vector<std::pair<int, int>> pathCells; // Vector para mostrar el camino

    // VARIABLES PARA LOS MODOS:
    bool showPathVisualization = false;  // Para mostrar el camino sin ejecutar
    bool autoSolveMode = false;          // Para activar resolución automática

    // NUEVAS VARIABLES PARA MEJOR CONTROL:
    int lastPlayerRow = -1;
    int lastPlayerCol = -1;
    int lastTurnCount = 0;
    bool pathNeedsUpdate = false;

    // Bucle principal del juego ultra optimizado
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            // Manejar redimensionamiento de ventana
            if (event.type == Event::Resized)
            {
                FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(View(visibleArea));
            }

            if (event.type == Event::KeyPressed)
            {
                // Si ya ganó, solo permitir salir con ESC
                if (event.key.code == Keyboard::Escape)
                {
                    if (showVictoryScreen)
                    {
                        window.close();
                    }
                    else
                    {
                        // ESC cancela todo y vuelve al modo manual
                        std::cout << "=== CANCELANDO TODOS LOS MODOS - VOLVIENDO A MANUAL ===" << std::endl;
                        showPathVisualization = false;
                        autoSolveMode = false;
                        player.isAutoMoving = false;
                        pathCells.clear();
                        lastPlayerRow = -1;
                        lastPlayerCol = -1;
                        std::cout << "Modo manual activado. Usa P para mostrar camino, R para auto-resolución." << std::endl;
                    }
                }
                else if (event.key.code == Keyboard::P && !showVictoryScreen && !autoSolveMode)
                {
                    // MODO VISUALIZACIÓN: Solo mostrar el camino
                    std::cout << "=== MODO VISUALIZACIÓN ACTIVADO ===" << std::endl;
                    PathfindingResult path = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
                   
                    if (path.success && !path.path.empty()) {
                        pathCells.clear();
                        for (auto *cell : path.path)
                        {
                            pathCells.emplace_back(cell->row, cell->col);
                        }
                       
                        showPathVisualization = true;
                        autoSolveMode = false;
                        player.isAutoMoving = false;
                        
                        // Guardar posición del jugador
                        lastPlayerRow = player.row;
                        lastPlayerCol = player.col;
                        
                        std::cout << "Camino mostrado con " << pathCells.size() << " pasos. Presiona T para ejecutar." << std::endl;
                    } else {
                        // No hay camino disponible
                        pathCells.clear();
                        showPathVisualization = false;
                        lastPlayerRow = -1;
                        lastPlayerCol = -1;
                        std::cout << "No se encontró camino hacia la meta." << std::endl;
                    }
                }
                else if (event.key.code == Keyboard::R && !showVictoryScreen)
                {
                    // MODO AUTO-RESOLUCIÓN DIRECTA
                    std::cout << "=== MODO AUTO-RESOLUCIÓN ACTIVADO ===" << std::endl;
                    PathfindingResult path = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
                   
                    if (path.success && !path.path.empty()) {
                        pathCells.clear();
                        for (auto *cell : path.path)
                        {
                            pathCells.emplace_back(cell->row, cell->col);
                        }
                       
                        autoSolveMode = true;
                        player.isAutoMoving = true;
                        showPathVisualization = false; // Modo directo sin visualización
                        
                        lastPlayerRow = player.row;
                        lastPlayerCol = player.col;
                        
                        std::cout << "Auto-resolución iniciada con " << pathCells.size() << " pasos. El jugador se moverá automáticamente." << std::endl;
                    } else {
                        std::cout << "No se encontró camino para auto-resolución." << std::endl;
                    }
                }
                else if (event.key.code == Keyboard::T && showPathVisualization && !autoSolveMode && !showVictoryScreen)
                {
                    // EJECUTAR EL CAMINO YA MOSTRADO
                    if (!pathCells.empty()) {
                        std::cout << "=== EJECUTANDO CAMINO MOSTRADO ===" << std::endl;
                        autoSolveMode = true;
                        player.isAutoMoving = true;
                        // Mantener showPathVisualization = true para seguir viendo el camino
                        
                        std::cout << "Ejecutando camino con " << pathCells.size() << " pasos. El camino permanecerá visible." << std::endl;
                    }
                }
                else if (!showVictoryScreen && !autoSolveMode && !player.isAutoMoving)
                {
                    // MODO MANUAL: Solo permitir movimiento manual si NO está en auto-resolución
                   
                    // Recordar posición antes del movimiento
                    int oldRow = player.row;
                    int oldCol = player.col;
                   
                    handlePlayerMovement(event.key.code, player, grid);
                   
                    // Si el jugador se movió manualmente, limpiar visualización
                    if (player.row != oldRow || player.col != oldCol) {
                        if (showPathVisualization) {
                            pathCells.clear();
                            showPathVisualization = false;
                            lastPlayerRow = -1;
                            lastPlayerCol = -1;
                        }
                    }
                }
            }
        }

        // DETECCIÓN DE CAMBIOS DE POSICIÓN DEL JUGADOR (por bandas transportadoras)
        if (!autoSolveMode && showPathVisualization) {
            if (player.row != lastPlayerRow || player.col != lastPlayerCol) {
                // El jugador se movió desde que se calculó el camino
                // Verificar si el camino sigue siendo válido
                if (!isPathStillValid(grid, pathCells, player.row, player.col)) {
                    pathCells.clear();
                    showPathVisualization = false;
                    lastPlayerRow = -1;
                    lastPlayerCol = -1;
                }
            }
        }

        // DETECCIÓN DE NUEVAS PAREDES
        int currentTurnCount = TurnSystem::getCurrentTurnCount();
        if (currentTurnCount != lastTurnCount) {
            lastTurnCount = currentTurnCount;
            pathNeedsUpdate = true;
        }

        // VERIFICACIÓN Y RECÁLCULO DE CAMINOS
        if (pathNeedsUpdate && !pathCells.empty() && (showPathVisualization || autoSolveMode)) {
            pathNeedsUpdate = false;
            
            if (!isPathStillValid(grid, pathCells, player.row, player.col)) {
                // El camino está bloqueado, intentar recalcular
                bool wasExecuting = autoSolveMode;
                
                if (!recalculatePath(grid, player, goal, pathCells, showPathVisualization, autoSolveMode)) {
                    // No se pudo recalcular, detener todo
                    if (wasExecuting) {
                        autoSolveMode = false;
                        player.isAutoMoving = false;
                    }
                }
            }
        }

        // ACTUALIZAR AUTO-MOVIMIENTO
        if (autoSolveMode && player.isAutoMoving) {
            core::updateAutoMovement(grid, player, pathCells, goal->row, goal->col);
           
            // Si terminó el auto-movimiento, actualizar estado
            if (!player.isAutoMoving) {
                std::cout << "=== AUTO-MOVIMIENTO COMPLETADO ===" << std::endl;
                autoSolveMode = false;
                
                // Si no estaba en modo visualización, limpiar el camino
                if (!showPathVisualization) {
                    pathCells.clear();
                    std::cout << "Camino limpiado. Volviendo a modo manual." << std::endl;
                } else {
                    std::cout << "Camino mantenido visible. Presiona ESC para limpiar." << std::endl;
                }
                
                lastPlayerRow = player.row;
                lastPlayerCol = player.col;
            }
        }

        // VERIFICAR CONDICIÓN DE VICTORIA
        if (!gameWon && player.hasWon)
        {
            gameWon = true;
            showVictoryScreen = true;
            autoSolveMode = false;
            showPathVisualization = false;
            player.isAutoMoving = false;
            pathCells.clear();
            lastPlayerRow = -1;
            lastPlayerCol = -1;
            victoryClock.restart();
        }

        // APLICAR EFECTOS DE BANDAS TRANSPORTADORAS
        if (!showVictoryScreen)
        {
            handleConveyorMovement(player, grid);
        }

        // RENDERIZAR
        window.clear(Color(5, 10, 20));

        if (showVictoryScreen)
        {
            drawVictoryScreen(window, font, player.winTime,
                              TurnSystem::getCurrentTurnCount(), victoryClock);
        }
        else
        {
            // Dibujar el juego normal
            drawGrid(window, grid, player, hexagon, texto, font, animationClock, backgroundClock, pathCells);

            // Dibujar UI
            drawModernEnergyBar(window, player, font, animationClock);
            drawGameInfo(window, font, TurnSystem::getCurrentTurnCount(), animationClock,
                        showPathVisualization, autoSolveMode);
            drawModernControls(window, font, animationClock);
        }

        window.display();
    }

    return 0;
}