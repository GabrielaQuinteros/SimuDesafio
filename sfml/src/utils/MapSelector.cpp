#include "MapSelector.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

using namespace sf;
namespace fs = std::filesystem;

std::vector<MapInfo> availableMaps;
int selectedMapIndex = 0;
std::string mappath = "O tambien puedes P para usar la direccion pegada en el portapapeles.\nEj: C:\\Docs\\mymap.txt";

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
        
        std::cout << "  - Líneas totales: " << lineCount << std::endl;
        std::cout << "  - Líneas de juego válidas: " << validGameLines << std::endl;
        std::cout << "  - Tiene Start (S): " << (hasStart ? "Sí" : "No") << std::endl;
        std::cout << "  - Tiene Goal (G): " << (hasGoal ? "Sí" : "No") << std::endl;
        std::cout << "  - Es válido: " << (isValid ? "Sí" : "No") << std::endl;
        
        return isValid;
        
    } catch (const std::exception& e) {
        std::cout << "  - Excepción: " << e.what() << std::endl;
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
                
                std::cout << "Procesando entrada: " << fullPath << " con extensión: '" << extension << "'" << std::endl;
                
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
                              << " (Válido: " << (isValid ? "Sí" : "No") 
                              << ", Defecto: " << (isDefault ? "Sí" : "No") << ")" << std::endl;
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
            std::cout << "Mapa por defecto agregado: resources/map.txt (Válido: " << (isValid ? "Sí" : "No") << ")" << std::endl;
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
                  << (map.isValid ? "VÁLIDO" : "INVÁLIDO") 
                  << (map.isDefault ? " [DEFECTO]" : "") << std::endl;
    }
    
    std::cout << "Mapas válidos: " << validCount << std::endl;
    std::cout << "Índice seleccionado inicial: " << selectedMapIndex << std::endl;
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

void drawClipboardIcon(RenderWindow& window, float x, float y, float scale) {
    
    RectangleShape clipboard(Vector2f(40 * scale, 50 * scale));
    clipboard.setPosition(x - 20 * scale, y - 25 * scale);
    clipboard.setFillColor(Color(240, 240, 240));
    clipboard.setOutlineColor(Color(100, 100, 100));
    clipboard.setOutlineThickness(2 * scale);
    
    RectangleShape clip(Vector2f(20 * scale, 8 * scale));
    clip.setPosition(x - 10 * scale, y - 30 * scale);
    clip.setFillColor(Color(150, 150, 150));
    
    for (int i = 0; i < 3; ++i) {
        RectangleShape line(Vector2f(25 * scale, 2 * scale));
        line.setPosition(x - 12 * scale, y - 10 * scale + i * 8 * scale);
        line.setFillColor(Color(80, 80, 80));
        window.draw(line);
    }
    
    window.draw(clipboard);
    window.draw(clip);
}

void mostrarSelectorMapas(RenderWindow& window, Font& font) {
    static Clock selectorClock;
    float time = selectorClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    
    for (int i = 0; i < window.getSize().y; i += 3) {
        float gradient = static_cast<float>(i) / window.getSize().y;
        Color topColor(12, 25, 55);
        Color bottomColor(25, 45, 85);
        
        float t = gradient;
        Color currentColor(
            static_cast<Uint8>(topColor.r + (bottomColor.r - topColor.r) * t),
            static_cast<Uint8>(topColor.g + (bottomColor.g - topColor.g) * t),
            static_cast<Uint8>(topColor.b + (bottomColor.b - topColor.b) * t)
        );
        
        RectangleShape line(Vector2f(windowWidth, 3));
        line.setPosition(0, static_cast<float>(i));
        line.setFillColor(currentColor);
        window.draw(line);
    }
    
    for (int i = 0; i < 15; ++i) {
        float x = fmod(time * 15.0f + i * 80.0f, windowWidth + 120.0f) - 60.0f;
        float y = 100.0f + sin(time * 0.6f + i * 0.8f) * 40.0f + i * 35.0f;
        
        if (y < windowHeight - 50) {
            CircleShape hex(6 + (i % 2) * 2, 6);
            hex.setOrigin(6 + (i % 2) * 2, 6 + (i % 2) * 2);
            hex.setPosition(x, y);
            hex.setFillColor(Color::Transparent);
            hex.setOutlineColor(Color(0, 150, 255, 30));
            hex.setOutlineThickness(1);
            hex.rotate(time * 20.0f);
            window.draw(hex);
        }
    }
    
    float headerY = centerY - 280;
    
    CircleShape titleHex(60, 6);
    titleHex.setOrigin(60, 60);
    titleHex.setPosition(centerX, headerY);
    titleHex.setFillColor(Color::Transparent);
    titleHex.setOutlineColor(Color(0, 255, 150, 150));
    titleHex.setOutlineThickness(2);
    titleHex.rotate(time * 12.0f);
    window.draw(titleHex);
    
    Text titulo;
    titulo.setFont(font);
    titulo.setCharacterSize(32);
    titulo.setStyle(Text::Bold);
    titulo.setString("SELECCIONAR MAPA");
    titulo.setFillColor(Color(0, 255, 150));
    
    FloatRect titleBounds = titulo.getLocalBounds();
    titulo.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    titulo.setPosition(centerX, headerY);
    window.draw(titulo);
    
    float instructY = headerY + 80;
    
    RectangleShape instructPanel(Vector2f(700, 35));
    instructPanel.setOrigin(350, 17.5f);
    instructPanel.setPosition(centerX, instructY);
    instructPanel.setFillColor(Color(20, 40, 80, 180));
    instructPanel.setOutlineColor(Color(0, 200, 255, 100));
    instructPanel.setOutlineThickness(1);
    window.draw(instructPanel);
    
    Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(12);
    instructions.setString("W/S: Navegar  |  ENTER: Confirmar  |  ESC: Mapa por defecto  |  F5: Reescanear mapas");
    instructions.setFillColor(Color(200, 220, 255));
    
    FloatRect instBounds = instructions.getLocalBounds();
    instructions.setOrigin(instBounds.width / 2, instBounds.height / 2);
    instructions.setPosition(centerX, instructY);
    window.draw(instructions);
    
    float contentY = instructY + 50;
    
    RectangleShape mainPanel(Vector2f(900, 400));
    mainPanel.setOrigin(450, 200);
    mainPanel.setPosition(centerX, contentY + 200);
    mainPanel.setFillColor(Color(15, 30, 70, 220));
    mainPanel.setOutlineColor(Color(0, 200, 255, 150));
    mainPanel.setOutlineThickness(2);
    window.draw(mainPanel);
    
    if (availableMaps.empty()) {
        Text warningTitle;
        warningTitle.setFont(font);
        warningTitle.setCharacterSize(24);
        warningTitle.setStyle(Text::Bold);
        warningTitle.setString("NO SE ENCONTRARON MAPAS VALIDOS");
        warningTitle.setFillColor(Color(255, 100, 100));
        
        FloatRect warnBounds = warningTitle.getLocalBounds();
        warningTitle.setOrigin(warnBounds.width / 2, warnBounds.height / 2);
        warningTitle.setPosition(centerX, contentY + 150);
        window.draw(warningTitle);
        
        Text warningSubtext;
        warningSubtext.setFont(font);
        warningSubtext.setCharacterSize(14);
        warningSubtext.setString("Presiona F5 para reescanear la carpeta 'resources'\no ESC para usar el mapa por defecto");
        warningSubtext.setFillColor(Color(255, 200, 100));
        
        FloatRect subBounds = warningSubtext.getLocalBounds();
        warningSubtext.setOrigin(subBounds.width / 2, subBounds.height / 2);
        warningSubtext.setPosition(centerX, contentY + 200);
        window.draw(warningSubtext);
        
        return;
    }
    
    float listStartY = contentY + 50;
    float itemHeight = 45;
    int maxVisible = 7;
    
    int startIndex = std::max(0, std::min(selectedMapIndex - maxVisible / 2, 
                                         static_cast<int>(availableMaps.size()) - maxVisible));
    int endIndex = std::min(static_cast<int>(availableMaps.size()), startIndex + maxVisible);
    
    for (int i = startIndex; i < endIndex; ++i) {
        float itemY = listStartY + (i - startIndex) * itemHeight;
        bool isSelected = (i == selectedMapIndex);
        
        RectangleShape itemBg(Vector2f(850, itemHeight - 6));
        itemBg.setPosition(centerX - 425, itemY - (itemHeight - 6) / 2);
        
        if (isSelected) {
            float pulse = sin(time * 6.0f) * 0.2f + 0.8f;
            if (availableMaps[i].isValid) {
                itemBg.setFillColor(Color(50, 120, 200, static_cast<Uint8>(150 * pulse)));
                itemBg.setOutlineColor(Color(255, 255, 100));
                itemBg.setOutlineThickness(2);
            } else {
                itemBg.setFillColor(Color(150, 50, 50, static_cast<Uint8>(150 * pulse)));
                itemBg.setOutlineColor(Color(255, 150, 150));
                itemBg.setOutlineThickness(2);
            }
        } else {
            if (availableMaps[i].isValid) {
                itemBg.setFillColor(Color(25, 50, 100, 80));
                itemBg.setOutlineColor(Color(80, 120, 160, 80));
            } else {
                itemBg.setFillColor(Color(80, 40, 40, 60));
                itemBg.setOutlineColor(Color(120, 80, 80, 80));
            }
            itemBg.setOutlineThickness(1);
        }
        window.draw(itemBg);
        
        CircleShape indicator(12, 6);
        indicator.setOrigin(12, 12);
        indicator.setPosition(centerX - 390, itemY);
        indicator.setFillColor(Color::Transparent);

        if (!availableMaps[i].isValid) {
            indicator.setOutlineColor(Color(255, 80, 80));
        } else if (availableMaps[i].isDefault) {
            indicator.setOutlineColor(Color(255, 215, 0));
        } else {
            indicator.setOutlineColor(Color(100, 255, 150));
        }
        indicator.setOutlineThickness(2);
        window.draw(indicator);
        
        Text mapName;
        mapName.setFont(font);
        mapName.setCharacterSize(18);
        
        std::string displayName = availableMaps[i].displayName;
        if (!availableMaps[i].isValid) {
            displayName += " [INVALIDO]";
        }
        
        if (displayName.length() > 40) {
            displayName = displayName.substr(0, 37) + "...";
        }
        
        mapName.setString(displayName);
        
        if (isSelected) {
            mapName.setFillColor(availableMaps[i].isValid ? Color::White : Color(255, 180, 180));
            mapName.setStyle(Text::Bold);
        } else {
            mapName.setFillColor(availableMaps[i].isValid ? Color(220, 220, 255) : Color(180, 120, 120));
        }
        
        mapName.setPosition(centerX - 350, itemY - 15);
        window.draw(mapName);
        
        Text mapInfo;
        mapInfo.setFont(font);
        mapInfo.setCharacterSize(11);
        
        std::string infoText = availableMaps[i].filename;
        
        std::string lowerName = availableMaps[i].displayName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find("facil") != std::string::npos) {
            infoText += "  [Principiante 20x30]";
        } else if (lowerName.find("medio") != std::string::npos) {
            infoText += "  [Intermedio 40x50]";
        } else if (lowerName.find("dificil") != std::string::npos) {
            infoText += "  [Experto 70x70]";
        } else if (availableMaps[i].isDefault) {
            infoText += "  [Mapa clasico original]";
        }
        
        if (infoText.length() > 70) {
            infoText = infoText.substr(0, 67) + "...";
        }
        
        mapInfo.setString(infoText);
        mapInfo.setFillColor(Color(150, 170, 200));
        mapInfo.setPosition(centerX - 350, itemY + 5);
        window.draw(mapInfo);
    }

    float footerY = contentY + 320;
    
    if (availableMaps.size() > maxVisible) {
        Text scrollInfo;
        scrollInfo.setFont(font);
        scrollInfo.setCharacterSize(12);
        scrollInfo.setString("Mapa " + std::to_string(selectedMapIndex + 1) + 
                           " de " + std::to_string(availableMaps.size()));
        scrollInfo.setFillColor(Color(150, 200, 255));
        
        FloatRect scrollBounds = scrollInfo.getLocalBounds();
        scrollInfo.setOrigin(scrollBounds.width / 2, scrollBounds.height / 2);
        scrollInfo.setPosition(centerX - 300, footerY);
        window.draw(scrollInfo);
    }
    
    Text mapStats;
    mapStats.setFont(font);
    mapStats.setCharacterSize(12);
    
    int validCount = 0;
    for (const auto& map : availableMaps) {
        if (map.isValid) validCount++;
    }
    
    mapStats.setString("Encontrados: " + std::to_string(availableMaps.size()) + 
                      " | Validos: " + std::to_string(validCount));
    mapStats.setFillColor(Color(100, 255, 150));
    
    FloatRect statsBounds = mapStats.getLocalBounds();
    mapStats.setOrigin(statsBounds.width / 2, statsBounds.height / 2);
    mapStats.setPosition(centerX + 300, footerY);
    window.draw(mapStats);

    float pathPanelWidth = 800;
    float pathPanelHeight = 100;
    float pathPanelY = centerY + 275;
    float pathPanelX = centerX - 400;

    RectangleShape pathRect(Vector2f(pathPanelWidth, pathPanelHeight));
    pathRect.setPosition(pathPanelX, pathPanelY);
    pathRect.setFillColor(Color::White);
    pathRect.setOutlineColor(Color(0, 200, 255, 80));
    pathRect.setOutlineThickness(2);
    window.draw(pathRect);

    Text pathIndicator(mappath, font);
    pathIndicator.setCharacterSize(18);
    FloatRect pathBounds = pathIndicator.getLocalBounds();
    pathIndicator.setOrigin(pathBounds.width / 2, 0);
    pathIndicator.setColor(Color::Black);
    pathIndicator.setPosition(centerX - 100, centerY + 300);
    window.draw(pathIndicator);

    drawClipboardIcon(window, centerX + 300, centerY + 300, 1.5f);

    if (!availableMaps.empty() && selectedMapIndex >= 0 && 
        selectedMapIndex < static_cast<int>(availableMaps.size())) {
        
        float selectionY = footerY + 40;
        
        RectangleShape selectionPanel(Vector2f(800, 45));
        selectionPanel.setOrigin(400, 22.5f);
        selectionPanel.setPosition(centerX, selectionY);
        
        if (isCurrentSelectionValid()) {
            selectionPanel.setFillColor(Color(20, 80, 40, 200));
            selectionPanel.setOutlineColor(Color(100, 255, 150));
        } else {
            selectionPanel.setFillColor(Color(80, 20, 20, 200));
            selectionPanel.setOutlineColor(Color(255, 150, 150));
        }
        selectionPanel.setOutlineThickness(2);
        window.draw(selectionPanel);
        
        Text selectionLabel;
        selectionLabel.setFont(font);
        selectionLabel.setCharacterSize(12);
        selectionLabel.setString("SELECCIONADO:");
        selectionLabel.setFillColor(Color(200, 200, 200));
        selectionLabel.setPosition(centerX - 380, selectionY - 15);
        window.draw(selectionLabel);
        
        Text selectionName;
        selectionName.setFont(font);
        selectionName.setCharacterSize(16);
        selectionName.setStyle(Text::Bold);
        
        std::string selectedText = availableMaps[selectedMapIndex].displayName;
        if (!isCurrentSelectionValid()) {
            selectedText += " [MAPA INVALIDO - USA ESC PARA DEFECTO]";
            selectionName.setFillColor(Color(255, 150, 150));
        } else {
            selectionName.setFillColor(Color(150, 255, 150));
        }
        
        selectionName.setString(selectedText);
        selectionName.setPosition(centerX - 380, selectionY + 5);
        window.draw(selectionName);
    }
    
    RectangleShape bottomLine(Vector2f(700, 2));
    bottomLine.setOrigin(350, 1);
    bottomLine.setPosition(centerX, footerY + 90);
    bottomLine.setFillColor(Color(0, 150, 255, 150));
    window.draw(bottomLine);
}