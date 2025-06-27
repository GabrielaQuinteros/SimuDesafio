#include "MapSelector.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cmath>

using namespace sf;
namespace fs = std::filesystem;

std::vector<MapInfo> availableMaps;
int selectedMapIndex = 0;
std::string mappath = "Tambien puedes copiar la ruta del mapa que tengas, por ejemplo: C:\\Docs\\mymap.txt. Luego, regresa a la terminal, presiona la tecla P y se abrira tu mapa automaticamente.";

bool isValidMapFile(const std::string& filepath) {
    try {
        std::cout << "Verificando mapa: " << filepath << std::endl;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cout << "  - No se pudo abrir el archivo" << std::endl;
            return false;
        }
        
        std::string line;
        bool hasStart = false;
        bool hasGoal = false;
        int lineCount = 0;
        int validGameLines = 0;
        
        while (std::getline(file, line) && lineCount < 100) {
            std::string trimmedLine = line;
            trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t\r\n"));
            trimmedLine.erase(trimmedLine.find_last_not_of(" \t\r\n") + 1);
            
            if (!trimmedLine.empty()) {
                bool hasGameChars = false;
                for (char c : trimmedLine) {
                    if (c == 'S' || c == 'G' || c == '.' || c == '#' || 
                        c == 'A' || c == 'B' || c == 'C' || c == 'D' || 
                        c == 'E' || c == 'F' || c == 'K' || c == 'X' || c == 'Z') {
                        hasGameChars = true;
                        break;
                    }
                }
                
                if (hasGameChars) {
                    validGameLines++;
                    if (trimmedLine.find('S') != std::string::npos) hasStart = true;
                    if (trimmedLine.find('G') != std::string::npos) hasGoal = true;
                }
            }
            lineCount++;
        }
        
        file.close();
        
        bool isValid = hasStart && hasGoal && validGameLines >= 10;
        
        std::cout << "  - Lineas totales: " << lineCount << std::endl;
        std::cout << "  - Lineas de juego validas: " << validGameLines << std::endl;
        std::cout << "  - Tiene Start (S): " << (hasStart ? "Si" : "No") << std::endl;
        std::cout << "  - Tiene Goal (G): " << (hasGoal ? "Si" : "No") << std::endl;
        std::cout << "  - Es valido: " << (isValid ? "Si" : "No") << std::endl;
        
        return isValid;
        
    } catch (const std::exception& e) {
        std::cout << "  - Excepcion: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cout << "  - Error desconocido" << std::endl;
        return false;
    }
}

void scanMapFiles() {
    availableMaps.clear();
    
    std::cout << "=== ESCANEANDO MAPAS ===" << std::endl;
    
    try {
        if (fs::exists("resources") && fs::is_directory("resources")) {
            std::cout << "Carpeta resources encontrada, buscando mapas..." << std::endl;
            
            std::cout << "\n--- LISTANDO TODOS LOS ARCHIVOS EN RESOURCES ---" << std::endl;
            for (const auto& entry : fs::directory_iterator("resources")) {
                std::cout << "Archivo encontrado: " << entry.path().string() 
                          << " (Ext: " << entry.path().extension().string() << ")" << std::endl;
            }
            std::cout << "--- FIN DE LISTADO ---\n" << std::endl;
            
            for (const auto& entry : fs::directory_iterator("resources")) {
                std::string fullPath = entry.path().string();
                std::string extension = entry.path().extension().string();
                
                std::cout << "Procesando entrada: " << fullPath << " con extension: '" << extension << "'" << std::endl;
                
                if (entry.is_regular_file() && extension == ".txt") {
                    std::string filename = entry.path().string();
                    std::string basename = entry.path().stem().string();
                    
                    std::replace(filename.begin(), filename.end(), '\\', '/');
                    
                    std::cout << "\n--- Procesando archivo .txt: " << filename << " ---" << std::endl;
                    std::cout << "Basename: " << basename << std::endl;
                    
                    std::string lowerBasename = basename;
                    std::transform(lowerBasename.begin(), lowerBasename.end(), lowerBasename.begin(), ::tolower);
                    
                    if (lowerBasename == "license" || lowerBasename == "readme" || 
                        lowerBasename == "changelog" || lowerBasename == "credits" ||
                        lowerBasename == "arial") {
                        std::cout << "Archivo excluido (no es mapa): " << basename << std::endl;
                        continue;
                    }
                    
                    std::cout << "Procesando como posible mapa..." << std::endl;
                    
                    bool isValid = isValidMapFile(filename);
                    
                    std::string displayName = basename;
                    if (!displayName.empty()) {
                        displayName[0] = std::toupper(displayName[0]);
                        std::replace(displayName.begin(), displayName.end(), '_', ' ');
                    }
                    
                    bool isDefault = (basename == "map");
                    
                    availableMaps.push_back({filename, displayName, isDefault, isValid});
                    std::cout << ">>> MAPA AGREGADO: " << displayName << " - " << filename 
                              << " (Valido: " << (isValid ? "Si" : "No") 
                              << ", Defecto: " << (isDefault ? "Si" : "No") << ")" << std::endl;
                } else {
                    std::cout << "Archivo ignorado (no es .txt o no es archivo regular): " << fullPath << std::endl;
                }
            }
        } else {
            std::cout << "Carpeta resources no encontrada o no es un directorio." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error escaneando archivos: " << e.what() << std::endl;
    }
    
    if (availableMaps.empty()) {
        std::cout << "No se encontraron mapas, intentando agregar mapa por defecto..." << std::endl;
        if (fs::exists("resources/map.txt")) {
            bool isValid = isValidMapFile("resources/map.txt");
            availableMaps.push_back({"resources/map.txt", "Mapa Original (Defecto)", true, isValid});
            std::cout << "Mapa por defecto agregado: resources/map.txt (Valido: " << (isValid ? "Si" : "No") << ")" << std::endl;
        }
    }
    
    std::sort(availableMaps.begin(), availableMaps.end(), 
              [](const MapInfo& a, const MapInfo& b) {
                  if (a.isDefault != b.isDefault) {
                      return a.isDefault > b.isDefault;
                  }
                  if (a.isValid != b.isValid) {
                      return a.isValid > b.isValid;
                  }
                  
                  std::string lowerA = a.displayName;
                  std::string lowerB = b.displayName;
                  std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
                  std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
                  
                  auto getDifficultyOrder = [](const std::string& name) -> int {
                      if (name.find("facil") != std::string::npos) return 1;
                      if (name.find("medio") != std::string::npos) return 2;
                      if (name.find("dificil") != std::string::npos) return 3;
                      return 4;
                  };
                  
                  int orderA = getDifficultyOrder(lowerA);
                  int orderB = getDifficultyOrder(lowerB);
                  
                  if (orderA != orderB) {
                      return orderA < orderB;
                  }
                  
                  return a.displayName < b.displayName;
              });
    
    selectedMapIndex = 0;
    for (size_t i = 0; i < availableMaps.size(); ++i) {
        if (availableMaps[i].isValid) {
            selectedMapIndex = static_cast<int>(i);
            break;
        }
    }
    
    std::cout << "\n=== RESUMEN DE ESCANEO ===" << std::endl;
    std::cout << "Total de mapas encontrados: " << availableMaps.size() << std::endl;
    
    int validCount = 0;
    for (const auto& map : availableMaps) {
        if (map.isValid) validCount++;
        std::cout << "  - " << map.displayName << " (" << map.filename << ") - " 
                  << (map.isValid ? "VALIDO" : "INVALIDO") 
                  << (map.isDefault ? " [DEFECTO]" : "") << std::endl;
    }
    
    std::cout << "Mapas validos: " << validCount << std::endl;
    std::cout << "Indice seleccionado inicial: " << selectedMapIndex << std::endl;
    if (selectedMapIndex >= 0 && selectedMapIndex < static_cast<int>(availableMaps.size())) {
        std::cout << "Mapa seleccionado: " << availableMaps[selectedMapIndex].displayName << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

std::string getCurrentSelectedMap() {
    if (availableMaps.empty() || selectedMapIndex < 0 || selectedMapIndex >= static_cast<int>(availableMaps.size())) {
        return "resources/map.txt";
    }
    return availableMaps[selectedMapIndex].filename;
}

bool isCurrentSelectionValid() {
    if (availableMaps.empty() || selectedMapIndex < 0 || selectedMapIndex >= static_cast<int>(availableMaps.size())) {
        return false;
    }
    return availableMaps[selectedMapIndex].isValid;
}

// Helper function to create rounded rectangle
void drawRoundedRect(RenderWindow& window, float x, float y, float width, float height, 
                     float radius, Color fillColor, Color outlineColor = Color::Transparent, 
                     float outlineThickness = 0) {
    // Main rectangle
    RectangleShape mainRect(Vector2f(width - 2 * radius, height));
    mainRect.setPosition(x + radius, y);
    mainRect.setFillColor(fillColor);
    if (outlineThickness > 0) {
        mainRect.setOutlineColor(outlineColor);
        mainRect.setOutlineThickness(outlineThickness);
    }
    window.draw(mainRect);
    
    // Side rectangles
    RectangleShape leftRect(Vector2f(radius, height - 2 * radius));
    leftRect.setPosition(x, y + radius);
    leftRect.setFillColor(fillColor);
    window.draw(leftRect);
    
    RectangleShape rightRect(Vector2f(radius, height - 2 * radius));
    rightRect.setPosition(x + width - radius, y + radius);
    rightRect.setFillColor(fillColor);
    window.draw(rightRect);
    
    // Corner circles
    CircleShape corners[4];
    Vector2f cornerPositions[4] = {
        Vector2f(x + radius, y + radius),        // Top-left
        Vector2f(x + width - radius, y + radius), // Top-right
        Vector2f(x + radius, y + height - radius), // Bottom-left
        Vector2f(x + width - radius, y + height - radius) // Bottom-right
    };
    
    for (int i = 0; i < 4; i++) {
        corners[i].setRadius(radius);
        corners[i].setOrigin(radius, radius);
        corners[i].setPosition(cornerPositions[i]);
        corners[i].setFillColor(fillColor);
        window.draw(corners[i]);
    }
}

// Function to draw hexagon
void drawHexagon(RenderWindow& window, float x, float y, float radius, Color fillColor, float rotation = 0.0f) {
    CircleShape hexagon(radius, 6);
    hexagon.setOrigin(radius, radius);
    hexagon.setPosition(x, y);
    hexagon.setFillColor(fillColor);
    hexagon.setRotation(rotation);
    window.draw(hexagon);
}

void mostrarSelectorMapas(RenderWindow& window, Font& font) {
    static Clock selectorClock;
    float time = selectorClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    
    // Modern gradient background
    for (int i = 0; i < window.getSize().y; i += 2) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        Color topColor(8, 12, 24);      // Dark navy
        Color bottomColor(16, 24, 40);   // Slightly lighter navy
        
        float t = gradient;
        Color currentColor(
            static_cast<Uint8>(topColor.r + (bottomColor.r - topColor.r) * t),
            static_cast<Uint8>(topColor.g + (bottomColor.g - topColor.g) * t),
            static_cast<Uint8>(topColor.b + (bottomColor.b - topColor.b) * t)
        );
        
        RectangleShape line(Vector2f(windowWidth, 2));
        line.setPosition(0, static_cast<float>(i));
        line.setFillColor(currentColor);
        window.draw(line);
    }
    
    // Animated hexagonal background pattern
    for (int i = 0; i < 25; ++i) {
        float angle = time * 0.3f + i * 0.8f;
        float distance = 200.0f + sin(time * 0.5f + i * 0.4f) * 100.0f;
        float x = centerX + cos(angle) * distance;
        float y = centerY + sin(angle) * distance;
        
        if (x > -50 && x < windowWidth + 50 && y > -50 && y < windowHeight + 50) {
            float hexSize = 12 + (i % 4) * 3;
            float alpha = 15 + sin(time * 2.0f + i * 0.6f) * 10;
            drawHexagon(window, x, y, hexSize, Color(64, 120, 255, static_cast<Uint8>(alpha)), time * 15.0f + i * 20.0f);
        }
    }
    
    // Additional hexagon decorations around the interface
    for (int i = 0; i < 8; ++i) {
        float x = 100 + i * 140 + sin(time * 0.4f + i) * 30;
        float y = 80 + cos(time * 0.3f + i * 1.5f) * 20;
        
        if (x > 50 && x < windowWidth - 50) {
            drawHexagon(window, x, y, 8, Color(100, 180, 255, 30 + static_cast<Uint8>(sin(time + i) * 15)), time * 10.0f);
        }
        
        // Bottom hexagons
        y = windowHeight - 80 + cos(time * 0.3f + i * 1.2f) * 25;
        if (x > 50 && x < windowWidth - 50) {
            drawHexagon(window, x, y, 6, Color(120, 200, 255, 25 + static_cast<Uint8>(cos(time + i) * 12)), -time * 12.0f);
        }
    }
    
    float headerY = centerY - 300;
    
    // Modern title with hexagon decorations
    Text titulo;
    titulo.setFont(font);
    titulo.setCharacterSize(42);
    titulo.setStyle(Text::Bold);
    titulo.setString("SELECCIONAR MAPA");
    titulo.setFillColor(Color(255, 255, 255));
    
    FloatRect titleBounds = titulo.getLocalBounds();
    titulo.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    titulo.setPosition(centerX, headerY);
    window.draw(titulo);
    
    // Hexagon decorations around title
    drawHexagon(window, centerX - 220, headerY, 15, Color(64, 120, 255, 120), time * 20.0f);
    drawHexagon(window, centerX + 220, headerY, 15, Color(64, 120, 255, 120), -time * 20.0f);
    
    // Subtle underline
    RectangleShape titleUnderline(Vector2f(300, 3));
    titleUnderline.setOrigin(150, 1.5f);
    titleUnderline.setPosition(centerX, headerY + 35);
    titleUnderline.setFillColor(Color(64, 120, 255, 180));
    window.draw(titleUnderline);
    
    float instructY = headerY + 80;
    
    // Modern instruction panel with hexagon decoration
    drawRoundedRect(window, centerX - 400, instructY - 20, 800, 40, 8, 
                    Color(20, 30, 50, 200), Color(64, 120, 255, 100), 1);
    
    // Small hexagons on instruction panel
    drawHexagon(window, centerX - 380, instructY, 8, Color(64, 120, 255, 80));
    drawHexagon(window, centerX + 380, instructY, 8, Color(64, 120, 255, 80));
    
    Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(14);
    instructions.setString("Arriba/Abajo Navegar    ENTER Confirmar    ESC Mapa por defecto    F5 Reescanear");
    instructions.setFillColor(Color(200, 220, 255, 200));
    
    FloatRect instBounds = instructions.getLocalBounds();
    instructions.setOrigin(instBounds.width / 2, instBounds.height / 2);
    instructions.setPosition(centerX, instructY);
    window.draw(instructions);
    
    float contentY = instructY + 60;
    
    // Main content panel with modern styling and hexagon corners
    drawRoundedRect(window, centerX - 450, contentY - 20, 900, 450, 16, 
                    Color(15, 25, 45, 240), Color(64, 120, 255, 120), 2);
    
    // Hexagon decorations on panel corners
    drawHexagon(window, centerX - 430, contentY - 5, 12, Color(64, 120, 255, 60), time * 15.0f);
    drawHexagon(window, centerX + 430, contentY - 5, 12, Color(64, 120, 255, 60), -time * 15.0f);
    drawHexagon(window, centerX - 430, contentY + 415, 12, Color(64, 120, 255, 60), time * 12.0f);
    drawHexagon(window, centerX + 430, contentY + 415, 12, Color(64, 120, 255, 60), -time * 12.0f);
    
    if (availableMaps.empty()) {
        // Error state with modern styling
        Text warningTitle;
        warningTitle.setFont(font);
        warningTitle.setCharacterSize(28);
        warningTitle.setStyle(Text::Bold);
        warningTitle.setString("NO SE ENCONTRARON MAPAS VALIDOS");
        warningTitle.setFillColor(Color(255, 100, 120));
        
        FloatRect warnBounds = warningTitle.getLocalBounds();
        warningTitle.setOrigin(warnBounds.width / 2, warnBounds.height / 2);
        warningTitle.setPosition(centerX, contentY + 180);
        window.draw(warningTitle);
        
        Text warningSubtext;
        warningSubtext.setFont(font);
        warningSubtext.setCharacterSize(16);
        warningSubtext.setString("Presiona F5 para reescanear la carpeta 'resources'\no ESC para usar el mapa por defecto");
        warningSubtext.setFillColor(Color(200, 200, 200));
        
        FloatRect subBounds = warningSubtext.getLocalBounds();
        warningSubtext.setOrigin(subBounds.width / 2, subBounds.height / 2);
        warningSubtext.setPosition(centerX, contentY + 230);
        window.draw(warningSubtext);
        
        return;
    }
    
    float listStartY = contentY + 30;
    float itemHeight = 52;
    int maxVisible = 7;
    
    int startIndex = std::max(0, std::min(selectedMapIndex - maxVisible / 2, 
                                         static_cast<int>(availableMaps.size()) - maxVisible));
    int endIndex = std::min(static_cast<int>(availableMaps.size()), startIndex + maxVisible);
    
    // Map list with modern card-like items
    for (int i = startIndex; i < endIndex; ++i) {
        float itemY = listStartY + (i - startIndex) * itemHeight;
        bool isSelected = (i == selectedMapIndex);
        
        // Modern card styling
        float cardX = centerX - 420;
        float cardWidth = 840;
        float cardHeight = itemHeight - 8;
        
        Color cardColor, borderColor;
        float borderWidth = 0;
        
        if (isSelected) {
            float pulse = sin(time * 4.0f) * 0.15f + 0.85f;
            if (availableMaps[i].isValid) {
                cardColor = Color(64, 120, 255, static_cast<Uint8>(120 * pulse));
                borderColor = Color(100, 160, 255);
                borderWidth = 2;
            } else {
                cardColor = Color(255, 80, 100, static_cast<Uint8>(120 * pulse));
                borderColor = Color(255, 120, 140);
                borderWidth = 2;
            }
        } else {
            if (availableMaps[i].isValid) {
                cardColor = Color(25, 35, 55, 160);
                borderColor = Color(40, 60, 90, 80);
                borderWidth = 1;
            } else {
                cardColor = Color(60, 30, 35, 120);
                borderColor = Color(80, 50, 55, 80);
                borderWidth = 1;
            }
        }
        
        drawRoundedRect(window, cardX, itemY - cardHeight/2, cardWidth, cardHeight, 8, 
                        cardColor, borderColor, borderWidth);
        
        // Hexagon as status indicator
        float hexSize = 8;
        Color hexColor;
        if (!availableMaps[i].isValid) {
            hexColor = Color(255, 100, 120);
        } else if (availableMaps[i].isDefault) {
            hexColor = Color(255, 200, 50);
        } else {
            hexColor = Color(100, 255, 150);
        }
        drawHexagon(window, cardX + 30, itemY, hexSize, hexColor, time * 30.0f + i * 45.0f);
        
        // Map name with modern typography
        Text mapName;
        mapName.setFont(font);
        mapName.setCharacterSize(20);
        
        std::string displayName = availableMaps[i].displayName;
        if (!availableMaps[i].isValid) {
            displayName += " [INVALIDO]";
        }
        
        if (displayName.length() > 35) {
            displayName = displayName.substr(0, 32) + "...";
        }
        
        mapName.setString(displayName);
        
        if (isSelected) {
            mapName.setFillColor(Color::White);
            mapName.setStyle(Text::Bold);
        } else {
            mapName.setFillColor(availableMaps[i].isValid ? Color(220, 230, 255) : Color(180, 140, 150));
        }
        
        mapName.setPosition(cardX + 60, itemY - 16);
        window.draw(mapName);
        
        // Map details with modern styling (sin puntos ni símbolos)
        Text mapInfo;
        mapInfo.setFont(font);
        mapInfo.setCharacterSize(12);
        
        std::string infoText = "";
        
        std::string lowerName = availableMaps[i].displayName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find("facil") != std::string::npos) {
            infoText = "Principiante 20x30 " + availableMaps[i].filename;
        } else if (lowerName.find("medio") != std::string::npos) {
            infoText = "Intermedio 40x50 " + availableMaps[i].filename;
        } else if (lowerName.find("dificil") != std::string::npos) {
            infoText = "Experto 70x70 " + availableMaps[i].filename;
        } else if (availableMaps[i].isDefault) {
            infoText = "Mapa clasico original " + availableMaps[i].filename;
        } else {
            infoText = availableMaps[i].filename;
        }
        
        if (infoText.length() > 70) {
            infoText = infoText.substr(0, 67) + "...";
        }
        
        mapInfo.setString(infoText);
        mapInfo.setFillColor(Color(160, 180, 220, 180));
        mapInfo.setPosition(cardX + 60, itemY + 6);
        window.draw(mapInfo);
    }

    float footerY = contentY + 360;
    
    // Modern stats display
    if (availableMaps.size() > maxVisible) {
        Text scrollInfo;
        scrollInfo.setFont(font);
        scrollInfo.setCharacterSize(14);
        scrollInfo.setString(std::to_string(selectedMapIndex + 1) + " de " + std::to_string(availableMaps.size()));
        scrollInfo.setFillColor(Color(160, 180, 220));
        
        FloatRect scrollBounds = scrollInfo.getLocalBounds();
        scrollInfo.setOrigin(scrollBounds.width / 2, scrollBounds.height / 2);
        scrollInfo.setPosition(centerX, footerY);
        window.draw(scrollInfo);
    }

    // Modern clipboard instruction panel ARRIBA del SELECCIONADO
    float pathPanelY = footerY + 30;
    drawRoundedRect(window, centerX - 420, pathPanelY - 35, 840, 70, 12, 
                    Color(25, 35, 60, 200), Color(64, 120, 255, 80), 1);

    // Hexagon decorations on clipboard panel
    drawHexagon(window, centerX - 400, pathPanelY, 12, Color(64, 120, 255, 80), time * 18.0f);
    drawHexagon(window, centerX + 400, pathPanelY, 12, Color(64, 120, 255, 80), -time * 18.0f);

    // Dividir el texto en líneas más legibles
    std::string line1 = "Tambien puedes copiar la ruta del mapa que tengas, por ejemplo:";
    std::string line2 = "C:\\Docs\\mymap.txt. Luego, regresa a la terminal, presiona la";
    std::string line3 = "tecla P y se abrira tu mapa automaticamente.";
    
    Text pathLine1, pathLine2, pathLine3;
    pathLine1.setFont(font);
    pathLine1.setCharacterSize(13);
    pathLine1.setString(line1);
    pathLine1.setFillColor(Color(180, 200, 255));
    
    pathLine2.setFont(font);
    pathLine2.setCharacterSize(13);
    pathLine2.setString(line2);
    pathLine2.setFillColor(Color(180, 200, 255));
    
    pathLine3.setFont(font);
    pathLine3.setCharacterSize(13);
    pathLine3.setString(line3);
    pathLine3.setFillColor(Color(180, 200, 255));
    
    FloatRect line1Bounds = pathLine1.getLocalBounds();
    FloatRect line2Bounds = pathLine2.getLocalBounds();
    FloatRect line3Bounds = pathLine3.getLocalBounds();
    
    pathLine1.setOrigin(line1Bounds.width / 2, line1Bounds.height / 2);
    pathLine1.setPosition(centerX, pathPanelY - 15);
    window.draw(pathLine1);
    
    pathLine2.setOrigin(line2Bounds.width / 2, line2Bounds.height / 2);
    pathLine2.setPosition(centerX, pathPanelY);
    window.draw(pathLine2);
    
    pathLine3.setOrigin(line3Bounds.width / 2, line3Bounds.height / 2);
    pathLine3.setPosition(centerX, pathPanelY + 15);
    window.draw(pathLine3);

    // Current selection display with modern styling - MOVIDO ABAJO
    if (!availableMaps.empty() && selectedMapIndex >= 0 && 
        selectedMapIndex < static_cast<int>(availableMaps.size())) {
        
        float selectionY = pathPanelY + 80;
        
        Color panelColor, borderColor;
        if (isCurrentSelectionValid()) {
            panelColor = Color(20, 60, 40, 220);
            borderColor = Color(100, 255, 150, 150);
        } else {
            panelColor = Color(60, 20, 30, 220);
            borderColor = Color(255, 120, 140, 150);
        }
        
        drawRoundedRect(window, centerX - 400, selectionY - 25, 800, 50, 12, 
                        panelColor, borderColor, 2);
        
        // Hexagon decorations on selection panel
        drawHexagon(window, centerX - 380, selectionY, 10, 
                   isCurrentSelectionValid() ? Color(100, 255, 150, 100) : Color(255, 120, 140, 100), 
                   time * 25.0f);
        drawHexagon(window, centerX + 380, selectionY, 10, 
                   isCurrentSelectionValid() ? Color(100, 255, 150, 100) : Color(255, 120, 140, 100), 
                   -time * 25.0f);
        
        Text selectionLabel;
        selectionLabel.setFont(font);
        selectionLabel.setCharacterSize(12);
        selectionLabel.setString("SELECCIONADO:");
        selectionLabel.setFillColor(Color(180, 180, 180));
        selectionLabel.setPosition(centerX - 380, selectionY - 18);
        window.draw(selectionLabel);
        
        Text selectionName;
        selectionName.setFont(font);
        selectionName.setCharacterSize(18);
        selectionName.setStyle(Text::Bold);
        
        std::string selectedText = availableMaps[selectedMapIndex].displayName;
        if (!isCurrentSelectionValid()) {
            selectedText += " [MAPA INVALIDO - USA ESC]";
            selectionName.setFillColor(Color(255, 150, 170));
        } else {
            selectionName.setFillColor(Color(150, 255, 180));
        }
        
        selectionName.setString(selectedText);
        selectionName.setPosition(centerX - 380, selectionY + 2);
        window.draw(selectionName);
    }
}