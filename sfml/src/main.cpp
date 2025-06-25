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
#define WINDOW_WIDTH 900  // Aumentado para mejor UI
#define WINDOW_HEIGHT 600 // Aumentado para mejor UI
#define MAP_PATH "resources/map.txt"
#define FONT_PATH "resources/arial.ttf"


using namespace model;
using namespace sf;


// NUEVA FUNCIÓN: Verificar si el camino actual sigue siendo válido
bool isPathStillValid(const HexGrid& grid, const std::vector<std::pair<int, int>>& pathCells) {
    for (const auto& cell : pathCells) {
        if (grid.at(cell.first, cell.second).type == CellType::WALL) {
            return false; // Hay una pared en el camino
        }
    }
    return true;
}


// NUEVA FUNCIÓN: Recalcular camino automáticamente
bool recalculatePath(HexGrid& grid, Player& player, HexCell* goal,
                    std::vector<std::pair<int, int>>& pathCells,
                    bool& showPathVisualization, bool isExecuting) {
   
    std::cout << "\n=== RECALCULANDO CAMINO ===" << std::endl;
    std::cout << "Detectada obstrucción en el camino. Buscando ruta alternativa..." << std::endl;
   
    PathfindingResult newPath = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
   
    if (newPath.success) {
        std::cout << "¡Nueva ruta encontrada con " << newPath.path.size() << " pasos!" << std::endl;
       
        // Actualizar el camino
        pathCells.clear();
        for (auto *cell : newPath.path) {
            int r = cell->row, c = cell->col;
            // Ignoramos el primer paso si es la posición actual
            if (r == player.row && c == player.col)
                continue;
            pathCells.emplace_back(r, c);
        }
       
        // Si estaba ejecutando, mantener la visualización
        if (isExecuting) {
            showPathVisualization = true;
        }
       
        std::cout << "Continuando con la nueva ruta..." << std::endl;
        return true;
    } else {
        std::cout << "❌ NO SE ENCONTRÓ CAMINO ALTERNATIVO HACIA LA META ❌" << std::endl;
        std::cout << "El jugador está bloqueado. Intenta romper paredes o espera a que se generen nuevos caminos." << std::endl;
       
        // Limpiar todo
        pathCells.clear();
        showPathVisualization = false;
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
    std::vector<std::pair<int, int>> pathCells; // Vector para mostrar el camino


    // VARIABLES PARA LOS MODOS:
    bool showPathVisualization = false;  // Para mostrar el camino sin ejecutar
    bool autoSolveMode = false;          // Para activar resolución automática


    // NUEVA VARIABLE: Recordar la posición del jugador cuando se calculó el camino
    int lastPathPlayerRow = -1;
    int lastPathPlayerCol = -1;


    // NUEVA VARIABLE: Contador de turnos para detectar cuando aparecen nuevas paredes
    int lastTurnCount = 0;


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
                    else
                    {
                        // ESC cancela todo y vuelve al modo manual
                        showPathVisualization = false;
                        autoSolveMode = false;
                        player.isAutoMoving = false;
                        pathCells.clear(); // Limpiar camino mostrado
                        lastPathPlayerRow = -1;
                        lastPathPlayerCol = -1;
                        std::cout << "Modo cancelado. Volviendo al modo manual." << std::endl;
                    }
                }
                else if (event.key.code == Keyboard::P && !showVictoryScreen && !autoSolveMode)
                {
                    // MODO VISUALIZACIÓN: Solo mostrar el camino (SIN auto-movimiento)
                    std::cout << "\n=== MODO VISUALIZACIÓN DE CAMINO ===" << std::endl;
                    PathfindingResult path = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
                   
                    if (path.success) {
                        std::cout << "Camino encontrado con " << path.path.size() << " pasos:" << std::endl;
                        for (const auto &cell : path.path)
                        {
                            std::cout << "(" << cell->row << ", " << cell->col << ") ";
                        }
                        std::cout << std::endl;


                        // Limpiar y llenar el vector para visualización
                        pathCells.clear();
                        for (auto *cell : path.path)
                        {
                            int r = cell->row, c = cell->col;
                            // Ignoramos el primer paso si es la posición actual
                            if (r == player.row && c == player.col)
                                continue;
                            pathCells.emplace_back(r, c);
                        }
                       
                        showPathVisualization = true;   // Mostrar camino
                        autoSolveMode = false;          // NO ejecutar automáticamente
                        player.isAutoMoving = false;    // NO mover al jugador
                       
                        // GUARDAR posición del jugador cuando se calculó el camino
                        lastPathPlayerRow = player.row;
                        lastPathPlayerCol = player.col;
                       
                        std::cout << "Camino mostrado en ROJO con números de secuencia." << std::endl;
                        std::cout << "Presiona T para ejecutar este camino o R para auto-resolver directamente." << std::endl;
                        std::cout << "Presiona ESC para cancelar." << std::endl;
                    } else {
                        // NO HAY CAMINO DISPONIBLE
                        std::cout << "❌ NO SE ENCONTRÓ CAMINO HACIA LA META ❌" << std::endl;
                        std::cout << "Posibles soluciones:" << std::endl;
                        std::cout << "• Rompe paredes con SPACE (necesitas energía llena)" << std::endl;
                        std::cout << "• Espera a que aparezcan nuevos caminos" << std::endl;
                        std::cout << "• Muévete a una posición diferente" << std::endl;
                       
                        pathCells.clear();
                        showPathVisualization = false;
                        lastPathPlayerRow = -1;
                        lastPathPlayerCol = -1;
                    }
                }
                else if (event.key.code == Keyboard::R && !showVictoryScreen)
                {
                    // MODO AUTO-RESOLUCIÓN DIRECTA: Calcula y ejecuta inmediatamente
                    std::cout << "\n=== AUTO-RESOLUCIÓN DIRECTA ===" << std::endl;
                   
                    PathfindingResult path = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
                   
                    if (path.success) {
                        std::cout << "Camino calculado automáticamente con " << path.path.size() << " pasos." << std::endl;
                       
                        // Limpiar y llenar el vector para ejecución
                        pathCells.clear();
                        for (auto *cell : path.path)
                        {
                            int r = cell->row, c = cell->col;
                            // Ignoramos el primer paso si es la posición actual
                            if (r == player.row && c == player.col)
                                continue;
                            pathCells.emplace_back(r, c);
                        }
                       
                        if (!pathCells.empty()) {
                            autoSolveMode = true;           // Activar ejecución automática
                            player.isAutoMoving = true;     // Iniciar movimiento automático
                            showPathVisualization = false;  // NO mostrar visualización (modo directo)
                           
                            // NO guardar posición porque es ejecución directa
                            lastPathPlayerRow = -1;
                            lastPathPlayerCol = -1;
                           
                            std::cout << "Ejecutando auto-resolución con " << pathCells.size() << " pasos." << std::endl;
                            std::cout << "El camino se recalculará automáticamente si aparecen obstáculos." << std::endl;
                            std::cout << "Presiona ESC para cancelar." << std::endl;
                        }
                    } else {
                        // NO HAY CAMINO DISPONIBLE
                        std::cout << "❌ NO SE ENCONTRÓ CAMINO HACIA LA META ❌" << std::endl;
                        std::cout << "Auto-resolución no disponible. El jugador está bloqueado." << std::endl;
                        std::cout << "Intenta romper paredes manualmente o cambiar de posición." << std::endl;
                    }
                }
                else if (event.key.code == Keyboard::T && showPathVisualization && !autoSolveMode && !showVictoryScreen)
                {
                    // EJECUTAR EL CAMINO YA MOSTRADO: Mantener visualización + ejecutar
                    std::cout << "\n=== EJECUTANDO CAMINO MOSTRADO ===" << std::endl;
                   
                    if (!pathCells.empty()) {
                        autoSolveMode = true;               // Activar ejecución automática
                        player.isAutoMoving = true;         // Iniciar movimiento automático
                        // showPathVisualization = true;    // MANTENER VISUALIZACIÓN!!!
                       
                        std::cout << "Ejecutando camino mostrado con " << pathCells.size() << " pasos." << std::endl;
                        std::cout << "El camino permanece visible y se recalculará automáticamente si aparecen obstáculos." << std::endl;
                        std::cout << "Presiona ESC para cancelar." << std::endl;
                    }
                }
                else if (!showVictoryScreen && !autoSolveMode)
                {
                    // MODO MANUAL: Solo permitir movimiento manual si NO está en auto-resolución
                   
                    // ANTES de mover, recordar la posición actual
                    int oldRow = player.row;
                    int oldCol = player.col;
                   
                    handlePlayerMovement(event.key.code, player, grid);
                   
                    // DESPUÉS de mover, verificar si cambió de posición
                    if (player.row != oldRow || player.col != oldCol) {
                        // LIMPIAR el camino porque se movió manualmente
                        if (showPathVisualization || !pathCells.empty()) {
                            pathCells.clear();
                            showPathVisualization = false;
                            lastPathPlayerRow = -1;
                            lastPathPlayerCol = -1;
                            std::cout << "Camino invalidado: El jugador se movió manualmente." << std::endl;
                        }
                    }
                }
                else if (autoSolveMode && (event.key.code == Keyboard::W || event.key.code == Keyboard::E ||
                                          event.key.code == Keyboard::A || event.key.code == Keyboard::D ||
                                          event.key.code == Keyboard::Z || event.key.code == Keyboard::X ||
                                          event.key.code == Keyboard::Space))
                {
                    // Si está en modo automático e intenta moverse manualmente, mostrar mensaje
                    std::cout << "Modo automático activo. Presiona ESC para cancelar y volver al modo manual." << std::endl;
                }
            }
        }


        // VERIFICAR si el jugador se movió desde que se calculó el camino (por bandas transportadoras, etc.)
        if (!autoSolveMode && showPathVisualization) {
            if (player.row != lastPathPlayerRow || player.col != lastPathPlayerCol) {
                // El jugador se movió (posiblemente por bandas transportadoras)
                pathCells.clear();
                showPathVisualization = false;
                lastPathPlayerRow = -1;
                lastPathPlayerCol = -1;
                std::cout << "Camino invalidado: El jugador cambió de posición." << std::endl;
            }
        }


        // NUEVA VERIFICACIÓN: Detectar si aparecieron nuevas paredes que bloquean el camino
        int currentTurnCount = TurnSystem::getCurrentTurnCount();
        if (currentTurnCount != lastTurnCount) {
            lastTurnCount = currentTurnCount;
           
            // Si hay un camino activo, verificar si sigue siendo válido
            if (!pathCells.empty() && (showPathVisualization || autoSolveMode)) {
                if (!isPathStillValid(grid, pathCells)) {
                    // El camino está bloqueado, intentar recalcular
                    bool wasExecuting = autoSolveMode;
                    recalculatePath(grid, player, goal, pathCells, showPathVisualization, wasExecuting);
                   
                    // Si no se pudo recalcular y estaba en modo automático, detener
                    if (pathCells.empty() && autoSolveMode) {
                        autoSolveMode = false;
                        player.isAutoMoving = false;
                    }
                }
            }
        }


        // Solo actualizar auto-movimiento si está en modo automático
        if (autoSolveMode && player.isAutoMoving) {
            core::updateAutoMovement(grid, player, pathCells, goal->row, goal->col);
           
            // Si terminó el auto-movimiento, salir del modo automático
            if (!player.isAutoMoving) {
                autoSolveMode = false;
                // NO limpiar pathCells aquí si queremos que permanezca visible
                if (!showPathVisualization) {
                    pathCells.clear(); // Solo limpiar si no estaba en modo visualización
                }
                lastPathPlayerRow = -1;
                lastPathPlayerCol = -1;
                std::cout << "Auto-resolución completada." << std::endl;
            }
        }


        // Verificar condición de victoria
        if (!gameWon && player.hasWon)
        {
            gameWon = true;
            showVictoryScreen = true;
            autoSolveMode = false;           // Desactivar modo automático
            showPathVisualization = false;   // Desactivar visualización
            pathCells.clear();               // Limpiar camino
            lastPathPlayerRow = -1;
            lastPathPlayerCol = -1;
            victoryClock.restart();          // Iniciar efectos de victoria
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
            // SIEMPRE pasar pathCells para que se muestre cuando sea necesario
            drawGrid(window, grid, player, hexagon, texto, font, animationClock, backgroundClock, pathCells);


            // Dibujar UI ultra profesional
            drawModernEnergyBar(window, player, font, animationClock);
            drawGameInfo(window, font, TurnSystem::getCurrentTurnCount(), animationClock,
                        showPathVisualization, autoSolveMode);
            drawModernControls(window, font, animationClock);
        }


        window.display();
    }


    return 0;
}

