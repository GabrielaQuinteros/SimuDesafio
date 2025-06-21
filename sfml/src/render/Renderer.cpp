#pragma once
#include "Renderer.hpp"
#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "model/Player.hpp"
#include "core/PathFinding.hpp"
#include <cmath>

using namespace model;
using namespace sf;

// Paleta de colores vibrante y profesional
const Color NEON_BLUE = Color(0, 200, 255);
const Color NEON_GREEN = Color(0, 255, 150);
const Color NEON_PURPLE = Color(180, 100, 255);
const Color NEON_ORANGE = Color(255, 150, 0);
const Color ELECTRIC_YELLOW = Color(255, 255, 0);
const Color CYBER_WHITE = Color(240, 240, 255);
const Color TECH_GRAY = Color(70, 90, 120);
const Color DARK_BLUE = Color(20, 40, 80);

CircleShape createHexagon()
{
    CircleShape hexagon(25, 6);
    hexagon.setOutlineColor(Color(80, 80, 80));
    hexagon.setFillColor(Color::White);
    hexagon.setOrigin(25, 25);
    hexagon.setOutlineThickness(2);
    return hexagon;
}

Color lerpColor(const Color& a, const Color& b, float t) {
    return Color(
        static_cast<Uint8>(a.r + (b.r - a.r) * t),
        static_cast<Uint8>(a.g + (b.g - a.g) * t),
        static_cast<Uint8>(a.b + (b.b - a.b) * t),
        static_cast<Uint8>(a.a + (b.a - a.a) * t)
    );
}

// Función para crear hexágonos decorativos
void drawDecorativeHex(RenderWindow& window, Vector2f position, float size, Color color, float time, bool rotating = true) {
    CircleShape hex(size, 6);
    hex.setOrigin(size, size);
    hex.setPosition(position);
    hex.setFillColor(Color::Transparent);
    hex.setOutlineColor(color);
    hex.setOutlineThickness(2);
    
    if (rotating) {
        hex.rotate(time * 30.0f);
    }
    
    window.draw(hex);
}

// Pantalla de victoria con temática hexagonal profesional
void drawVictoryScreen(RenderWindow& window, Font& font, float winTime, int turnCount, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();
    
    // Fondo con gradiente vibrante
    for (int i = 0; i < window.getSize().y; i += 4) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        Color currentColor = lerpColor(DARK_BLUE, Color(40, 20, 80), gradient);
        
        RectangleShape line(Vector2f(window.getSize().x, 4));
        line.setPosition(0, i);
        line.setFillColor(currentColor);
        window.draw(line);
    }

    // Hexágonos decorativos flotantes
    for (int i = 0; i < 20; ++i) {
        float x = 100 + (i % 4) * 200 + sin(time + i) * 50;
        float y = 100 + (i / 4) * 100 + cos(time * 1.5f + i) * 30;
        
        Color hexColor = (i % 3 == 0) ? NEON_BLUE : 
                        (i % 3 == 1) ? NEON_GREEN : NEON_PURPLE;
        hexColor.a = 100;
        
        drawDecorativeHex(window, Vector2f(x, y), 15 + (i % 3) * 5, hexColor, time * (1 + i % 3));
    }

    float centerX = window.getSize().x / 2.0f;
    float centerY = window.getSize().y / 2.0f;
    
    // Marco hexagonal principal
    for (int i = 0; i < 3; ++i) {
        CircleShape mainHex(120 + i * 25, 6);
        mainHex.setOrigin(120 + i * 25, 120 + i * 25);
        mainHex.setPosition(centerX, centerY);
        mainHex.setFillColor(Color::Transparent);
        
        Color hexColor;
        if (i == 0) hexColor = ELECTRIC_YELLOW;
        else if (i == 1) hexColor = NEON_BLUE;
        else hexColor = NEON_PURPLE;
        
        hexColor.a = 150 - i * 30;
        mainHex.setOutlineColor(hexColor);
        mainHex.setOutlineThickness(3);
        mainHex.rotate(time * 20.0f * (i + 1));
        window.draw(mainHex);
    }

    // Panel central con efecto de cristal
    RectangleShape centerPanel(Vector2f(400, 250));
    centerPanel.setOrigin(200, 125);
    centerPanel.setPosition(centerX, centerY);
    centerPanel.setFillColor(Color(20, 40, 80, 200));
    centerPanel.setOutlineColor(NEON_BLUE);
    centerPanel.setOutlineThickness(2);
    window.draw(centerPanel);

    // Líneas de circuito decorativas
    for (int i = 0; i < 6; ++i) {
        RectangleShape circuit(Vector2f(150, 2));
        circuit.setOrigin(75, 1);
        circuit.setPosition(centerX + (i % 3 - 1) * 100, centerY - 80 + (i / 3) * 160);
        
        float intensity = sin(time * 4.0f + i) * 0.5f + 0.5f;
        Color circuitColor = NEON_GREEN;
        circuitColor.a = static_cast<Uint8>(100 + 100 * intensity);
        circuit.setFillColor(circuitColor);
        window.draw(circuit);
    }

    // Título VICTORY con efecto neon
    Text victoryTitle;
    victoryTitle.setFont(font);
    victoryTitle.setCharacterSize(48);
    victoryTitle.setStyle(Text::Bold);
    victoryTitle.setString("VICTORY");
    
    FloatRect titleBounds = victoryTitle.getLocalBounds();
    victoryTitle.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    victoryTitle.setPosition(centerX, centerY - 60);
    
    float titleGlow = sin(time * 6.0f) * 0.4f + 0.6f;
    victoryTitle.setFillColor(Color(
        static_cast<Uint8>(ELECTRIC_YELLOW.r * titleGlow),
        static_cast<Uint8>(ELECTRIC_YELLOW.g * titleGlow),
        static_cast<Uint8>(ELECTRIC_YELLOW.b)
    ));
    window.draw(victoryTitle);

    // Subtítulo con estilo tech
    Text subtitle;
    subtitle.setFont(font);
    subtitle.setCharacterSize(16);
    subtitle.setString("MISION COMPLETADA");
    
    FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setOrigin(subtitleBounds.width / 2, subtitleBounds.height / 2);
    subtitle.setPosition(centerX, centerY - 20);
    subtitle.setFillColor(CYBER_WHITE);
    window.draw(subtitle);

    // Estadísticas con diseño futurista
    Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(14);
    timeText.setString("TIEMPO TOTAL: " + std::to_string(static_cast<int>(winTime)) + " SEGUNDOS");
    
    FloatRect timeTextBounds = timeText.getLocalBounds();
    timeText.setOrigin(timeTextBounds.width / 2, timeTextBounds.height / 2);
    timeText.setPosition(centerX, centerY + 15);
    timeText.setFillColor(NEON_GREEN);
    window.draw(timeText);

    Text turnsText;
    turnsText.setFont(font);
    turnsText.setCharacterSize(14);
    turnsText.setString("TURNOS EJECUTADOS: " + std::to_string(turnCount));
    
    FloatRect turnsTextBounds = turnsText.getLocalBounds();
    turnsText.setOrigin(turnsTextBounds.width / 2, turnsTextBounds.height / 2);
    turnsText.setPosition(centerX, centerY + 35);
    turnsText.setFillColor(NEON_BLUE);
    window.draw(turnsText);

    // Evaluación del rendimiento
    Text performanceText;
    performanceText.setFont(font);
    performanceText.setCharacterSize(13);
    
    std::string performance;
    Color perfColor;
    if (winTime < 30 && turnCount < 20) {
        performance = "RENDIMIENTO LEGENDARIO";
        perfColor = ELECTRIC_YELLOW;
    } else if (winTime < 60 && turnCount < 35) {
        performance = "EXCELENTE ESTRATEGIA";
        perfColor = NEON_PURPLE;
    } else if (winTime < 120) {
        performance = "BUEN TRABAJO";
        perfColor = NEON_GREEN;
    } else {
        performance = "MISION CUMPLIDA";
        perfColor = NEON_BLUE;
    }
    
    performanceText.setString(performance);
    FloatRect perfBounds = performanceText.getLocalBounds();
    performanceText.setOrigin(perfBounds.width / 2, perfBounds.height / 2);
    performanceText.setPosition(centerX, centerY + 65);
    performanceText.setFillColor(perfColor);
    window.draw(performanceText);

    // Instrucciones para salir
    Text exitText;
    exitText.setFont(font);
    exitText.setCharacterSize(11);
    exitText.setString("PRESIONA ESC PARA SALIR");
    
    FloatRect exitBounds = exitText.getLocalBounds();
    exitText.setOrigin(exitBounds.width / 2, exitBounds.height / 2);
    exitText.setPosition(centerX, centerY + 95);
    
    float exitAlpha = sin(time * 3.0f) * 100 + 155;
    exitText.setFillColor(Color(CYBER_WHITE.r, CYBER_WHITE.g, CYBER_WHITE.b, static_cast<Uint8>(exitAlpha)));
    window.draw(exitText);
}

// Título del juego con elementos hexagonales
void drawGameTitle(RenderWindow& window, Font& font, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();

    // Hexágonos decorativos alrededor del título
    float centerX = window.getSize().x / 2.0f;
    for (int i = 0; i < 8; ++i) {
        float angle = (i / 8.0f) * 2 * 3.14159f;
        float x = centerX + cos(angle + time * 0.5f) * 120;
        float y = 30 + sin(angle + time * 0.5f) * 15;
        
        Color hexColor = (i % 2 == 0) ? NEON_BLUE : NEON_GREEN;
        hexColor.a = 120;
        drawDecorativeHex(window, Vector2f(x, y), 8, hexColor, time * 45.0f);
    }

    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setStyle(Text::Bold);
    titleText.setString("HexEscape");

    FloatRect titleBounds = titleText.getLocalBounds();
    float titleX = (window.getSize().x - titleBounds.width) / 2.0f;
    float titleY = 15.0f;

    // Título sin efectos de brillo, color sólido
    titleText.setPosition(titleX, titleY);
    titleText.setFillColor(ELECTRIC_YELLOW);
    window.draw(titleText);

    // Subtítulo centrado entre las líneas
    Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setCharacterSize(12);
    subtitleText.setString("FABRICA DE ROMPECABEZAS ELITE");

    FloatRect subtitleBounds = subtitleText.getLocalBounds();
    float subtitleX = (window.getSize().x - subtitleBounds.width) / 2.0f;
    float subtitleY = titleY + 40.0f;

    // Línea izquierda
    RectangleShape leftLine(Vector2f(80, 2));
    leftLine.setPosition(subtitleX - 90, subtitleY + 8);
    leftLine.setFillColor(NEON_BLUE);
    window.draw(leftLine);

    // Subtítulo centrado
    subtitleText.setPosition(subtitleX, subtitleY);
    subtitleText.setFillColor(CYBER_WHITE);
    window.draw(subtitleText);

    // Línea derecha
    RectangleShape rightLine(Vector2f(80, 2));
    rightLine.setPosition(subtitleX + subtitleBounds.width + 10, subtitleY + 8);
    rightLine.setFillColor(NEON_BLUE);
    window.draw(rightLine);
}

// Fondo vibrante con elementos hexagonales
void drawAnimatedBackground(RenderWindow& window, Clock& bgClock) {
    float time = bgClock.getElapsedTime().asSeconds();

    // Gradiente vibrante de fondo
    for (int i = 0; i < window.getSize().y; i += 3) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        float wave = sin(time * 2.0f + gradient * 8.0f) * 0.2f + 0.8f;
        
        Color topColor(15, 25, 60);
        Color midColor(25, 45, 90);
        Color bottomColor(35, 55, 110);
        
        Color currentColor;
        if (gradient < 0.5f) {
            currentColor = lerpColor(topColor, midColor, gradient * 2.0f * wave);
        } else {
            currentColor = lerpColor(midColor, bottomColor, (gradient - 0.5f) * 2.0f * wave);
        }

        RectangleShape line(Vector2f(window.getSize().x, 3));
        line.setPosition(0, i);
        line.setFillColor(currentColor);
        window.draw(line);
    }

    // Hexágonos flotantes como partículas
    for (int i = 0; i < 25; ++i) {
        float x = fmod(time * 40.0f + i * 50.0f, window.getSize().x + 100.0f) - 50.0f;
        float y = 100.0f + sin(time * 1.2f + i * 0.5f) * 40.0f + i * 20.0f;
        
        if (y < window.getSize().y) {
            Color hexColor;
            if (i % 4 == 0) hexColor = NEON_BLUE;
            else if (i % 4 == 1) hexColor = NEON_GREEN;
            else if (i % 4 == 2) hexColor = NEON_PURPLE;
            else hexColor = NEON_ORANGE;
            
            hexColor.a = 80 + (i % 50);
            drawDecorativeHex(window, Vector2f(x, y), 3 + (i % 3), hexColor, time * 60.0f);
        }
    }

    // Líneas de circuito de fondo
    for (int i = 0; i < 8; ++i) {
        float y = (i * window.getSize().y / 8.0f) + sin(time * 1.5f + i) * 20;
        
        RectangleShape circuit(Vector2f(window.getSize().x, 1));
        circuit.setPosition(0, y);
        
        float intensity = sin(time * 3.0f + i * 0.8f) * 0.3f + 0.4f;
        Color circuitColor = NEON_BLUE;
        circuitColor.a = static_cast<Uint8>(50 * intensity);
        circuit.setFillColor(circuitColor);
        window.draw(circuit);
    }
}

// Barra de energía con diseño hexagonal futurista
void drawModernEnergyBar(RenderWindow& window, const Player& player, Font& font, Clock& animClock) {
    const float barWidth = 240.0f;
    const float barHeight = 20.0f;
    const float barX = 25.0f;
    const float barY = window.getSize().y - 80.0f;

    float time = animClock.getElapsedTime().asSeconds();

    // Hexágonos decorativos alrededor de la barra
    for (int i = 0; i < 3; ++i) {
        float hexX = barX - 20 + i * 20;
        float hexY = barY + 10;
        
        Color hexColor = NEON_GREEN;
        hexColor.a = 100;
        drawDecorativeHex(window, Vector2f(hexX, hexY), 6, hexColor, time * 90.0f);
    }

    // Marco de la barra con efecto tech
    RectangleShape energyFrame(Vector2f(barWidth, barHeight));
    energyFrame.setPosition(barX, barY);
    energyFrame.setFillColor(Color(20, 40, 80, 180));
    energyFrame.setOutlineColor(TECH_GRAY);
    energyFrame.setOutlineThickness(2);
    window.draw(energyFrame);

    // Líneas de división hexagonales
    for (int i = 1; i < 10; ++i) {
        float divX = barX + (barWidth / 10.0f) * i;
        RectangleShape divider(Vector2f(1, barHeight));
        divider.setPosition(divX, barY);
        divider.setFillColor(Color(TECH_GRAY.r, TECH_GRAY.g, TECH_GRAY.b, 100));
        window.draw(divider);
    }

    // Relleno de energía con efectos
    float fillWidth = barWidth * player.getEnergyPercentage();
    if (fillWidth > 0) {
        RectangleShape energyFill(Vector2f(fillWidth, barHeight));
        energyFill.setPosition(barX, barY);

        Color energyColor;
        if (player.isEnergyFull()) {
            float pulse = sin(time * 8.0f) * 0.4f + 0.6f;
            energyColor = Color(
                static_cast<Uint8>(ELECTRIC_YELLOW.r * pulse),
                static_cast<Uint8>(ELECTRIC_YELLOW.g * pulse),
                static_cast<Uint8>(ELECTRIC_YELLOW.b)
            );
            
            // Partículas de energía
            for (int i = 0; i < 5; ++i) {
                float sparkX = barX + fillWidth * 0.8f + (i * 10);
                float sparkY = barY + 10 + sin(time * 12.0f + i) * 5;
                
                CircleShape spark(2);
                spark.setPosition(sparkX, sparkY);
                spark.setFillColor(Color(255, 255, 255, 200));
                window.draw(spark);
            }
        }
        else if (player.getEnergyPercentage() > 0.6f) {
            energyColor = NEON_GREEN;
        }
        else if (player.getEnergyPercentage() > 0.3f) {
            energyColor = NEON_ORANGE;
        }
        else {
            energyColor = Color(255, 100, 100);
        }

        energyFill.setFillColor(energyColor);
        window.draw(energyFill);

        // Líneas de flujo de energía
        for (int i = 0; i < 3; ++i) {
            float lineX = barX + (fillWidth * 0.3f) + i * (fillWidth * 0.25f);
            if (lineX < barX + fillWidth - 5) {
                RectangleShape energyLine(Vector2f(2, barHeight - 6));
                energyLine.setPosition(lineX, barY + 3);
                
                float lineIntensity = sin(time * 10.0f + i * 2.0f) * 0.5f + 0.5f;
                energyLine.setFillColor(Color(255, 255, 255, static_cast<Uint8>(150 * lineIntensity)));
                window.draw(energyLine);
            }
        }
    }

    // Texto de energía con estilo futurista
    Text energyText;
    energyText.setFont(font);
    energyText.setCharacterSize(14);
    energyText.setFillColor(CYBER_WHITE);
    energyText.setStyle(Text::Bold);
    energyText.setString("ENERGIA: " + std::to_string(player.energy) + "/" + std::to_string(Player::MAX_ENERGY));
    energyText.setPosition(barX, barY - 22);
    window.draw(energyText);

    // Indicador de habilidad
    if (player.canUseWallBreak()) {
        float pulse = sin(time * 8.0f) * 0.5f + 0.5f;

        Text abilityText;
        abilityText.setFont(font);
        abilityText.setCharacterSize(13);
        abilityText.setFillColor(Color(
            static_cast<Uint8>(ELECTRIC_YELLOW.r * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.g * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.b)
        ));
        abilityText.setStyle(Text::Bold);
        abilityText.setString("SPACE: DESTRUIR PARED");
        abilityText.setPosition(barX, barY - 42);
        window.draw(abilityText);
    }
    else if (player.isSelectingWall) {
        Text selectText;
        selectText.setFont(font);
        selectText.setCharacterSize(12);
        selectText.setFillColor(NEON_ORANGE);
        selectText.setString("SELECCIONA DIRECCION - ESC: CANCELAR");
        selectText.setPosition(barX, barY - 42);
        window.draw(selectText);
    }
    else {
        Text waitText;
        waitText.setFont(font);
        waitText.setCharacterSize(11);
        waitText.setFillColor(Color(150, 180, 200));
        waitText.setString("ACUMULA ENERGIA PARA HABILIDADES");
        waitText.setPosition(barX, barY - 42);
        window.draw(waitText);
    }
}

// Panel de información con temática hexagonal
void drawGameInfo(RenderWindow& window, Font& font, int turnCount, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();
    
    // Panel principal con diseño futurista
    RectangleShape panel(Vector2f(140, 100));
    panel.setPosition(window.getSize().x - 160, 80);
    panel.setFillColor(Color(20, 40, 80, 200));
    panel.setOutlineColor(NEON_BLUE);
    panel.setOutlineThickness(2);
    window.draw(panel);

    // Hexágonos decorativos en las esquinas
    Vector2f corners[] = {
        Vector2f(window.getSize().x - 155, 85),
        Vector2f(window.getSize().x - 25, 85),
        Vector2f(window.getSize().x - 155, 175),
        Vector2f(window.getSize().x - 25, 175)
    };
    
    for (int i = 0; i < 4; ++i) {
        Color hexColor = (i % 2 == 0) ? NEON_GREEN : NEON_PURPLE;
        hexColor.a = 120;
        drawDecorativeHex(window, corners[i], 6, hexColor, time * 60.0f);
    }

    // Líneas de circuito decorativas
    for (int i = 0; i < 2; ++i) {
        RectangleShape circuit(Vector2f(120, 1));
        circuit.setPosition(window.getSize().x - 150, 105 + i * 25);
        circuit.setFillColor(Color(NEON_GREEN.r, NEON_GREEN.g, NEON_GREEN.b, 80));
        window.draw(circuit);
    }

    // Título del panel
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(12);
    titleText.setFillColor(CYBER_WHITE);
    titleText.setStyle(Text::Bold);
    titleText.setString("STATUS");
    titleText.setPosition(window.getSize().x - 145, 90);
    window.draw(titleText);

    // Información de turnos
    Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(11);
    turnText.setFillColor(CYBER_WHITE);
    turnText.setString("TURNS: " + std::to_string(turnCount));
    turnText.setPosition(window.getSize().x - 145, 110);
    window.draw(turnText);

    // Próximo evento
    int turnsUntilWall = 5 - (turnCount % 5);
    Text eventText;
    eventText.setFont(font);
    eventText.setCharacterSize(11);
    eventText.setFillColor(CYBER_WHITE);
    eventText.setString("WALL: " + std::to_string(turnsUntilWall == 5 ? 0 : turnsUntilWall));
    eventText.setPosition(window.getSize().x - 145, 125);
    window.draw(eventText);

    // Cronómetro
    Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(10);
    timeText.setFillColor(CYBER_WHITE);
    timeText.setString("TIME: " + std::to_string(static_cast<int>(time)) + "S");
    timeText.setPosition(window.getSize().x - 145, 140);
    window.draw(timeText);

    // Estado del sistema
    Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(10);
    statusText.setFillColor(CYBER_WHITE);
    statusText.setString("SYSTEM: ONLINE");
    statusText.setPosition(window.getSize().x - 145, 155);
    window.draw(statusText);
}

// Panel de controles con diseño hexagonal
void drawModernControls(RenderWindow& window, Font& font, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();
    
    // Panel principal
    RectangleShape controlPanel(Vector2f(140, 100));
    controlPanel.setPosition(window.getSize().x - 160, 190);
    controlPanel.setFillColor(Color(20, 40, 80, 200));
    controlPanel.setOutlineColor(NEON_PURPLE);
    controlPanel.setOutlineThickness(2);
    window.draw(controlPanel);

    // Hexágonos decorativos en el panel de controles
    for (int i = 0; i < 4; ++i) {
        float hexX = window.getSize().x - 145 + (i % 2) * 30;
        float hexY = 205 + (i / 2) * 50;
        
        Color hexColor = (i % 2 == 0) ? NEON_BLUE : NEON_GREEN;
        hexColor.a = 80;
        drawDecorativeHex(window, Vector2f(hexX, hexY), 5, hexColor, time * 45.0f + i * 60);
    }

    // Título
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(12);
    titleText.setFillColor(CYBER_WHITE);
    titleText.setStyle(Text::Bold);
    titleText.setString("CONTROLS");
    titleText.setPosition(window.getSize().x - 145, 200);
    window.draw(titleText);

    // Controles simplificados
    std::vector<std::string> controls = {
        "W/E: UP",
        "A/D: SIDE", 
        "Z/X: DOWN",
        "SPACE: POWER"
    };

    for (size_t i = 0; i < controls.size(); ++i) {
        Text controlText;
        controlText.setFont(font);
        controlText.setCharacterSize(10);
        controlText.setFillColor(CYBER_WHITE);
        controlText.setString(controls[i]);
        controlText.setPosition(window.getSize().x - 145, 220 + i * 14);
        window.draw(controlText);
    }

    // Línea de separación
    RectangleShape separator(Vector2f(120, 1));
    separator.setPosition(window.getSize().x - 150, 275);
    separator.setFillColor(Color(NEON_PURPLE.r, NEON_PURPLE.g, NEON_PURPLE.b, 150));
    window.draw(separator);
}

// Colores de celdas vibrantes y profesionales
Color getCellColor(CellType type, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();

    switch (type) {
    case CellType::EMPTY:
        return Color(220, 235, 255, 180);
    case CellType::WALL: {
        float pulse = sin(time * 1.5f) * 0.2f + 0.8f;
        return Color(
            static_cast<Uint8>(TECH_GRAY.r * pulse),
            static_cast<Uint8>(TECH_GRAY.g * pulse),
            static_cast<Uint8>(TECH_GRAY.b * pulse)
        );
    }
    case CellType::START: {
        float pulse = sin(time * 3.0f) * 0.4f + 0.6f;
        return Color(
            0,
            static_cast<Uint8>(NEON_GREEN.g * pulse),
            static_cast<Uint8>(NEON_GREEN.b * pulse)
        );
    }
    case CellType::GOAL: {
        float pulse = sin(time * 4.0f) * 0.5f + 0.5f;
        return Color(
            static_cast<Uint8>(ELECTRIC_YELLOW.r * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.g * pulse),
            0
        );
    }
    case CellType::ITEM: {
        float sparkle = sin(time * 6.0f) * 0.4f + 0.6f;
        return Color(
            static_cast<Uint8>(NEON_PURPLE.r * sparkle),
            static_cast<Uint8>(NEON_PURPLE.g * sparkle),
            static_cast<Uint8>(NEON_PURPLE.b * sparkle)
        );
    }
    // Bandas transportadoras con colores vibrantes
    case CellType::UP_RIGHT: {
        float flow = sin(time * 5.0f) * 0.3f + 0.7f;
        return Color(255, static_cast<Uint8>(150 * flow), 0);
    }
    case CellType::RIGHT: {
        float flow = sin(time * 5.0f + 1.0f) * 0.3f + 0.7f;
        return Color(0, static_cast<Uint8>(200 * flow), 255);
    }
    case CellType::DOWN_RIGHT: {
        float flow = sin(time * 5.0f + 2.0f) * 0.3f + 0.7f;
        return Color(static_cast<Uint8>(180 * flow), 0, 255);
    }
    case CellType::DOWN_LEFT: {
        float flow = sin(time * 5.0f + 3.0f) * 0.3f + 0.7f;
        return Color(255, 0, static_cast<Uint8>(180 * flow));
    }
    case CellType::LEFT: {
        float flow = sin(time * 5.0f + 4.0f) * 0.3f + 0.7f;
        return Color(0, 255, static_cast<Uint8>(150 * flow));
    }
    case CellType::UP_LEFT: {
        float flow = sin(time * 5.0f + 5.0f) * 0.3f + 0.7f;
        return Color(255, 255, static_cast<Uint8>(100 * flow));
    }
    default:
        return Color::White;
    }
}

// Grid principal con efectos profesionales y hexagonales
void drawGrid(RenderWindow& window, const HexGrid& grid,
    Player& player, CircleShape& hexagon,
    Text& text, Font& font, Clock& animClock, Clock& bgClock,
    std::set<std::pair<int, int>>& pathCells)
{


    // Dibujar fondo vibrante
    drawAnimatedBackground(window, bgClock);

    // Dibujar título con elementos hexagonales
    drawGameTitle(window, font, animClock);

    // Actualizar animación del jugador
    player.updateMovement();

    // Centrar grid
    float gridWidth = grid.cols() * 50.0f + 25.0f;
    float gridHeight = grid.rows() * 40.0f + 50.0f;
    float offsetX = (window.getSize().x - gridWidth - 220) / 2.0f;
    float offsetY = ((window.getSize().y - gridHeight) / 2.0f) + 10.0f;

    float time = animClock.getElapsedTime().asSeconds();

    // Hexágonos decorativos alrededor del grid
    for (int i = 0; i < 12; ++i) {
        float angle = (i / 12.0f) * 2 * 3.14159f;
        float radius = 200 + sin(time * 2.0f + i) * 20;
        float x = offsetX + gridWidth/2 + cos(angle + time * 0.3f) * radius;
        float y = offsetY + gridHeight/2 + sin(angle + time * 0.3f) * radius;
        
        if (x > 0 && x < window.getSize().x - 220 && y > 80 && y < window.getSize().y - 100) {
            Color hexColor = (i % 3 == 0) ? NEON_BLUE : 
                            (i % 3 == 1) ? NEON_GREEN : NEON_PURPLE;
            hexColor.a = 60;
            drawDecorativeHex(window, Vector2f(x, y), 8, hexColor, time * 30.0f + i * 30);
        }
    }

    // Dibujar celdas del grid
    for (int y = 0; y < grid.rows(); ++y) {
        for (int x = 0; x < grid.cols(); ++x) {
            const auto& cell = grid.at(y, x);
            Vector2f pos = grid.toPixel(y, x);

            pos.x += offsetX;
            pos.y += offsetY;

            // Sombra con efecto de profundidad
            CircleShape shadow = hexagon;
            shadow.setPosition(pos.x + 3, pos.y + 3);
            shadow.setFillColor(Color(0, 0, 0, 120));
            shadow.setOutlineThickness(0);
            window.draw(shadow);

            // Hexágono principal
            hexagon.setPosition(pos);
            hexagon.setFillColor(getCellColor(cell.type, animClock));

            // Efectos especiales por tipo de celda
            if (cell.type == CellType::GOAL) {
                // Anillos de energía para la meta
                for (int i = 0; i < 3; ++i) {
                    CircleShape ring(30 + i * 10, 6);
                    ring.setOrigin(30 + i * 10, 30 + i * 10);
                    ring.setPosition(pos);
                    ring.setFillColor(Color::Transparent);
                    
                    float ringIntensity = sin(time * 4.0f + i * 2.0f) * 0.4f + 0.6f;
                    Color ringColor = ELECTRIC_YELLOW;
                    ringColor.a = static_cast<Uint8>(100 * ringIntensity);
                    ring.setOutlineColor(ringColor);
                    ring.setOutlineThickness(2);
                    ring.rotate(time * 45.0f * (i + 1));
                    window.draw(ring);
                }

                hexagon.setOutlineColor(ELECTRIC_YELLOW);
                hexagon.setOutlineThickness(4);
            }
            else if (cell.type == CellType::START) {
                // Pulso verde para el inicio
                CircleShape pulse(35, 6);
                pulse.setOrigin(35, 35);
                pulse.setPosition(pos);
                pulse.setFillColor(Color::Transparent);
                
                float pulseIntensity = sin(time * 3.0f) * 0.5f + 0.5f;
                Color pulseColor = NEON_GREEN;
                pulseColor.a = static_cast<Uint8>(120 * pulseIntensity);
                pulse.setOutlineColor(pulseColor);
                pulse.setOutlineThickness(3);
                window.draw(pulse);

                hexagon.setOutlineColor(NEON_GREEN);
                hexagon.setOutlineThickness(3);
            }
            else if (cell.type == CellType::ITEM) {
                // Efecto de cristal para items
                float itemSparkle = sin(time * 8.0f) * 0.4f + 0.6f;
                
                CircleShape crystal(32, 6);
                crystal.setOrigin(32, 32);
                crystal.setPosition(pos);
                crystal.setFillColor(Color::Transparent);
                crystal.setOutlineColor(Color(
                    static_cast<Uint8>(NEON_PURPLE.r * itemSparkle),
                    static_cast<Uint8>(NEON_PURPLE.g * itemSparkle),
                    static_cast<Uint8>(NEON_PURPLE.b * itemSparkle)
                ));
                crystal.setOutlineThickness(2);
                window.draw(crystal);
                
                hexagon.setOutlineColor(NEON_PURPLE);
                hexagon.setOutlineThickness(3);
            }
            else if (cell.type >= CellType::UP_RIGHT && cell.type <= CellType::DOWN_LEFT) {
                // Efectos de flujo para bandas transportadoras
                float flowEffect = sin(time * 6.0f + x + y) * 0.4f + 0.6f;
                
                CircleShape flow(28, 6);
                flow.setOrigin(28, 28);
                flow.setPosition(pos);
                flow.setFillColor(Color::Transparent);
                flow.setOutlineColor(Color(
                    static_cast<Uint8>(NEON_BLUE.r * flowEffect),
                    static_cast<Uint8>(NEON_BLUE.g * flowEffect),
                    static_cast<Uint8>(NEON_BLUE.b * flowEffect)
                ));
                flow.setOutlineThickness(2);
                window.draw(flow);
                
                hexagon.setOutlineColor(NEON_BLUE);
                hexagon.setOutlineThickness(2);
            }
            else if (cell.type == CellType::WALL) {
                hexagon.setOutlineColor(TECH_GRAY);
                hexagon.setOutlineThickness(3);
            }
            else {
                hexagon.setOutlineColor(Color(120, 140, 160));
                hexagon.setOutlineThickness(1);
            }
            // Si la celda está en el path, resáltala
            if (pathCells.count({ cell.row, cell.col })) {
                hexagon.setOutlineColor(sf::Color::Red); // o el color que quieras
                hexagon.setOutlineThickness(4.f); // grosor para que resalte
            }
           
            

            window.draw(hexagon);

            // Texto con mejor contraste
            text.setString(CellTypeToString(cell.type));
            text.setCharacterSize(14);
            text.setStyle(Text::Bold);
            text.setFillColor(Color(30, 30, 50));
            text.setPosition(pos.x - 7.f, pos.y - 7.f);
            window.draw(text);
        }
    }

    // Dibujar jugador con efectos avanzados
    CircleShape playerCircle(16);
    Vector2f playerPos = player.getVisualPosition(grid);

    playerPos.x += offsetX;
    playerPos.y += offsetY;

    // Sombra del jugador
    CircleShape playerShadow(18);
    playerShadow.setPosition(playerPos.x + 4, playerPos.y + 4);
    playerShadow.setFillColor(Color(0, 0, 0, 150));
    playerShadow.setOrigin(18, 18);
    window.draw(playerShadow);

    // Color y efectos del jugador
    Color playerColor;
    if (player.isMoving) {
        float trail = sin(time * 20.0f) * 0.4f + 0.6f;
        playerColor = Color(
            static_cast<Uint8>(255 * trail),
            static_cast<Uint8>(150 * trail),
            static_cast<Uint8>(150 * trail)
        );
    }
    else if (player.canUseWallBreak()) {
        float pulse = sin(time * 8.0f) * 0.5f + 0.5f;
        playerColor = Color(
            static_cast<Uint8>(ELECTRIC_YELLOW.r * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.g * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.b)
        );
        
        // Aura de poder hexagonal
        for (int i = 0; i < 3; ++i) {
            CircleShape powerHex(25 + i * 8, 6);
            powerHex.setOrigin(25 + i * 8, 25 + i * 8);
            powerHex.setPosition(playerPos);
            powerHex.setFillColor(Color::Transparent);
            
            Color auraColor = ELECTRIC_YELLOW;
            auraColor.a = 80 - i * 20;
            powerHex.setOutlineColor(auraColor);
            powerHex.setOutlineThickness(2);
            powerHex.rotate(time * 60.0f * (i + 1));
            window.draw(powerHex);
        }
    }
    else if (player.isSelectingWall) {
        playerColor = NEON_ORANGE;
        
        // Retícula de selección hexagonal
        CircleShape targetHex(30, 6);
        targetHex.setOrigin(30, 30);
        targetHex.setPosition(playerPos);
        targetHex.setFillColor(Color::Transparent);
        targetHex.setOutlineColor(NEON_ORANGE);
        targetHex.setOutlineThickness(3);
        targetHex.rotate(time * 90.0f);
        window.draw(targetHex);
    }
    else {
        float breath = sin(time * 2.0f) * 0.3f + 0.7f;
        playerColor = Color(
            static_cast<Uint8>(NEON_BLUE.r * breath),
            static_cast<Uint8>(NEON_BLUE.g * breath),
            static_cast<Uint8>(NEON_BLUE.b)
        );
    }

    // Jugador principal
    playerCircle.setFillColor(playerColor);
    playerCircle.setOutlineColor(CYBER_WHITE);
    playerCircle.setOutlineThickness(3);
    playerCircle.setOrigin(16, 16);
    playerCircle.setPosition(playerPos);
    window.draw(playerCircle);

    // Núcleo del jugador con efecto hexagonal
    CircleShape playerCore(8, 6);
    playerCore.setFillColor(CYBER_WHITE);
    playerCore.setOrigin(8, 8);
    playerCore.setPosition(playerPos);
    window.draw(playerCore);

    // Punto central
    CircleShape centerDot(3);
    centerDot.setFillColor(Color(50, 50, 80));
    centerDot.setOrigin(3, 3);
    centerDot.setPosition(playerPos);
    window.draw(centerDot);
}