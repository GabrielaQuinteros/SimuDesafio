#include "Renderer.hpp"
#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "model/Player.hpp"
#include <cmath>

using namespace model;
using namespace sf;

// Crea y devuelve un hexágono base configurado
CircleShape createHexagon()
{
    CircleShape hexagon(25, 6); // 6 lados para hexágono
    hexagon.setOutlineColor(Color(80, 80, 80));
    hexagon.setFillColor(Color::White);
    hexagon.setOrigin(25, 25); // centro
    hexagon.setOutlineThickness(2);
    return hexagon;
}

// Función para crear gradientes de color
Color lerpColor(const Color& a, const Color& b, float t) {
    return Color(
        static_cast<Uint8>(a.r + (b.r - a.r) * t),
        static_cast<Uint8>(a.g + (b.g - a.g) * t),
        static_cast<Uint8>(a.b + (b.b - a.b) * t),
        static_cast<Uint8>(a.a + (b.a - a.a) * t)
    );
}

// Dibuja el título del juego con efectos especiales
void drawGameTitle(RenderWindow& window, Font& font, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();

    // Título principal con efectos
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(32);
    titleText.setStyle(Text::Bold);
    titleText.setString("HexEscape");

    // Calcular posición centrada en la parte superior
    FloatRect titleBounds = titleText.getLocalBounds();
    float titleX = (window.getSize().x - titleBounds.width) / 2.0f;
    float titleY = 10.0f; // Reducido de 15 a 10

    // Efecto de brillo pulsante
    float glow = sin(time * 3.0f) * 0.3f + 0.7f;

    // Sombra múltiple para efecto de profundidad
    for (int i = 5; i > 0; --i) {
        Text shadow = titleText;
        shadow.setPosition(titleX + i, titleY + i);
        shadow.setFillColor(Color(0, 0, 0, 100 - i * 15));
        window.draw(shadow);
    }

    // Título con gradiente simulado
    titleText.setPosition(titleX, titleY);
    titleText.setFillColor(Color(150 + 105 * glow, 200 + 55 * glow, 255));
    window.draw(titleText);

    // Subtítulo elegante
    Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setCharacterSize(12);
    subtitleText.setString("- Fabrica de Rompecabezas -");
    subtitleText.setStyle(Text::Italic);

    FloatRect subtitleBounds = subtitleText.getLocalBounds();
    float subtitleX = (window.getSize().x - subtitleBounds.width) / 2.0f;
    float subtitleY = titleY + 35.0f; // Reducido de 40 a 35

    subtitleText.setPosition(subtitleX, subtitleY);
    subtitleText.setFillColor(Color(180, 180, 220, 200));
    window.draw(subtitleText);

    // Líneas decorativas
    RectangleShape leftLine(Vector2f(80, 2));
    leftLine.setPosition(subtitleX - 90, subtitleY + 8);
    leftLine.setFillColor(Color(100, 150, 255, 150));
    window.draw(leftLine);

    RectangleShape rightLine(Vector2f(80, 2));
    rightLine.setPosition(subtitleX + subtitleBounds.width + 10, subtitleY + 8);
    rightLine.setFillColor(Color(100, 150, 255, 150));
    window.draw(rightLine);
}

// Dibuja el fondo animado con efectos avanzados
void drawAnimatedBackground(RenderWindow& window, Clock& bgClock) {
    float time = bgClock.getElapsedTime().asSeconds();

    // Fondo base con gradiente complejo
    for (int i = 0; i < window.getSize().y; i += 3) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        float wave1 = sin(time * 1.5f + gradient * 8.0f) * 0.2f + 0.8f;
        float wave2 = cos(time * 2.0f + gradient * 5.0f) * 0.15f + 0.85f;

        Color topColor(15, 20, 45);     // Azul marino profundo
        Color midColor(25, 35, 65);     // Azul medio
        Color bottomColor(35, 45, 85);  // Azul más claro

        Color currentColor;
        if (gradient < 0.5f) {
            currentColor = lerpColor(topColor, midColor, gradient * 2.0f * wave1);
        }
        else {
            currentColor = lerpColor(midColor, bottomColor, (gradient - 0.5f) * 2.0f * wave2);
        }

        RectangleShape line(Vector2f(window.getSize().x, 3));
        line.setPosition(0, i);
        line.setFillColor(currentColor);
        window.draw(line);
    }

    // Efectos de partículas flotantes
    for (int i = 0; i < 20; ++i) {
        float x = fmod(time * 30.0f + i * 50.0f, window.getSize().x + 100.0f) - 50.0f;
        float y = 100.0f + sin(time * 2.0f + i * 0.5f) * 50.0f + i * 20.0f;

        if (y < window.getSize().y) {
            CircleShape particle(1 + i % 3);
            particle.setPosition(x, y);
            particle.setFillColor(Color(100, 150, 255, 80 + i % 50));
            window.draw(particle);
        }
    }
}

// Dibuja una barra de energía ultra moderna con efectos avanzados
void drawModernEnergyBar(RenderWindow& window, const Player& player, Font& font, Clock& animClock) {
    const float barWidth = 200.0f;
    const float barHeight = 25.0f;
    const float barX = 15.0f;
    const float barY = window.getSize().y - 60.0f; // CAMBIADO: Movido más cerca del borde inferior

    float time = animClock.getElapsedTime().asSeconds();

    // Marco exterior con brillo
    RectangleShape outerGlow(Vector2f(barWidth + 12, barHeight + 12));
    outerGlow.setPosition(barX - 6, barY - 6);
    outerGlow.setFillColor(Color(50, 100, 200, 60));
    window.draw(outerGlow);

    // Marco metálico con gradiente
    RectangleShape metalFrame(Vector2f(barWidth + 6, barHeight + 6));
    metalFrame.setPosition(barX - 3, barY - 3);
    metalFrame.setFillColor(Color(80, 90, 120));
    window.draw(metalFrame);

    // Fondo interno oscuro
    RectangleShape energyFrame(Vector2f(barWidth, barHeight));
    energyFrame.setPosition(barX, barY);
    energyFrame.setFillColor(Color(15, 20, 35));
    energyFrame.setOutlineColor(Color(120, 140, 180));
    energyFrame.setOutlineThickness(1);
    window.draw(energyFrame);

    // Relleno de energía con efectos avanzados
    float fillWidth = barWidth * player.getEnergyPercentage();
    if (fillWidth > 0) {
        RectangleShape energyFill(Vector2f(fillWidth, barHeight));
        energyFill.setPosition(barX, barY);

        // Colores dinámicos más sofisticados
        Color energyColor;
        if (player.isEnergyFull()) {
            // Efecto arcoíris pulsante cuando está llena
            float pulse = sin(time * 10.0f) * 0.4f + 0.6f;
            energyColor = Color(100 + 155 * pulse, 255 * pulse, 150 + 105 * pulse);
        }
        else if (player.getEnergyPercentage() > 0.7f) {
            energyColor = Color(255, 220, 0); // Dorado
        }
        else if (player.getEnergyPercentage() > 0.4f) {
            energyColor = Color(255, 160, 0); // Naranja vibrante
        }
        else {
            energyColor = Color(255, 60, 60); // Rojo intenso
        }

        energyFill.setFillColor(energyColor);
        window.draw(energyFill);

        // Efecto de cristal/brillo superior
        RectangleShape glassEffect(Vector2f(fillWidth, 8));
        glassEffect.setPosition(barX, barY + 2);
        Color glassColor = energyColor;
        glassColor.a = 80;
        glassEffect.setFillColor(glassColor);
        window.draw(glassEffect);

        // Líneas de energía dinámicas
        if (player.getEnergyPercentage() > 0.1f) {
            for (int i = 0; i < 3; ++i) {
                float lineX = barX + (fillWidth * 0.3f) + i * (fillWidth * 0.2f);
                if (lineX < barX + fillWidth - 5) {
                    RectangleShape energyLine(Vector2f(2, barHeight - 6));
                    energyLine.setPosition(lineX, barY + 3);
                    energyLine.setFillColor(Color(255, 255, 255, 120 + 60 * sin(time * 8.0f + i)));
                    window.draw(energyLine);
                }
            }
        }
    }

    // Texto de energía con estilo futurista - MOVIDO ARRIBA de la barra
    Text energyText;
    energyText.setFont(font);
    energyText.setCharacterSize(14);
    energyText.setFillColor(Color(200, 220, 255));
    energyText.setStyle(Text::Bold);
    energyText.setString("ENERGIA: " + std::to_string(player.energy) + "/" + std::to_string(Player::MAX_ENERGY));
    energyText.setPosition(barX, barY - 22); // Arriba de la barra

    // Sombra del texto con brillo
    Text energyShadow = energyText;
    energyShadow.setFillColor(Color(0, 50, 100, 150));
    energyShadow.setPosition(barX + 1, barY - 21);
    window.draw(energyShadow);
    window.draw(energyText);

    // Indicador de habilidad con efectos espectaculares - MOVIDO ARRIBA
    if (player.canUseWallBreak()) {
        float glow = sin(time * 8.0f) * 0.5f + 0.5f;
        float pulse = sin(time * 12.0f) * 0.3f + 0.7f;

        Text abilityText;
        abilityText.setFont(font);
        abilityText.setCharacterSize(12);
        abilityText.setFillColor(Color(100 + 155 * glow, 255 * pulse, 100 + 155 * glow));
        abilityText.setStyle(Text::Bold);
        abilityText.setString("SPACE: ROMPER PARED");
        abilityText.setPosition(barX, barY - 40); // Arriba del texto de energía

        // Múltiples sombras para efecto de resplandor
        for (int i = 3; i > 0; --i) {
            Text glowShadow = abilityText;
            glowShadow.setFillColor(Color(0, 255, 0, 50 * i));
            glowShadow.setPosition(barX + i, barY - 40 + i);
            window.draw(glowShadow);
        }
        window.draw(abilityText);
    }
    else {
        Text waitText;
        waitText.setFont(font);
        waitText.setCharacterSize(11);
        waitText.setFillColor(Color(120, 140, 160));
        waitText.setString("Acumula energia para romper paredes");
        waitText.setPosition(barX, barY - 40); // Arriba del texto de energía
        window.draw(waitText);
    }
}

// Dibuja información del juego con estilo premium
void drawGameInfo(RenderWindow& window, Font& font, int turnCount) {
    // Panel principal con efectos de cristal (mismo tamaño que controles)
    RectangleShape glassPanel(Vector2f(140, 100)); // Mismo tamaño que controles
    glassPanel.setPosition(window.getSize().x - 150, 85);
    glassPanel.setFillColor(Color(20, 30, 60, 180));
    glassPanel.setOutlineColor(Color(100, 150, 255, 200));
    glassPanel.setOutlineThickness(2);
    window.draw(glassPanel);

    // Borde interior brillante
    RectangleShape innerBorder(Vector2f(136, 96));
    innerBorder.setPosition(window.getSize().x - 148, 87);
    innerBorder.setFillColor(Color::Transparent);
    innerBorder.setOutlineColor(Color(150, 200, 255, 100));
    innerBorder.setOutlineThickness(1);
    window.draw(innerBorder);

    // Título del panel con efecto holográfico
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(12);
    titleText.setFillColor(Color(150, 200, 255));
    titleText.setStyle(Text::Bold);
    titleText.setString("ESTADO");
    titleText.setPosition(window.getSize().x - 135, 95);
    window.draw(titleText);

    // Información de turnos sin iconos
    Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(11);
    turnText.setFillColor(Color(200, 220, 255));
    turnText.setString("Turnos: " + std::to_string(turnCount));
    turnText.setPosition(window.getSize().x - 140, 115);
    window.draw(turnText);

    // Próximo evento con advertencia visual
    int turnsUntilWall = 5 - (turnCount % 5);
    Text eventText;
    eventText.setFont(font);
    eventText.setCharacterSize(11);

    // Color de advertencia basado en proximidad del evento
    if (turnsUntilWall <= 1) {
        eventText.setFillColor(Color(255, 100, 100)); // Rojo urgente
    }
    else if (turnsUntilWall <= 2) {
        eventText.setFillColor(Color(255, 200, 100)); // Amarillo advertencia
    }
    else {
        eventText.setFillColor(Color(100, 255, 150)); // Verde normal
    }

    eventText.setString("Muro en: " + std::to_string(turnsUntilWall == 5 ? 0 : turnsUntilWall));
    eventText.setPosition(window.getSize().x - 140, 135);
    window.draw(eventText);

    // Información adicional para llenar el espacio
    Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(10);
    infoText.setFillColor(Color(150, 170, 200));
    infoText.setString("Estado: Activo");
    infoText.setPosition(window.getSize().x - 140, 155);
    window.draw(infoText);
}

// Dibuja controles con estilo futurista y descripciones mejoradas
void drawModernControls(RenderWindow& window, Font& font) {
    // Panel de controles con efecto holográfico
    RectangleShape controlPanel(Vector2f(140, 120)); // Aumentado de 100 a 120 para más espacio
    controlPanel.setPosition(window.getSize().x - 150, 195);
    controlPanel.setFillColor(Color(15, 25, 50, 200));
    controlPanel.setOutlineColor(Color(80, 120, 200, 220));
    controlPanel.setOutlineThickness(2);
    window.draw(controlPanel);

    // Efecto de brillo interior
    RectangleShape innerGlow(Vector2f(136, 116)); // Ajustado al nuevo tamaño
    innerGlow.setPosition(window.getSize().x - 148, 197);
    innerGlow.setFillColor(Color::Transparent);
    innerGlow.setOutlineColor(Color(120, 160, 255, 80));
    innerGlow.setOutlineThickness(1);
    window.draw(innerGlow);

    // Título con decoración
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(12);
    titleText.setFillColor(Color(120, 180, 255));
    titleText.setStyle(Text::Bold);
    titleText.setString("CONTROLES");
    titleText.setPosition(window.getSize().x - 140, 205);
    window.draw(titleText);

    // Controles con descripciones más claras
    std::vector<std::pair<std::string, Color>> controls = {
        {"W/E: Arriba", Color(100, 255, 150)},
        {"A/D: Lados", Color(150, 200, 255)},
        {"Z/X: Abajo", Color(255, 150, 100)},
        {"SPC: Habilidad", Color(255, 255, 100)}
    };

    for (size_t i = 0; i < controls.size(); ++i) {
        Text controlText;
        controlText.setFont(font);
        controlText.setCharacterSize(10);
        controlText.setFillColor(controls[i].second);
        controlText.setStyle(Text::Bold);
        controlText.setString(controls[i].first);
        controlText.setPosition(window.getSize().x - 140, 225 + i * 16); // Más espacio entre líneas
        window.draw(controlText);
    }
}

// Obtiene el color de una celda según su tipo
Color getCellColor(CellType type, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();

    switch (type) {
    case CellType::EMPTY:
        return Color(240, 248, 255, 200); // Blanco azulado
    case CellType::WALL:
        return Color(80, 80, 100); // Gris oscuro
    case CellType::START: {
        float pulse = sin(time * 3.0f) * 0.3f + 0.7f;
        return Color(100 * pulse, 255 * pulse, 100 * pulse); // Verde pulsante
    }
    case CellType::GOAL: {
        float pulse = sin(time * 4.0f) * 0.4f + 0.6f;
        return Color(255 * pulse, 215 * pulse, 0); // Dorado pulsante
    }
    case CellType::ITEM: {
        float sparkle = sin(time * 5.0f) * 0.5f + 0.5f;
        return Color(255, 100 + 155 * sparkle, 255); // Magenta brillante
    }
                       // Bandas transportadoras con colores direccionales
    case CellType::UP_RIGHT:
        return Color(255, 180, 100); // Naranja
    case CellType::RIGHT:
        return Color(100, 180, 255); // Azul
    case CellType::DOWN_RIGHT:
        return Color(180, 100, 255); // Púrpura
    case CellType::DOWN_LEFT:
        return Color(255, 100, 180); // Rosa
    case CellType::LEFT:
        return Color(100, 255, 180); // Verde azulado
    case CellType::UP_LEFT:
        return Color(255, 255, 100); // Amarillo
    default:
        return Color::White;
    }
}

// Dibuja el grid completo con efectos modernos y profesionales
void drawGrid(RenderWindow& window, const HexGrid& grid,
    Player& player, CircleShape& hexagon,
    Text& text, Font& font, Clock& animClock, Clock& bgClock)
{
    // Dibujar fondo animado avanzado
    drawAnimatedBackground(window, bgClock);

    // Dibujar título del juego
    drawGameTitle(window, font, animClock);

    // Actualizar la animación del jugador
    player.updateMovement();

    // Calcular offset para centrar el grid (ajustado para menos espacio del título)
    float gridWidth = grid.cols() * 50.0f + 25.0f;
    float gridHeight = grid.rows() * 40.0f + 50.0f;
    float offsetX = (window.getSize().x - gridWidth - 200) / 2.0f;
    float offsetY = ((window.getSize().y - gridHeight) / 2.0f) - 10.0f; // Reducido de +10 a -10 para subir el grid

    // Dibujar celdas del grid con efectos mejorados
    for (int y = 0; y < grid.rows(); ++y) {
        for (int x = 0; x < grid.cols(); ++x) {
            const auto& cell = grid.at(y, x);
            Vector2f pos = grid.toPixel(y, x);

            // Aplicar offset para centrar
            pos.x += offsetX;
            pos.y += offsetY;

            // Sombra múltiple para profundidad
            for (int i = 3; i > 0; --i) {
                CircleShape shadow = hexagon;
                shadow.setPosition(pos.x + i * 2, pos.y + i * 2);
                shadow.setFillColor(Color(0, 0, 0, 30 - i * 5));
                shadow.setOutlineThickness(0);
                window.draw(shadow);
            }

            // Hexágono principal con colores mejorados
            hexagon.setPosition(pos);
            hexagon.setFillColor(getCellColor(cell.type, animClock));

            // Efectos especiales según tipo de celda
            if (cell.type == CellType::GOAL) {
                // Aura dorada para la meta
                CircleShape aura(35, 6);
                aura.setOrigin(35, 35);
                aura.setPosition(pos);
                aura.setFillColor(Color(255, 215, 0, 50));
                aura.setOutlineColor(Color(255, 255, 0, 100));
                aura.setOutlineThickness(2);
                window.draw(aura);

                hexagon.setOutlineColor(Color(255, 255, 255));
                hexagon.setOutlineThickness(4);
            }
            else if (cell.type == CellType::START) {
                // Aura verde para el inicio
                CircleShape aura(35, 6);
                aura.setOrigin(35, 35);
                aura.setPosition(pos);
                aura.setFillColor(Color(0, 255, 0, 40));
                window.draw(aura);

                hexagon.setOutlineColor(Color(255, 255, 255));
                hexagon.setOutlineThickness(4);
            }
            else if (cell.type == CellType::ITEM) {
                // Efecto de cristal para items
                hexagon.setOutlineColor(Color(255, 100, 255));
                hexagon.setOutlineThickness(3);
            }
            else if (cell.type >= CellType::UP_RIGHT && cell.type <= CellType::DOWN_LEFT) {
                // Bordes energéticos para bandas transportadoras
                hexagon.setOutlineColor(Color(150, 200, 255));
                hexagon.setOutlineThickness(3);
            }
            else if (cell.type == CellType::WALL) {
                // Paredes con aspecto metálico
                hexagon.setOutlineColor(Color(60, 60, 80));
                hexagon.setOutlineThickness(3);
            }
            else {
                hexagon.setOutlineColor(Color(80, 90, 120));
                hexagon.setOutlineThickness(2);
            }

            window.draw(hexagon);

            // Texto con mejor contraste y efectos
            text.setString(CellTypeToString(cell.type));
            text.setCharacterSize(14);
            text.setStyle(Text::Bold);
            text.setFillColor(Color(20, 20, 40));
            text.setPosition(pos.x - 8.f, pos.y - 8.f);

            // Múltiples sombras para el texto
            for (int i = 2; i > 0; --i) {
                Text textShadow = text;
                textShadow.setFillColor(Color(255, 255, 255, 100 + i * 50));
                textShadow.setPosition(pos.x - 8.f + i, pos.y - 8.f + i);
                window.draw(textShadow);
            }
            window.draw(text);
        }
    }

    // Dibujar el jugador con efectos espectaculares
    CircleShape playerCircle(14); // Ligeramente más grande
    Vector2f playerPos = player.getVisualPosition(grid);

    // Aplicar el mismo offset al jugador
    playerPos.x += offsetX;
    playerPos.y += offsetY;

    // Múltiples sombras para el jugador
    for (int i = 4; i > 0; --i) {
        CircleShape playerShadow(14 + i);
        playerShadow.setPosition(playerPos.x + i * 2, playerPos.y + i * 2);
        playerShadow.setFillColor(Color(0, 0, 0, 60 - i * 10));
        playerShadow.setOrigin(14 + i, 14 + i);
        window.draw(playerShadow);
    }

    // Color del jugador con efectos avanzados
    Color playerColor;
    float time = animClock.getElapsedTime().asSeconds();

    if (player.isMoving) {
        // Efecto de estela durante movimiento
        float trail = sin(time * 15.0f) * 0.3f + 0.7f;
        playerColor = Color(255 * trail, 150, 150);
    }
    else if (player.canUseWallBreak()) {
        // Efecto arcoíris cuando puede romper paredes
        float pulse = sin(time * 8.0f) * 0.4f + 0.6f;
        playerColor = Color(150 + 105 * pulse, 255 * pulse, 150 + 105 * pulse);
    }
    else {
        // Azul con respiración suave
        float breath = sin(time * 2.0f) * 0.2f + 0.8f;
        playerColor = Color(100 * breath, 150 * breath, 255);
    }

    // Aura del jugador
    if (player.canUseWallBreak()) {
        for (int i = 3; i > 0; --i) {
            CircleShape aura(25 + i * 5);
            aura.setFillColor(Color(100, 255, 100, 30 - i * 8));
            aura.setOrigin(25 + i * 5, 25 + i * 5);
            aura.setPosition(playerPos);
            window.draw(aura);
        }
    }

    // Jugador principal con brillo
    playerCircle.setFillColor(playerColor);
    playerCircle.setOutlineColor(Color(255, 255, 255));
    playerCircle.setOutlineThickness(3);
    playerCircle.setOrigin(14, 14);
    playerCircle.setPosition(playerPos);
    window.draw(playerCircle);

    // Punto central brillante
    CircleShape playerCore(6);
    playerCore.setFillColor(Color(255, 255, 255, 200));
    playerCore.setOrigin(6, 6);
    playerCore.setPosition(playerPos);
    window.draw(playerCore);
}