#include "Player.hpp" 
#include "HexGrid.hpp" 
#include <cmath>       

namespace model // Define el espacio de nombres model para organizar las clases relacionadas con el modelo del juego
{

    // Constructor de la clase Player
    Player::Player(int r, int c) : row(r), col(c) 
    {
        // Inicializa las coordenadas del jugador en el grid (fila y columna)
    }

    // Método para iniciar una animación de movimiento
    void Player::startMovement(sf::Vector2f start, sf::Vector2f target)
    {
        isMoving = true;          // Marca al jugador como "en movimiento"
        startPosition = start;    
        targetPosition = target; 
        currentPosition = start;  
        movementClock.restart();  // Reinicia el reloj para medir el tiempo transcurrido en la animación
    }

    // Método para actualizar la posición del jugador durante la animación
    void Player::updateMovement()
    {
        if (!isMoving) // Si el jugador no está en movimiento, no hace nada y retorna
            return;

        // Calcula el tiempo transcurrido desde que comenzó la animación
        float elapsed = movementClock.getElapsedTime().asSeconds();
        // Calcula el progreso de la animación como una fracción del tiempo total de la animación
        float progress = elapsed / movementDuration;

        if (progress >= 1.0f) // Si el progreso es igual o mayor a 1.0, la animación ha terminado
        {
            isMoving = false; //Desactiva la flag de movimiento             
            currentPosition = targetPosition; 
        }
        else // Si la animación no ha terminado, calcula la posición intermedia
        {
            // Aplica una interpolación suave (easing) para hacer que el movimiento sea más natural
            float easedProgress = 0.5f * (1.0f - cos(progress * 3.14159f)); // Easing basado en una función coseno
            // Calcula la posición actual interpolando entre la posición inicial y la posición objetivo
            currentPosition.x = startPosition.x + (targetPosition.x - startPosition.x) * easedProgress;
            currentPosition.y = startPosition.y + (targetPosition.y - startPosition.y) * easedProgress;
        }
    }

    // Método para obtener la posición visual actual del jugador
    sf::Vector2f Player::getVisualPosition(const HexGrid &grid) const
    {
        if (isMoving) // Si el jugador está en movimiento, devuelve la posición interpolada actual
        {
            return currentPosition;
        }
        else // Si el jugador no está en movimiento, calcula la posición en píxeles basada en su posición lógica en el grid
        {
            return grid.toPixel(row, col); // Convierte las coordenadas del grid a coordenadas de píxeles
        }
    }
}