#include "IntroScreen.hpp"
#include <vector>
#include <cmath>

using namespace sf;

void mostrarIntro(RenderWindow& window, Font& font) {
    static Clock introClock;
    float time = introClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    
    for (int i = 0; i < window.getSize().y; i += 3) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        float wave = sin(time * 1.5f + gradient * 8.0f) * 0.3f + 0.7f;
        
        Color topColor(10, 20, 50);
        Color midColor(20, 40, 80);
        Color bottomColor(30, 50, 100);
        
        Color currentColor;
        if (gradient < 0.5f) {
            float t = gradient * 2.0f * wave;
            currentColor = Color(
                static_cast<Uint8>(topColor.r + (midColor.r - topColor.r) * t),
                static_cast<Uint8>(topColor.g + (midColor.g - topColor.g) * t),
                static_cast<Uint8>(topColor.b + (midColor.b - topColor.b) * t)
            );
        } else {
            float t = (gradient - 0.5f) * 2.0f * wave;
            currentColor = Color(
                static_cast<Uint8>(midColor.r + (bottomColor.r - midColor.r) * t),
                static_cast<Uint8>(midColor.g + (bottomColor.g - midColor.g) * t),
                static_cast<Uint8>(midColor.b + (bottomColor.b - midColor.b) * t)
            );
        }
        
        RectangleShape line(Vector2f(windowWidth, 3));
        line.setPosition(0, static_cast<float>(i));
        line.setFillColor(currentColor);
        window.draw(line);
    }
    
    for (int i = 0; i < 30; ++i) {
        float x = fmod(time * 25.0f + i * 45.0f, windowWidth + 80.0f) - 40.0f;
        float y = 60.0f + sin(time * 0.8f + i * 0.4f) * 25.0f + i * 18.0f;
        
        if (y < windowHeight) {
            CircleShape hex(3 + (i % 4), 6);
            hex.setOrigin(3 + (i % 4), 3 + (i % 4));
            hex.setPosition(x, y);
            hex.setFillColor(Color::Transparent);
            
            Color hexColor;
            if (i % 4 == 0) hexColor = Color(0, 200, 255, 60);
            else if (i % 4 == 1) hexColor = Color(0, 255, 150, 60);
            else if (i % 4 == 2) hexColor = Color(180, 100, 255, 60);
            else hexColor = Color(255, 150, 0, 60);
            
            hex.setOutlineColor(hexColor);
            hex.setOutlineThickness(1);
            hex.rotate(time * 30.0f * (i + 1));
            window.draw(hex);
        }
    }
    
    for (int i = 0; i < 3; ++i) {
        CircleShape titleHex(80 + i * 15, 6);
        titleHex.setOrigin(80 + i * 15, 80 + i * 15);
        titleHex.setPosition(centerX, centerY - 200);
        titleHex.setFillColor(Color::Transparent);
        
        Color hexColor;
        if (i == 0) hexColor = Color(255, 255, 0, 120);
        else if (i == 1) hexColor = Color(0, 200, 255, 100);
        else hexColor = Color(180, 100, 255, 80);
        
        titleHex.setOutlineColor(hexColor);
        titleHex.setOutlineThickness(2);
        titleHex.rotate(time * 12.0f * (i + 1));
        window.draw(titleHex);
    }
    
    Text titulo;
    titulo.setFont(font);
    titulo.setCharacterSize(56);
    titulo.setStyle(Text::Bold);
    titulo.setString("HexEscape");
    
    float titlePulse = sin(time * 4.0f) * 0.4f + 0.6f;
    titulo.setFillColor(Color(
        static_cast<Uint8>(255 * titlePulse),
        static_cast<Uint8>(255 * titlePulse),
        0
    ));
    
    FloatRect titleBounds = titulo.getLocalBounds();
    titulo.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    titulo.setPosition(centerX, centerY - 200);
    window.draw(titulo);
    
    Text subtitulo;
    subtitulo.setFont(font);
    subtitulo.setCharacterSize(20);
    subtitulo.setStyle(Text::Bold);
    subtitulo.setString("FABRICA DE ROMPECABEZAS ELITE");
    subtitulo.setFillColor(Color(0, 200, 255));
    
    FloatRect subtitleBounds = subtitulo.getLocalBounds();
    subtitulo.setOrigin(subtitleBounds.width / 2, subtitleBounds.height / 2);
    subtitulo.setPosition(centerX, centerY - 150);
    window.draw(subtitulo);
    
    RectangleShape line1(Vector2f(350, 2));
    line1.setOrigin(175, 1);
    line1.setPosition(centerX, centerY - 125);
    line1.setFillColor(Color(0, 255, 150));
    window.draw(line1);
    
    RectangleShape contextPanel(Vector2f(700, 120));
    contextPanel.setOrigin(350, 60);
    contextPanel.setPosition(centerX, centerY - 40);
    contextPanel.setFillColor(Color(20, 40, 80, 180));
    contextPanel.setOutlineColor(Color(0, 255, 150));
    contextPanel.setOutlineThickness(2);
    window.draw(contextPanel);
    
    Text contexto1;
    contexto1.setFont(font);
    contexto1.setCharacterSize(16);
    contexto1.setString("Eres un ingeniero atrapado en una fabrica automatizada futurista.");
    contexto1.setFillColor(Color::White);
    FloatRect context1Bounds = contexto1.getLocalBounds();
    contexto1.setOrigin(context1Bounds.width / 2, context1Bounds.height / 2);
    contexto1.setPosition(centerX, centerY - 70);
    window.draw(contexto1);
    
    Text contexto2;
    contexto2.setFont(font);
    contexto2.setCharacterSize(16);
    contexto2.setString("Navega por bandas transportadoras, evita paredes dinamicas");
    contexto2.setFillColor(Color::White);
    FloatRect context2Bounds = contexto2.getLocalBounds();
    contexto2.setOrigin(context2Bounds.width / 2, context2Bounds.height / 2);
    contexto2.setPosition(centerX, centerY - 45);
    window.draw(contexto2);
    
    Text contexto3;
    contexto3.setFont(font);
    contexto3.setCharacterSize(16);
    contexto3.setString("y usa tu energia acumulada para destruir obstaculos. ¡Escapa!");
    contexto3.setFillColor(Color::White);
    FloatRect context3Bounds = contexto3.getLocalBounds();
    contexto3.setOrigin(context3Bounds.width / 2, context3Bounds.height / 2);
    contexto3.setPosition(centerX, centerY - 20);
    window.draw(contexto3);
    
    RectangleShape leftPanel(Vector2f(320, 180));
    leftPanel.setPosition(centerX - 380, centerY + 40);
    leftPanel.setFillColor(Color(20, 40, 80, 180));
    leftPanel.setOutlineColor(Color(180, 100, 255));
    leftPanel.setOutlineThickness(2);
    window.draw(leftPanel);
    
    Text movementTitle;
    movementTitle.setFont(font);
    movementTitle.setCharacterSize(18);
    movementTitle.setStyle(Text::Bold);
    movementTitle.setString("MOVIMIENTO HEXAGONAL");
    movementTitle.setFillColor(Color(255, 255, 0));
    movementTitle.setPosition(centerX - 370, centerY + 50);
    window.draw(movementTitle);
    
    std::vector<std::string> movementControls = {
        "W / E  -  Arriba Diagonal",
        "A / D  -  Izquierda / Derecha", 
        "Z / X  -  Abajo Diagonal",
        "",
        "SPACE  -  Romper Pared",
        "         (Energia Llena)"
    };
    
    for (size_t i = 0; i < movementControls.size(); ++i) {
        Text controlText;
        controlText.setFont(font);
        controlText.setCharacterSize(13);
        controlText.setFillColor(i == 4 ? Color(255, 100, 255) : Color(200, 200, 255));
        if (i == 4) controlText.setStyle(Text::Bold);
        controlText.setString(movementControls[i]);
        controlText.setPosition(centerX - 370, centerY + 75 + i * 18);
        window.draw(controlText);
    }
    
    RectangleShape rightPanel(Vector2f(320, 180));
    rightPanel.setPosition(centerX + 60, centerY + 40);
    rightPanel.setFillColor(Color(20, 40, 80, 180));
    rightPanel.setOutlineColor(Color(0, 200, 255));
    rightPanel.setOutlineThickness(2);
    window.draw(rightPanel);
    
    Text modesTitle;
    modesTitle.setFont(font);
    modesTitle.setCharacterSize(18);
    modesTitle.setStyle(Text::Bold);
    modesTitle.setString("MODOS DE JUEGO");
    modesTitle.setFillColor(Color(255, 255, 0));
    modesTitle.setPosition(centerX + 70, centerY + 50);
    window.draw(modesTitle);
    
    std::vector<std::pair<std::string, Color>> gameModes = {
        {"P  -  Mostrar Camino Optimo", Color(255, 50, 50)},
        {"R  -  Auto-Resolver Directo", Color(255, 150, 0)},
        {"T  -  Ejecutar Camino", Color(255, 255, 0)},
        {"", Color::White},
        {"ESC  -  Cancelar / Salir", Color(200, 200, 200)}
    };
    
    for (size_t i = 0; i < gameModes.size(); ++i) {
        Text modeText;
        modeText.setFont(font);
        modeText.setCharacterSize(13);
        modeText.setFillColor(gameModes[i].second);
        if (i < 3) modeText.setStyle(Text::Bold);
        modeText.setString(gameModes[i].first);
        modeText.setPosition(centerX + 70, centerY + 75 + i * 18);
        window.draw(modeText);
    }
    
    RectangleShape featuresPanel(Vector2f(760, 60));
    featuresPanel.setOrigin(380, 30);
    featuresPanel.setPosition(centerX, centerY + 270);
    featuresPanel.setFillColor(Color(20, 40, 80, 180));
    featuresPanel.setOutlineColor(Color(0, 255, 150));
    featuresPanel.setOutlineThickness(2);
    window.draw(featuresPanel);
    
    Text featuresTitle;
    featuresTitle.setFont(font);
    featuresTitle.setCharacterSize(16);
    featuresTitle.setStyle(Text::Bold);
    featuresTitle.setString("CARACTERISTICAS ESPECIALES");
    featuresTitle.setFillColor(Color(0, 255, 150));
    FloatRect featuresBounds = featuresTitle.getLocalBounds();
    featuresTitle.setOrigin(featuresBounds.width / 2, featuresBounds.height / 2);
    featuresTitle.setPosition(centerX, centerY + 250);
    window.draw(featuresTitle);
    
    Text features;
    features.setFont(font);
    features.setCharacterSize(12);
    features.setString("• Bandas Transportadoras Automaticas  • Sistema de Energia Dinamico  • Paredes Cada 5 Turnos");
    features.setFillColor(Color(180, 180, 255));
    FloatRect featuresBounds2 = features.getLocalBounds();
    features.setOrigin(featuresBounds2.width / 2, featuresBounds2.height / 2);
    features.setPosition(centerX, centerY + 270);
    window.draw(features);
    
    Text features2;
    features2.setFont(font);
    features2.setCharacterSize(12);
    features2.setString("• Pathfinding Inteligente A*  • Grid Hexagonal Futurista  • Multiples Modos de Solucion");
    features2.setFillColor(Color(180, 180, 255));
    FloatRect features2Bounds = features2.getLocalBounds();
    features2.setOrigin(features2Bounds.width / 2, features2Bounds.height / 2);
    features2.setPosition(centerX, centerY + 290);
    window.draw(features2);
    
    float startPulse = sin(time * 6.0f) * 0.5f + 0.5f;
    
    RectangleShape startButton(Vector2f(500, 50));
    startButton.setOrigin(250, 25);
    startButton.setPosition(centerX, centerY + 350);
    startButton.setFillColor(Color(30, 60, 120, static_cast<Uint8>(100 + 100 * startPulse)));
    startButton.setOutlineColor(Color(
        static_cast<Uint8>(255 * startPulse),
        static_cast<Uint8>(255 * startPulse),
        static_cast<Uint8>(100 + 155 * startPulse)
    ));
    startButton.setOutlineThickness(3);
    window.draw(startButton);
    
    Text startText;
    startText.setFont(font);
    startText.setCharacterSize(28);
    startText.setStyle(Text::Bold);
    startText.setString("PRESIONA  ESPACIO  PARA  CONTINUAR");
    startText.setFillColor(Color(
        static_cast<Uint8>(255 * startPulse),
        static_cast<Uint8>(255 * startPulse),
        static_cast<Uint8>(100 + 155 * startPulse)
    ));
    
    FloatRect startBounds = startText.getLocalBounds();
    startText.setOrigin(startBounds.width / 2, startBounds.height / 2);
    startText.setPosition(centerX, centerY + 350);
    window.draw(startText);
    
    for (int i = 0; i < 8; ++i) {
        float angle = (i / 8.0f) * 2 * 3.14159f;
        float x = centerX + cos(angle + time * 2.0f) * 120;
        float y = centerY + 350 + sin(angle + time * 2.0f) * 15;
        
        CircleShape hexDeco(6, 6);
        hexDeco.setOrigin(6, 6);
        hexDeco.setPosition(x, y);
        hexDeco.setFillColor(Color::Transparent);
        hexDeco.setOutlineColor(Color(
            static_cast<Uint8>(100 + 155 * startPulse),
            static_cast<Uint8>(150 + 105 * startPulse),
            255,
            static_cast<Uint8>(150 * startPulse)
        ));
        hexDeco.setOutlineThickness(2);
        hexDeco.rotate(time * 90.0f);
        window.draw(hexDeco);
    }
    
    RectangleShape bottomLine1(Vector2f(200, 2));
    bottomLine1.setOrigin(100, 1);
    bottomLine1.setPosition(centerX - 150, centerY + 400);
    bottomLine1.setFillColor(Color(0, 200, 255, static_cast<Uint8>(150 * startPulse)));
    window.draw(bottomLine1);
    
    RectangleShape bottomLine2(Vector2f(200, 2));
    bottomLine2.setOrigin(100, 1);
    bottomLine2.setPosition(centerX + 150, centerY + 400);
    bottomLine2.setFillColor(Color(0, 200, 255, static_cast<Uint8>(150 * startPulse)));
    window.draw(bottomLine2);
}