#include "UIRenderer.hpp"
#include "EffectsRenderer.hpp"
#include <vector>
#include <string>

using namespace model;
using namespace sf;

const Color NEON_BLUE = Color(0, 200, 255);
const Color NEON_GREEN = Color(0, 255, 150);
const Color NEON_PURPLE = Color(180, 100, 255);
const Color NEON_ORANGE = Color(255, 150, 0);
const Color ELECTRIC_YELLOW = Color(255, 255, 0);
const Color CYBER_WHITE = Color(240, 240, 255);
const Color TECH_GRAY = Color(70, 90, 120);
const Color PATH_RED = Color(255, 50, 50);

void drawModernEnergyBar(RenderWindow& window, const Player& player, Font& font, Clock& animClock) {
    const float barWidth = 280.0f;
    const float barHeight = 22.0f;
    const float barX = 25.0f;
    const float barY = window.getSize().y - 90.0f;

    float time = animClock.getElapsedTime().asSeconds();

    RectangleShape energyFrame(Vector2f(barWidth, barHeight));
    energyFrame.setPosition(barX, barY);
    energyFrame.setFillColor(Color(20, 40, 80, 180));
    energyFrame.setOutlineColor(TECH_GRAY);
    energyFrame.setOutlineThickness(2);
    window.draw(energyFrame);

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

    Text energyText;
    energyText.setFont(font);
    energyText.setCharacterSize(14); 
    energyText.setFillColor(CYBER_WHITE);
    energyText.setStyle(Text::Bold);
    energyText.setString("ENERGIA: " + std::to_string(player.energy) + "/" + std::to_string(Player::MAX_ENERGY));
    energyText.setPosition(barX, barY - 24); // Ajustado por el tamaño mayor
    window.draw(energyText);

    if (player.canUseWallBreak()) {
        float pulse = sin(time * 8.0f) * 0.5f + 0.5f;

        Text abilityText;
        abilityText.setFont(font);
        abilityText.setCharacterSize(15); 
        abilityText.setFillColor(Color(
            static_cast<Uint8>(ELECTRIC_YELLOW.r * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.g * pulse),
            static_cast<Uint8>(ELECTRIC_YELLOW.b)
        ));
        abilityText.setStyle(Text::Bold);
        abilityText.setString("ESPACIO: DESTRUIR PARED");
        abilityText.setPosition(barX, barY - 46); 
        window.draw(abilityText);
    }
    else if (player.isSelectingWall) {
        Text selectText;
        selectText.setFont(font);
        selectText.setCharacterSize(14); 
        selectText.setFillColor(NEON_ORANGE);
        selectText.setString("SELECCIONA DIRECCION - ESC: CANCELAR");
        selectText.setPosition(barX, barY - 46);
        window.draw(selectText);
    }
    else {
        Text waitText;
        waitText.setFont(font);
        waitText.setCharacterSize(13); 
        waitText.setFillColor(Color(150, 180, 200));
        waitText.setString("ACUMULA ENERGIA PARA HABILIDADES");
        waitText.setPosition(barX, barY - 46);
        window.draw(waitText);
    }
}

void drawGameInfo(RenderWindow& window, Font& font, int turnCount, Clock& animClock,
                 bool showPathVisualization, bool autoSolveMode) {
    float time = animClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    
    float panelX = windowWidth - 180;
    float panelY = 80;
    
    RectangleShape panel(Vector2f(160, 180));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(Color(20, 40, 80, 200));
    panel.setOutlineColor(NEON_BLUE);
    panel.setOutlineThickness(2);
    window.draw(panel);

    Vector2f corners[] = {
        Vector2f(panelX + 5, panelY + 5),
        Vector2f(panelX + 155, panelY + 5),
        Vector2f(panelX + 5, panelY + 175),
        Vector2f(panelX + 155, panelY + 175)
    };
   
    for (int i = 0; i < 4; ++i) {
        Color hexColor = (i % 2 == 0) ? NEON_GREEN : NEON_PURPLE;
        hexColor.a = 120;
        drawDecorativeHex(window, corners[i], 6, hexColor, time * 60.0f);
    }

    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(14); 
    titleText.setFillColor(CYBER_WHITE);
    titleText.setStyle(Text::Bold);
    titleText.setString("ESTADO");
    titleText.setPosition(panelX + 15, panelY + 10);
    window.draw(titleText);

    Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(12); 
    turnText.setFillColor(CYBER_WHITE);
    turnText.setString("TURNOS: " + std::to_string(turnCount));
    turnText.setPosition(panelX + 15, panelY + 30);
    window.draw(turnText);

    int turnsUntilWall = 5 - (turnCount % 5);
    Text eventText;
    eventText.setFont(font);
    eventText.setCharacterSize(12); 
    eventText.setFillColor(CYBER_WHITE);
    eventText.setString("PARED: " + std::to_string(turnsUntilWall == 5 ? 0 : turnsUntilWall));
    eventText.setPosition(panelX + 15, panelY + 45);
    window.draw(eventText);

    Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(11); 
    timeText.setFillColor(CYBER_WHITE);
    timeText.setString("TIEMPO: " + std::to_string(static_cast<int>(time)) + "S");
    timeText.setPosition(panelX + 15, panelY + 60);
    window.draw(timeText);

    Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(12); 
    modeText.setStyle(Text::Bold);
    modeText.setPosition(panelX + 15, panelY + 80);

    if (autoSolveMode && showPathVisualization) {
        // Modo: Ejecutando camino mostrado
        modeText.setFillColor(ELECTRIC_YELLOW);
        modeText.setString("MODO: EJECUTANDO");
    } else if (autoSolveMode && !showPathVisualization) {
        // Modo: Auto-resolución directa
        modeText.setFillColor(NEON_ORANGE);
        modeText.setString("MODO: AUTO");
    } else if (showPathVisualization && !autoSolveMode) {
        // Modo: Solo mostrando camino
        modeText.setFillColor(PATH_RED);
        modeText.setString("MODO: VISTA PREVIA");
    } else {
        // Modo manual
        modeText.setFillColor(NEON_GREEN);
        modeText.setString("MODO: MANUAL");
    }
    window.draw(modeText);

    Text instructionText;
    instructionText.setFont(font);
    instructionText.setCharacterSize(10); // Era 9, ahora 10
    instructionText.setPosition(panelX + 15, panelY + 100);

    if (autoSolveMode && showPathVisualization) {
        instructionText.setFillColor(ELECTRIC_YELLOW);
        instructionText.setString("EJECUTANDO RUTA...");
    } else if (autoSolveMode && !showPathVisualization) {
        instructionText.setFillColor(NEON_ORANGE);
        instructionText.setString("RESOLVIENDO AUTO...");
    } else if (showPathVisualization && !autoSolveMode) {
        instructionText.setFillColor(ELECTRIC_YELLOW);
        instructionText.setString("T: EJECUTAR | ESC: CANCELAR");
    } else {
        instructionText.setFillColor(CYBER_WHITE);
        instructionText.setString("P: VISTA PREVIA | R: AUTO");
    }
    window.draw(instructionText);

    Text optionsText;
    optionsText.setFont(font);
    optionsText.setCharacterSize(9); 
    optionsText.setFillColor(Color(150, 180, 200));
    optionsText.setPosition(panelX + 15, panelY + 120);
   
    if (autoSolveMode) {
        optionsText.setString("ESC: PARAR");
    } else if (showPathVisualization) {
        optionsText.setString("T: EJECUTAR | R: DIRECTO");
    } else {
        optionsText.setString("P: MOSTRAR | R: RESOLVER");
    }
    window.draw(optionsText);
}

void drawModernControls(RenderWindow& window, Font& font, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    
    float panelX = windowWidth - 180;
    float panelY = 280;

    RectangleShape controlPanel(Vector2f(160, 180));
    controlPanel.setPosition(panelX, panelY);
    controlPanel.setFillColor(Color(20, 40, 80, 200));
    controlPanel.setOutlineColor(NEON_PURPLE);
    controlPanel.setOutlineThickness(2);
    window.draw(controlPanel);

    for (int i = 0; i < 4; ++i) {
        float hexX = panelX + 15 + (i % 2) * 30;
        float hexY = panelY + 15 + (i / 2) * 60;
       
        Color hexColor = (i % 2 == 0) ? NEON_BLUE : NEON_GREEN;
        hexColor.a = 80;
        drawDecorativeHex(window, Vector2f(hexX, hexY), 5, hexColor, time * 45.0f + i * 60);
    }

    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(14); 
    titleText.setFillColor(CYBER_WHITE);
    titleText.setStyle(Text::Bold);
    titleText.setString("CONTROLES");
    titleText.setPosition(panelX + 15, panelY + 10);
    window.draw(titleText);

    std::vector<std::string> controls;
    controls.push_back("W/E: ARRIBA");
    controls.push_back("A/D: LADOS");
    controls.push_back("Z/X: ABAJO");
    controls.push_back("ESPACIO: PODER");
    controls.push_back("P: MOSTRAR RUTA");
    controls.push_back("T: EJECUTAR RUTA");  
    controls.push_back("R: AUTO-RESOLVER");      
    controls.push_back("ESC: CANCELAR");

    for (size_t i = 0; i < controls.size(); ++i) {
        Text controlText;
        controlText.setFont(font);
        controlText.setCharacterSize(10); 
       
        if (i == 4) { 
            controlText.setFillColor(PATH_RED);
            controlText.setStyle(Text::Bold);
        } else if (i == 5) { 
            controlText.setFillColor(NEON_ORANGE);
            controlText.setStyle(Text::Bold);
        } else if (i == 6) { 
            controlText.setFillColor(ELECTRIC_YELLOW);
            controlText.setStyle(Text::Bold);
        } else {
            controlText.setFillColor(CYBER_WHITE);
        }
       
        controlText.setString(controls[i]);
        controlText.setPosition(panelX + 15, panelY + 30 + i * 12);
        window.draw(controlText);
    }

    RectangleShape separator(Vector2f(140, 1));
    separator.setPosition(panelX + 10, panelY + 165);
    separator.setFillColor(Color(NEON_PURPLE.r, NEON_PURPLE.g, NEON_PURPLE.b, 150));
    window.draw(separator);
}