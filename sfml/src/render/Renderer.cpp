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


// NUEVOS COLORES PARA EL CAMINO DE PATHFINDING
const Color PATH_RED = Color(255, 50, 50);
const Color PATH_GLOW = Color(255, 100, 100, 180);
const Color PATH_BRIGHT = Color(255, 255, 255);


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
}


// Barra de energía con diseño hexagonal futurista
void drawModernEnergyBar(RenderWindow& window, const Player& player, Font& font, Clock& animClock) {
    const float barWidth = 240.0f;
    const float barHeight = 20.0f;
    const float barX = 25.0f;
    const float barY = window.getSize().y - 80.0f;


    float time = animClock.getElapsedTime().asSeconds();


    // Marco de la barra con efecto tech
    RectangleShape energyFrame(Vector2f(barWidth, barHeight));
    energyFrame.setPosition(barX, barY);
    energyFrame.setFillColor(Color(20, 40, 80, 180));
    energyFrame.setOutlineColor(TECH_GRAY);
    energyFrame.setOutlineThickness(2);
    window.draw(energyFrame);


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


// Panel de información con temática hexagonal - ACTUALIZADO CON MODOS CORREGIDOS
void drawGameInfo(RenderWindow& window, Font& font, int turnCount, Clock& animClock,
                 bool showPathVisualization, bool autoSolveMode) {
    float time = animClock.getElapsedTime().asSeconds();
   
    // Panel principal con diseño futurista
    RectangleShape panel(Vector2f(140, 160));
    panel.setPosition(window.getSize().x - 160, 80);
    panel.setFillColor(Color(20, 40, 80, 200));
    panel.setOutlineColor(NEON_BLUE);
    panel.setOutlineThickness(2);
    window.draw(panel);


    // Hexágonos decorativos en las esquinas
    Vector2f corners[] = {
        Vector2f(window.getSize().x - 155, 85),
        Vector2f(window.getSize().x - 25, 85),
        Vector2f(window.getSize().x - 155, 235),
        Vector2f(window.getSize().x - 25, 235)
    };
   
    for (int i = 0; i < 4; ++i) {
        Color hexColor = (i % 2 == 0) ? NEON_GREEN : NEON_PURPLE;
        hexColor.a = 120;
        drawDecorativeHex(window, corners[i], 6, hexColor, time * 60.0f);
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


    // INFORMACIÓN DEL MODO ACTUAL - CORREGIDA
    Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(11);
    modeText.setStyle(Text::Bold);
    modeText.setPosition(window.getSize().x - 145, 160);


    if (autoSolveMode && showPathVisualization) {
        // Modo: Ejecutando camino mostrado
        modeText.setFillColor(ELECTRIC_YELLOW);
        modeText.setString("MODE: EXECUTING");
    } else if (autoSolveMode && !showPathVisualization) {
        // Modo: Auto-resolución directa
        modeText.setFillColor(NEON_ORANGE);
        modeText.setString("MODE: AUTO");
    } else if (showPathVisualization && !autoSolveMode) {
        // Modo: Solo mostrando camino
        modeText.setFillColor(PATH_RED);
        modeText.setString("MODE: PREVIEW");
    } else {
        // Modo manual
        modeText.setFillColor(NEON_GREEN);
        modeText.setString("MODE: MANUAL");
    }
    window.draw(modeText);


    // Instrucciones contextuales
    Text instructionText;
    instructionText.setFont(font);
    instructionText.setCharacterSize(9);
    instructionText.setPosition(window.getSize().x - 145, 180);


    if (autoSolveMode && showPathVisualization) {
        instructionText.setFillColor(ELECTRIC_YELLOW);
        instructionText.setString("EXECUTING PATH...");
    } else if (autoSolveMode && !showPathVisualization) {
        instructionText.setFillColor(NEON_ORANGE);
        instructionText.setString("AUTO-SOLVING...");
    } else if (showPathVisualization && !autoSolveMode) {
        instructionText.setFillColor(ELECTRIC_YELLOW);
        instructionText.setString("T: EXECUTE | ESC: CANCEL");
    } else {
        instructionText.setFillColor(CYBER_WHITE);
        instructionText.setString("P: PREVIEW | R: AUTO");
    }
    window.draw(instructionText);


    // Opciones disponibles
    Text optionsText;
    optionsText.setFont(font);
    optionsText.setCharacterSize(8);
    optionsText.setFillColor(Color(150, 180, 200));
    optionsText.setPosition(window.getSize().x - 145, 200);
   
    if (autoSolveMode) {
        optionsText.setString("ESC: STOP");
    } else if (showPathVisualization) {
        optionsText.setString("T: RUN | R: DIRECT");
    } else {
        optionsText.setString("P: SHOW | R: SOLVE");
    }
    window.draw(optionsText);
}


// Panel de controles con diseño hexagonal - ACTUALIZADO CON CONTROLES CORREGIDOS
void drawModernControls(RenderWindow& window, Font& font, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();
   
    // Panel principal (más grande para más controles)
    RectangleShape controlPanel(Vector2f(140, 160));
    controlPanel.setPosition(window.getSize().x - 160, 260);
    controlPanel.setFillColor(Color(20, 40, 80, 200));
    controlPanel.setOutlineColor(NEON_PURPLE);
    controlPanel.setOutlineThickness(2);
    window.draw(controlPanel);


    // Hexágonos decorativos en el panel de controles
    for (int i = 0; i < 4; ++i) {
        float hexX = window.getSize().x - 145 + (i % 2) * 30;
        float hexY = 275 + (i / 2) * 60;
       
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
    titleText.setPosition(window.getSize().x - 145, 270);
    window.draw(titleText);


    // Controles actualizados
    std::vector<std::string> controls;
    controls.push_back("W/E: UP");
    controls.push_back("A/D: SIDE");
    controls.push_back("Z/X: DOWN");
    controls.push_back("SPACE: POWER");
    controls.push_back("P: SHOW PATH");
    controls.push_back("R: AUTO-SOLVE");    
    controls.push_back("T: RUN PATH");      
    controls.push_back("ESC: CANCEL");


    for (size_t i = 0; i < controls.size(); ++i) {
        Text controlText;
        controlText.setFont(font);
        controlText.setCharacterSize(9);
       
        if (i == 4) { // P: SHOW PATH
            controlText.setFillColor(PATH_RED);
            controlText.setStyle(Text::Bold);
        } else if (i == 5) { // R: AUTO-SOLVE
            controlText.setFillColor(NEON_ORANGE);
            controlText.setStyle(Text::Bold);
        } else if (i == 6) { // T: RUN PATH
            controlText.setFillColor(ELECTRIC_YELLOW);
            controlText.setStyle(Text::Bold);
        } else {
            controlText.setFillColor(CYBER_WHITE);
        }
       
        controlText.setString(controls[i]);
        controlText.setPosition(window.getSize().x - 145, 290 + i * 12);
        window.draw(controlText);
    }


    // Línea de separación
    RectangleShape separator(Vector2f(120, 1));
    separator.setPosition(window.getSize().x - 150, 405);
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


// Grid principal con efectos profesionales y hexagonales - CORREGIDO
void drawGrid(RenderWindow& window, const HexGrid& grid,
    Player& player, CircleShape& hexagon,
    Text& text, Font& font, Clock& animClock, Clock& bgClock,
    std::vector<std::pair<int, int>>& pathCells)
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


    // PASO 1: Dibujar todas las celdas normales
    for (int y = 0; y < grid.rows(); ++y) {
        for (int x = 0; x < grid.cols(); ++x) {
            const HexCell& cell = grid.at(y, x);
            Vector2f pos = grid.toPixel(y, x);


            pos.x += offsetX;
            pos.y += offsetY;


            // Verificar si esta celda está en el path
            bool isInPath = false;
            for (const auto& pathCell : pathCells) {
                if (pathCell.first == y && pathCell.second == x) {
                    isInPath = true;
                    break;
                }
            }


            // Sombra (reducida si está en path)
            if (!isInPath) {
                CircleShape shadow = hexagon;
                shadow.setPosition(pos.x + 3, pos.y + 3);
                shadow.setFillColor(Color(0, 0, 0, 120));
                shadow.setOutlineThickness(0);
                window.draw(shadow);
            }


            // Hexágono principal
            hexagon.setPosition(pos);
            hexagon.setFillColor(getCellColor(cell.type, animClock));
            hexagon.setOutlineColor(Color(120, 140, 160));
            hexagon.setOutlineThickness(1);


            window.draw(hexagon);


            // Texto (solo si no está en path)
            if (!isInPath) {
                text.setString(CellTypeToString(cell.type));
                text.setCharacterSize(14);
                text.setStyle(Text::Bold);
                text.setFillColor(Color(30, 30, 50));
                text.setPosition(pos.x - 7.f, pos.y - 7.f);
                window.draw(text);
            }
        }
    }


    // PASO 2: DIBUJAR PATHFINDING CUANDO EXISTE
    if (!pathCells.empty()) {
        // Dibujar hexágonos del camino
        for (size_t i = 0; i < pathCells.size(); ++i) {
            const std::pair<int, int>& pathCell = pathCells[i];
            Vector2f pos = grid.toPixel(pathCell.first, pathCell.second);
            pos.x += offsetX;
            pos.y += offsetY;
           
            // Sombra del path
            CircleShape pathShadow(32, 6);
            pathShadow.setOrigin(32, 32);
            pathShadow.setPosition(pos.x + 4, pos.y + 4);
            pathShadow.setFillColor(Color(255, 0, 0, 80));
            pathShadow.setOutlineThickness(0);
            window.draw(pathShadow);
           
            // Hexágono exterior rojo
            CircleShape pathHexOuter(32, 6);
            pathHexOuter.setOrigin(32, 32);
            pathHexOuter.setPosition(pos);
            pathHexOuter.setFillColor(Color::Transparent);
            pathHexOuter.setOutlineColor(Color(255, 0, 0, 255));
            pathHexOuter.setOutlineThickness(8);
            window.draw(pathHexOuter);
           
            // Hexágono intermedio
            CircleShape pathHexMid(28, 6);
            pathHexMid.setOrigin(28, 28);
            pathHexMid.setPosition(pos);
            pathHexMid.setFillColor(Color(255, 50, 50, 120));
            pathHexMid.setOutlineColor(Color(255, 255, 255, 220));
            pathHexMid.setOutlineThickness(3);
            window.draw(pathHexMid);
           
            // Hexágono interior
            CircleShape pathHexInner(24, 6);
            pathHexInner.setOrigin(24, 24);
            pathHexInner.setPosition(pos);
            pathHexInner.setFillColor(Color(255, 255, 255, 100));
            pathHexInner.setOutlineColor(Color(200, 200, 200));
            pathHexInner.setOutlineThickness(1);
            window.draw(pathHexInner);
        }
       
        // Dibujar números de secuencia
        for (size_t i = 0; i < pathCells.size(); ++i) {
            const std::pair<int, int>& pathCell = pathCells[i];
            Vector2f pos = grid.toPixel(pathCell.first, pathCell.second);
            pos.x += offsetX;
            pos.y += offsetY;
           
            // Círculo de fondo para el número
            CircleShape numberBg(18);
            numberBg.setOrigin(18, 18);
            numberBg.setPosition(pos);
            numberBg.setFillColor(Color(0, 0, 0, 240));
            numberBg.setOutlineColor(Color(255, 255, 255));
            numberBg.setOutlineThickness(4);
            window.draw(numberBg);
           
            // Número de secuencia
            Text seqNumber;
            seqNumber.setFont(font);
            seqNumber.setCharacterSize(20);
            seqNumber.setStyle(Text::Bold);
            seqNumber.setFillColor(Color::White);
            seqNumber.setString(std::to_string(static_cast<int>(i + 1)));
           
            FloatRect bounds = seqNumber.getLocalBounds();
            seqNumber.setOrigin(bounds.width / 2, bounds.height / 2);
            seqNumber.setPosition(pos);
            window.draw(seqNumber);
        }
    }


    // PASO 3: Dibujar jugador (siempre por encima)
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


    // Color del jugador
    Color playerColor;
    if (player.isMoving) {
        float trail = sin(time * 20.0f) * 0.4f + 0.6f;
        playerColor = Color(
            static_cast<Uint8>(255 * trail),
            static_cast<Uint8>(150 * trail),
            static_cast<Uint8>(150 * trail)
        );
    } else {
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


    // Núcleo del jugador
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


