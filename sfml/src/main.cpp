#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "utils/Utils.hpp"
#include "model/HexCell.hpp"
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include "core/GameLogic.hpp"
#include "render/Renderer.hpp"

// Definición de constantes para la ventana y recursos
#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 500
#define MAP_PATH "resources/map.txt"    // Ruta al archivo del mapa
#define FONT_PATH "resources/arial.ttf" // Ruta al archivo de la fuente

using namespace model;
using namespace sf;

int main()
{
    // Cargar mapa desde archivo
    HexGrid grid = loadHexGridFromFile(MAP_PATH);
    // Se carga el grid hexagonal desde el archivo especificado en MAP_PATH.
    // La función `loadHexGridFromFile` devuelve un objeto `HexGrid` inicializado.

    // Obtener celda de inicio
    HexCell *start = findStartCell(grid);
    // Busca la celda de inicio (de tipo START) en el grid.
    // Si no se encuentra, devuelve nullptr.

    if (!start) // Si no se encuentra la celda de inicio, termina el programa con un código de error.
        return 1;

    // Crear el jugador en la posición inicial
    Player player(start->row, start->col);
    // Inicializa al jugador en la fila y columna de la celda de inicio.

    // Crear la ventana de renderizado
    RenderWindow window({WINDOW_WIDTH, WINDOW_HEIGHT}, "Game", Style::Titlebar | Style::Close);
    // Crea una ventana de SFML con las dimensiones especificadas y el título "Hexagons".

    // Cargar la fuente para el texto
    Font font;
    if (!font.loadFromFile(FONT_PATH)) // Intenta cargar la fuente desde el archivo especificado en FONT_PATH.
    {
        return 1; // Si no se puede cargar la fuente, termina el programa con un código de error.
    }

    // Crear elementos gráficos
    Text texto = createText(font, 16, Color::Black);
    // Crea un objeto de texto con la fuente cargada, tamaño 16 y color negro.
    CircleShape hexagon = createHexagon();
    // Crea un hexágono que se usará para renderizar las celdas del grid.

    // Bucle principal del juego
    while (window.isOpen()) // Mientras la ventana esté abierta
    {
        Event event;                    // Objeto para manejar eventos de la ventana
        while (window.pollEvent(event)) // Procesa todos los eventos en la cola
        {
            if (event.type == Event::Closed)     // Si se cierra la ventana
                window.close();                  // Cierra la ventana
            if (event.type == Event::KeyPressed) // Si se presiona una tecla
                handlePlayerMovement(event.key.code, player, grid);
            // Llama a la función `handlePlayerMovement` para mover al jugador
            // según la tecla presionada.
        }

        // Aplicar efecto de bandas transportadoras
        handleConveyorMovement(player, grid);
        // Si el jugador está sobre una celda de tipo banda transportadora,
        // mueve automáticamente al jugador a la celda correspondiente.

        // Renderizar el contenido de la ventana
        window.clear(Color::Black);
        // Limpia la ventana con un color blanco de fondo.

        // Mostrar información de turnos (opcional)
        Text turnInfo = createText(font, 14, Color::White);
        turnInfo.setString("Turnos: " + std::to_string(getCurrentTurnCount()));
        turnInfo.setPosition(650, 100);
        window.draw(turnInfo);

        drawGrid(window, grid, player, hexagon, texto);
        // Dibuja el grid hexagonal, el jugador y otros elementos en la ventana.
        window.display();
        // Muestra el contenido renderizado en la ventana.
    }

    return 0;
}