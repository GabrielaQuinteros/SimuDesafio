#include "ScreenRenderer.hpp"
#include "EffectsRenderer.hpp"
#include <cmath>
#include <vector>
#include <string>

using namespace sf;

const Color NEON_BLUE = Color(0, 200, 255);
const Color NEON_GREEN = Color(0, 255, 150);
const Color NEON_PURPLE = Color(180, 100, 255);
const Color NEON_ORANGE = Color(255, 150, 0);
const Color ELECTRIC_YELLOW = Color(255, 255, 0);
const Color CYBER_WHITE = Color(240, 240, 255);
const Color DARK_BLUE = Color(20, 40, 80);
const Color PATH_RED = Color(255, 50, 50);

void drawVictoryScreen(RenderWindow& window, Font& font, float winTime, int turnCount, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();
   
    for (int i = 0; i < window.getSize().y; i += 4) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        Color currentColor = lerpColor(DARK_BLUE, Color(40, 20, 80), gradient);
       
        RectangleShape line(Vector2f(window.getSize().x, 4));
        line.setPosition(0, i);
        line.setFillColor(currentColor);
        window.draw(line);
    }

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

    RectangleShape centerPanel(Vector2f(400, 250));
    centerPanel.setOrigin(200, 125);
    centerPanel.setPosition(centerX, centerY);
    centerPanel.setFillColor(Color(20, 40, 80, 200));
    centerPanel.setOutlineColor(NEON_BLUE);
    centerPanel.setOutlineThickness(2);
    window.draw(centerPanel);

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

void drawGameTitle(RenderWindow& window, Font& font, Clock& animClock) {
    float time = animClock.getElapsedTime().asSeconds();

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

    Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setCharacterSize(12);
    subtitleText.setString("FABRICA DE ROMPECABEZAS");

    FloatRect subtitleBounds = subtitleText.getLocalBounds();
    float subtitleX = (window.getSize().x - subtitleBounds.width) / 2.0f;
    float subtitleY = titleY + 40.0f;

    RectangleShape leftLine(Vector2f(80, 2));
    leftLine.setPosition(subtitleX - 90, subtitleY + 8);
    leftLine.setFillColor(NEON_BLUE);
    window.draw(leftLine);

    subtitleText.setPosition(subtitleX, subtitleY);
    subtitleText.setFillColor(CYBER_WHITE);
    window.draw(subtitleText);

    RectangleShape rightLine(Vector2f(80, 2));
    rightLine.setPosition(subtitleX + subtitleBounds.width + 10, subtitleY + 8);
    rightLine.setFillColor(NEON_BLUE);
    window.draw(rightLine);
}

void drawAnimatedBackground(RenderWindow& window, Clock& bgClock) {
    float time = bgClock.getElapsedTime().asSeconds();

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

