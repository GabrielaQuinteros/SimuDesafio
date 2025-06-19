#include <string>  // Agregar esta línea al inicio
#include <SFML/Graphics.hpp> 
#include "utils/MapLoader.hpp" 
#include "utils/Utils.hpp" 
#include "model/HexCell.hpp" 
#include "model/HexGrid.hpp" 
#include "model/Player.hpp" 
#include "core/GameLogic.hpp" 
#include "core/TurnSystem.hpp" 
#include "render/Renderer.hpp" 

// Definición de constantes para la ventana y recursos
#define WINDOW_WIDTH 750 
#define WINDOW_HEIGHT 500 
#define MAP_PATH "resources/map.txt" 
#define FONT_PATH "resources/arial.ttf" 

using namespace model;
using namespace sf;

int main()
{
    // Cargar mapa desde archivo
    HexGrid grid = loadHexGridFromFile(MAP_PATH);

    // Obtener celda de inicio
    HexCell* start = findStartCell(grid);

    if (!start)
        return 1;

    // Crear el jugador en la posición inicial
    Player player(start->row, start->col);

    // Inicializar el sistema de turnos
    TurnSystem::resetTurnCounter();

    // Crear la ventana de renderizado con mayor resolución
    RenderWindow window({ WINDOW_WIDTH, WINDOW_HEIGHT }, "🎮 Escape the Grid - Fabrica de Rompecabezas 🏭", Style::Titlebar | Style::Close);
    window.setFramerateLimit(60); // Limitar FPS para animaciones suaves

    // Cargar la fuente para el texto
    Font font;
    if (!font.loadFromFile(FONT_PATH))
    {
        return 1;
    }

    // Crear elementos gráficos
    Text texto = createText(font, 16, Color::White);
    CircleShape hexagon = createHexagon();

    // Relojes para animaciones
    Clock animationClock;
    Clock backgroundClock;

    // Bucle principal del juego
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed)
                handlePlayerMovement(event.key.code, player, grid);
        }

        // Aplicar efecto de bandas transportadoras
        handleConveyorMovement(player, grid);

        // Limpiar ventana con color de fondo
        window.clear(Color(10, 15, 25));

        // Dibujar el grid con la nueva interfaz moderna
        drawGrid(window, grid, player, hexagon, texto, font, animationClock, backgroundClock);

        // Dibujar UI moderna
        drawModernEnergyBar(window, player, font, animationClock);
        drawGameInfo(window, font, TurnSystem::getCurrentTurnCount());
        drawModernControls(window, font);

        window.display();
    }

    return 0;
}