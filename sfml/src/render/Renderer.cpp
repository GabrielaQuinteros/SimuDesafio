#include "Renderer.hpp"
#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "model/Player.hpp"

using namespace model;
using namespace sf;

// Crea y devuelve un hexágono base configurado
CircleShape createHexagon()
{
    CircleShape hexagon(25, 6); // 6 lados para hexágono
    hexagon.setOutlineColor(Color::Black);
    hexagon.setFillColor(Color::White);
    hexagon.setOrigin(25, 25); // centro
    hexagon.setOutlineThickness(4);
    return hexagon;
}

// Dibuja el grid completo, incluyendo text y jugador con animación
void drawGrid(RenderWindow &window, const HexGrid &grid,
              Player &player, CircleShape &hexagon,
              Text &text)
{

    Color color[2] = {
        Color(220, 220, 220),
        Color(192, 192, 192)};

    // Colores para las bandas transportadoras
    Color conveyorColor = Color(200, 220, 255);

    // Actualizar la animación del jugador
    player.updateMovement();

    for (int y = 0; y < grid.rows(); ++y)
    {
        for (int x = 0; x < grid.cols(); ++x)
        {
            const auto &cell = grid.at(y, x);
            Vector2f pos = grid.toPixel(y, x);

            // Dibujar hexágono base
            hexagon.setPosition(pos);
            hexagon.setFillColor(color[y % 2]);

            // Resaltar bandas transportadoras con un color específico
            if (cell.type >= CellType::UP_RIGHT && cell.type <= CellType::DOWN_LEFT)
                hexagon.setFillColor(conveyorColor);
                window.draw(hexagon);

            // Dibujar text del tipo de celda
            text.setString(CellTypeToString(cell.type));
            text.setPosition(pos.x - 10.f, pos.y - 10.f);
            window.draw(text);
        }
    }

    // Dibujar el jugador en su posición visual
    CircleShape playerCircle(10);
    Vector2f playerPos = player.getVisualPosition(grid);

    // Cambiar color del jugador durante el movimiento
    if (player.isMoving)
    {
        playerCircle.setFillColor(Color(255, 100, 100)); // Rojo claro durante movimiento
    }
    else
    {
        playerCircle.setFillColor(Color::Blue); // Azul normal
    }

    playerCircle.setOrigin(10, 10);
    playerCircle.setPosition(playerPos);
    window.draw(playerCircle);
}