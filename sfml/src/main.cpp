#include <string>
#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "utils/Utils.hpp"
#include "utils/MapSelector.hpp"
#include "model/HexCell.hpp"
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include "core/GameLogic.hpp"
#include "core/TurnSystem.hpp"
#include "core/GameManager.hpp"
#include "render/Renderer.hpp"
#include "render/IntroScreen.hpp"
#include "core/PathFinding.hpp"
#include "core/AutoMovement.hpp"
#include <iostream>

#define WINDOW_WIDTH 1400  
#define WINDOW_HEIGHT 900  
#define MAP_PATH "resources/map.txt"
#define FONT_PATH "resources/arial.ttf"

using namespace model;
using namespace sf;

int main()
{
    std::cout << "=== INICIANDO HEXESCAPE ===" << std::endl;
    std::cout << "Escaneando mapas en carpeta resources..." << std::endl;
    scanMapFiles();
    
    std::cout << "\n=== MAPAS DISPONIBLES ===" << std::endl;
    for (size_t i = 0; i < availableMaps.size(); ++i) {
        std::cout << i << ". " << availableMaps[i].displayName 
                  << " (" << availableMaps[i].filename << ") - "
                  << (availableMaps[i].isValid ? "VÁLIDO" : "INVÁLIDO")
                  << (availableMaps[i].isDefault ? " [DEFECTO]" : "") << std::endl;
    }
    std::cout << "Total: " << availableMaps.size() << " mapas" << std::endl;
    std::cout << "========================\n" << std::endl;
    
    std::string selectedMapPath = MAP_PATH; // Por defecto

    bool mostrandoIntro = true;
    bool mostrandoSelector = false;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                        "HexEscape: Fabrica de Rompecabezas Elite",
                        Style::Titlebar | Style::Close | Style::Resize);
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile(FONT_PATH))
    {
        std::cout << "Error: No se pudo cargar la fuente " << FONT_PATH << std::endl;
        return 1;
    }

    HexGrid* grid = nullptr;
    HexCell* start = nullptr;
    HexCell* goal = nullptr;
    Player* player = nullptr;

    Text texto = createText(font, 16, Color::White);
    CircleShape hexagon = createHexagon();

    Clock animationClock;
    Clock backgroundClock;
    Clock victoryClock;

    bool gameWon = false;
    bool showVictoryScreen = false;
    std::vector<std::pair<int, int>> pathCells; 

    bool showPathVisualization = false; 
    bool autoSolveMode = false;         

    int lastPlayerRow = -1;
    int lastPlayerCol = -1;
    int lastTurnCount = 0;
    bool pathNeedsUpdate = false;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::Resized)
            {
                FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                window.setView(View(visibleArea));
            }

            if (event.type == Event::KeyPressed)
            {
                if (mostrandoIntro)
                {
                    if (event.key.code == Keyboard::Space)
                    {
                        mostrandoIntro = false;
                        mostrandoSelector = true;
                        std::cout << "=== MOSTRANDO SELECTOR DE MAPAS ===" << std::endl;
                    }
                    else if (event.key.code == Keyboard::Escape)
                    {
                        window.close();
                    }
                    continue;
                }
                
                else if (mostrandoSelector)
                {
                    std::cout << "TECLA PRESIONADA: " << event.key.code << std::endl;
                    std::cout << "ENTER = " << Keyboard::Enter << ", ESC = " << Keyboard::Escape << std::endl;
                    std::cout << "W = " << Keyboard::W << ", S = " << Keyboard::S << std::endl;
                    std::cout << "- Número de mapas: " << availableMaps.size() << std::endl;
                    std::cout << "- Índice seleccionado: " << selectedMapIndex << std::endl;
                    
                    if (event.key.code == Keyboard::Escape)
                    {
                        std::cout << "=== ESC DETECTADO ===" << std::endl;
                        selectedMapPath = MAP_PATH;
                        mostrandoSelector = false;
                        
                        if (loadSelectedMap(selectedMapPath, grid, start, goal, player)) {
                            std::cout << "=== JUEGO INICIADO CON MAPA POR DEFECTO ===" << std::endl;
                            animationClock.restart();
                            backgroundClock.restart();
                        } else {
                            std::cout << "Error crítico: No se puede cargar el mapa por defecto" << std::endl;
                            window.close();
                        }
                    }
                    else if (event.key.code == Keyboard::Return || event.key.code == Keyboard::Enter || event.key.code == 57)
                    {
                        std::cout << "=== ENTER/RETURN/SPACE DETECTADO (código: " << event.key.code << ") ===" << std::endl;
                        
                        if (selectedMapIndex >= 0 && selectedMapIndex < static_cast<int>(availableMaps.size())) {
                            selectedMapPath = availableMaps[selectedMapIndex].filename;
                            std::cout << "Seleccionando: " << selectedMapPath << std::endl;
                        } else {
                            selectedMapPath = MAP_PATH;
                            std::cout << "Usando mapa por defecto: " << selectedMapPath << std::endl;
                        }
                        
                        mostrandoSelector = false;
                        
                        if (loadSelectedMap(selectedMapPath, grid, start, goal, player)) {
                            std::cout << "=== ÉXITO: JUEGO INICIADO ===" << std::endl;
                            animationClock.restart();
                            backgroundClock.restart();
                        } else {
                            std::cout << "=== ERROR: No se pudo cargar ===" << std::endl;
                            if (selectedMapPath != MAP_PATH) {
                                if (loadSelectedMap(MAP_PATH, grid, start, goal, player)) {
                                    std::cout << "=== ÉXITO CON MAPA POR DEFECTO ===" << std::endl;
                                    animationClock.restart();
                                    backgroundClock.restart();
                                } else {
                                    window.close();
                                }
                            } else {
                                window.close();
                            }
                        }
                    }
                    else if (event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
                    {
                        std::cout << "=== W/UP DETECTADO ===" << std::endl;
                        if (selectedMapIndex > 0) {
                            selectedMapIndex--;
                            std::cout << "Nuevo índice: " << selectedMapIndex << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
                    {
                        std::cout << "=== S/DOWN DETECTADO ===" << std::endl;
                        if (selectedMapIndex < static_cast<int>(availableMaps.size()) - 1) {
                            selectedMapIndex++;
                            std::cout << "Nuevo índice: " << selectedMapIndex << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::F5)
                    {
                        std::cout << "=== F5 DETECTADO - REESCANEANDO ===" << std::endl;
                        scanMapFiles();
                        if (selectedMapIndex >= static_cast<int>(availableMaps.size())) {
                            selectedMapIndex = std::max(0, static_cast<int>(availableMaps.size()) - 1);
                        }
                    }
                    else if (event.key.code == Keyboard::P)
                    {
                        std::cout << "=== P DETECTADO - USANDO PORTAPAPELES ===" << std::endl;
                        selectedMapPath = Clipboard::getString();
                        std::cout << "Ruta del portapapeles: " << selectedMapPath << std::endl;
                        mostrandoSelector = false;
                        
                        if (loadSelectedMap(selectedMapPath, grid, start, goal, player)) {
                            std::cout << "=== ÉXITO: JUEGO INICIADO CON MAPA DEL PORTAPAPELES ===" << std::endl;
                            animationClock.restart();
                            backgroundClock.restart();
                        } else {
                            std::cout << "=== ERROR: No se pudo cargar mapa del portapapeles, usando defecto ===" << std::endl;
                            if (loadSelectedMap(MAP_PATH, grid, start, goal, player)) {
                                std::cout << "=== ÉXITO CON MAPA POR DEFECTO ===" << std::endl;
                                animationClock.restart();
                                backgroundClock.restart();
                            } else {
                                window.close();
                            }
                        }
                    }
                    else 
                    {
                        std::cout << "=== TECLA NO RECONOCIDA: " << event.key.code << " ===" << std::endl;
                    }
                    continue;
                }

                else if (grid && player && start && goal)
                {
                    if (event.key.code == Keyboard::Escape)
                    {
                        if (showVictoryScreen)
                        {
                            window.close();
                        }
                        else
                        {
                            std::cout << "=== CANCELANDO TODOS LOS MODOS - VOLVIENDO A MANUAL ===" << std::endl;
                            showPathVisualization = false;
                            autoSolveMode = false;
                            player->isAutoMoving = false;
                            pathCells.clear();
                            lastPlayerRow = -1;
                            lastPlayerCol = -1;
                            std::cout << "Modo manual activado. Usa P para mostrar camino, R para auto-resolución." << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::P && !showVictoryScreen && !autoSolveMode)
                    {
                        std::cout << "=== MODO VISUALIZACIÓN ACTIVADO ===" << std::endl;
                        PathfindingResult path = findPath(*grid, player->row, player->col, goal->row, goal->col, player->energy);
                       
                        if (path.success && !path.path.empty()) {
                            pathCells.clear();
                            for (auto *cell : path.path)
                            {
                                pathCells.emplace_back(cell->row, cell->col);
                            }
                           
                            showPathVisualization = true;
                            autoSolveMode = false;
                            player->isAutoMoving = false;
                           
                            lastPlayerRow = player->row;
                            lastPlayerCol = player->col;
                           
                            std::cout << "Camino mostrado con " << pathCells.size() << " pasos. Presiona T para ejecutar." << std::endl;
                        } else {
                            pathCells.clear();
                            showPathVisualization = false;
                            lastPlayerRow = -1;
                            lastPlayerCol = -1;
                            std::cout << "No se encontró camino hacia la meta." << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::R && !showVictoryScreen)
                    {
                        std::cout << "=== MODO AUTO-RESOLUCIÓN ACTIVADO ===" << std::endl;
                        PathfindingResult path = findPath(*grid, player->row, player->col, goal->row, goal->col, player->energy);
                       
                        if (path.success && !path.path.empty()) {
                            pathCells.clear();
                            for (auto *cell : path.path)
                            {
                                pathCells.emplace_back(cell->row, cell->col);
                            }
                           
                            autoSolveMode = true;
                            player->isAutoMoving = true;
                            showPathVisualization = false; 
                           
                            lastPlayerRow = player->row;
                            lastPlayerCol = player->col;
                           
                            std::cout << "Auto-resolución iniciada con " << pathCells.size() << " pasos. El jugador se moverá automáticamente." << std::endl;
                        } else {
                            std::cout << "No se encontró camino para auto-resolución." << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::T && showPathVisualization && !autoSolveMode && !showVictoryScreen)
                    {
                        if (!pathCells.empty()) {
                            std::cout << "=== EJECUTANDO CAMINO MOSTRADO ===" << std::endl;
                            autoSolveMode = true;
                            player->isAutoMoving = true;
                           
                            std::cout << "Ejecutando camino con " << pathCells.size() << " pasos. El camino permanecerá visible." << std::endl;
                        }
                    }
                    else if (!showVictoryScreen && !autoSolveMode && !player->isAutoMoving)
                    {
                       
                        int oldRow = player->row;
                        int oldCol = player->col;
                       
                        handlePlayerMovement(event.key.code, *player, *grid);
                       
                        if (player->row != oldRow || player->col != oldCol) {
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
        }

        if (!mostrandoIntro && !mostrandoSelector && grid && player && goal)
        {
            if (!autoSolveMode && showPathVisualization) {
                if (player->row != lastPlayerRow || player->col != lastPlayerCol) {
                    if (!isPathStillValid(*grid, pathCells, player->row, player->col)) {
                        pathCells.clear();
                        showPathVisualization = false;
                        lastPlayerRow = -1;
                        lastPlayerCol = -1;
                    }
                }
            }

            int currentTurnCount = TurnSystem::getCurrentTurnCount();
            if (currentTurnCount != lastTurnCount) {
                lastTurnCount = currentTurnCount;
                pathNeedsUpdate = true;
            }

            if (pathNeedsUpdate && !pathCells.empty() && (showPathVisualization || autoSolveMode)) {
                pathNeedsUpdate = false;
               
                if (!isPathStillValid(*grid, pathCells, player->row, player->col)) {
                    bool wasExecuting = autoSolveMode;
                   
                    if (!recalculatePath(*grid, *player, goal, pathCells, showPathVisualization, autoSolveMode)) {
                        if (wasExecuting) {
                            autoSolveMode = false;
                            player->isAutoMoving = false;
                        }
                    }
                }
            }

            if (autoSolveMode && player->isAutoMoving) {
                core::updateAutoMovement(*grid, *player, pathCells, goal->row, goal->col);
               
                if (!player->isAutoMoving) {
                    std::cout << "=== AUTO-MOVIMIENTO COMPLETADO ===" << std::endl;
                    autoSolveMode = false;
                   
                    if (!showPathVisualization) {
                        pathCells.clear();
                        std::cout << "Camino limpiado. Volviendo a modo manual." << std::endl;
                    } else {
                        std::cout << "Camino mantenido visible. Presiona ESC para limpiar." << std::endl;
                    }
                   
                    lastPlayerRow = player->row;
                    lastPlayerCol = player->col;
                }
            }

            if (!gameWon && player->hasWon)
            {
                gameWon = true;
                showVictoryScreen = true;
                autoSolveMode = false;
                showPathVisualization = false;
                player->isAutoMoving = false;
                pathCells.clear();
                lastPlayerRow = -1;
                lastPlayerCol = -1;
                victoryClock.restart();
            }

            if (!showVictoryScreen)
            {
                handleConveyorMovement(*player, *grid);
            }
        }

        window.clear(Color(5, 10, 20));

        if (mostrandoIntro)
        {
            mostrarIntro(window, font);
        }
        else if (mostrandoSelector)
        {
            mostrarSelectorMapas(window, font);
        }
        else if (showVictoryScreen && grid && player)
        {
            drawVictoryScreen(window, font, player->winTime,
                              TurnSystem::getCurrentTurnCount(), victoryClock);
        }
        else if (grid && player)
        {
            drawGrid(window, *grid, *player, hexagon, texto, font, animationClock, backgroundClock, pathCells);

            drawModernEnergyBar(window, *player, font, animationClock);
            drawGameInfo(window, font, TurnSystem::getCurrentTurnCount(), animationClock,
                        showPathVisualization, autoSolveMode);
            drawModernControls(window, font, animationClock);
        }

        window.display();
    }

    if (grid) {
        delete grid;
        grid = nullptr;
    }
    if (player) {
        delete player;
        player = nullptr;
    }

    std::cout << "=== FINALIZANDO HEXESCAPE ===" << std::endl;
    return 0;
}