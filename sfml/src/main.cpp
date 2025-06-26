#include <string>
#include <SFML/Graphics.hpp>
#include "utils/MapLoader.hpp"
#include "utils/Utils.hpp"
#include "model/HexCell.hpp"
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include "core/GameLogic.hpp"
#include "core/TurnSystem.hpp"
#include "render/Renderer.hpp"
#include "core/PathFinding.hpp"
#include "core/AutoMovement.hpp"
#include <iostream>
#include <fstream>     // NUEVO: Para leer archivos
#include <vector>
#include <filesystem>  // NUEVO: Para escanear archivos
#include <algorithm>   // NUEVO: Para ordenar
#include <cctype>      // NUEVO: Para std::toupper

// Definición de constantes para la ventana y recursos
#define WINDOW_WIDTH 1400  
#define WINDOW_HEIGHT 900  
#define MAP_PATH "resources/map.txt"
#define FONT_PATH "resources/arial.ttf"
#define ICON_PATH "resources/pasteicon.png"

using namespace model;
using namespace sf;
namespace fs = std::filesystem;  // NUEVO

// ==================== NUEVO: SELECTOR DE MAPAS ====================

struct MapInfo {
    std::string filename;
    std::string displayName;
    bool isDefault;
    bool isValid;  // NUEVO: Para marcar si el mapa es válido
};

std::vector<MapInfo> availableMaps;
int selectedMapIndex = 0;

// FUNCIÓN CORREGIDA: Verificar si un archivo es un mapa válido
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
        
        // Verificar las primeras líneas para confirmar formato
        while (std::getline(file, line) && lineCount < 100) {
            // Limpiar la línea de espacios en blanco
            std::string trimmedLine = line;
            trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t\r\n"));
            trimmedLine.erase(trimmedLine.find_last_not_of(" \t\r\n") + 1);
            
            if (!trimmedLine.empty()) {
                // Verificar si contiene caracteres típicos de mapa de juego
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
        
        // Un mapa válido debe tener S (start), G (goal), al menos 10 líneas de juego
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

// FUNCIÓN CORREGIDA: Escanear archivos de mapas
void scanMapFiles() {
    availableMaps.clear();
    
    std::cout << "=== ESCANEANDO MAPAS ===" << std::endl;
    
    try {
        // Buscar archivos .txt en la carpeta resources
        if (fs::exists("resources") && fs::is_directory("resources")) {
            std::cout << "Carpeta resources encontrada, buscando mapas..." << std::endl;
            
            // NUEVO: Primero listar TODOS los archivos para debug
            std::cout << "\n--- LISTANDO TODOS LOS ARCHIVOS EN RESOURCES ---" << std::endl;
            for (const auto& entry : fs::directory_iterator("resources")) {
                std::cout << "Archivo encontrado: " << entry.path().string() 
                          << " (Ext: " << entry.path().extension().string() << ")" << std::endl;
            }
            std::cout << "--- FIN DE LISTADO ---\n" << std::endl;
            
            // Ahora procesar solo los .txt
            for (const auto& entry : fs::directory_iterator("resources")) {
                std::string fullPath = entry.path().string();
                std::string extension = entry.path().extension().string();
                
                std::cout << "Procesando entrada: " << fullPath << " con extensión: '" << extension << "'" << std::endl;
                
                if (entry.is_regular_file() && extension == ".txt") {
                    std::string filename = entry.path().string();
                    std::string basename = entry.path().stem().string();
                    
                    // Normalizar separadores de ruta
                    std::replace(filename.begin(), filename.end(), '\\', '/');
                    
                    std::cout << "\n--- Procesando archivo .txt: " << filename << " ---" << std::endl;
                    std::cout << "Basename: " << basename << std::endl;
                    
                    // SIMPLIFICADO: Solo excluir archivos obvios que NO son mapas
                    std::string lowerBasename = basename;
                    std::transform(lowerBasename.begin(), lowerBasename.end(), lowerBasename.begin(), ::tolower);
                    
                    // Lista específica de archivos a excluir
                    if (lowerBasename == "license" || lowerBasename == "readme" || 
                        lowerBasename == "changelog" || lowerBasename == "credits" ||
                        lowerBasename == "arial") {  // Excluir arial.ttf también
                        std::cout << "Archivo excluido (no es mapa): " << basename << std::endl;
                        continue;
                    }
                    
                    std::cout << "Procesando como posible mapa..." << std::endl;
                    
                    // Verificar si es un mapa válido
                    bool isValid = isValidMapFile(filename);
                    
                    std::string displayName = basename;
                    // Capitalizar primera letra y reemplazar guiones bajos
                    if (!displayName.empty()) {
                        displayName[0] = std::toupper(displayName[0]);
                        std::replace(displayName.begin(), displayName.end(), '_', ' ');
                    }
                    
                    // Marcar el mapa original como defecto
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
    
    // Si no hay mapas válidos, intentar agregar el mapa por defecto
    if (availableMaps.empty()) {
        std::cout << "No se encontraron mapas, intentando agregar mapa por defecto..." << std::endl;
        if (fs::exists(MAP_PATH)) {
            bool isValid = isValidMapFile(MAP_PATH);
            availableMaps.push_back({MAP_PATH, "Mapa Original (Defecto)", true, isValid});
            std::cout << "Mapa por defecto agregado: " << MAP_PATH << " (Válido: " << (isValid ? "Sí" : "No") << ")" << std::endl;
        }
    }
    
    // Ordenar: primero el defecto, luego por dificultad, luego por nombre
    std::sort(availableMaps.begin(), availableMaps.end(), 
              [](const MapInfo& a, const MapInfo& b) {
                  // Primero el mapa por defecto
                  if (a.isDefault != b.isDefault) {
                      return a.isDefault > b.isDefault;
                  }
                  // Luego por validez
                  if (a.isValid != b.isValid) {
                      return a.isValid > b.isValid;
                  }
                  
                  // Orden específico para los mapas de dificultad
                  std::string lowerA = a.displayName;
                  std::string lowerB = b.displayName;
                  std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
                  std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
                  
                  // Definir orden de dificultad
                  auto getDifficultyOrder = [](const std::string& name) -> int {
                      if (name.find("facil") != std::string::npos) return 1;
                      if (name.find("medio") != std::string::npos) return 2;
                      if (name.find("dificil") != std::string::npos) return 3;
                      return 4; // Otros mapas al final
                  };
                  
                  int orderA = getDifficultyOrder(lowerA);
                  int orderB = getDifficultyOrder(lowerB);
                  
                  if (orderA != orderB) {
                      return orderA < orderB;
                  }
                  
                  // Si tienen el mismo orden, ordenar alfabéticamente
                  return a.displayName < b.displayName;
              });
    
    // Resetear índice seleccionado al primer mapa válido
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

// FUNCIÓN CORREGIDA: Obtener el mapa seleccionado actual
std::string getCurrentSelectedMap() {
    if (availableMaps.empty() || selectedMapIndex < 0 || selectedMapIndex >= static_cast<int>(availableMaps.size())) {
        return MAP_PATH; // Fallback al mapa por defecto
    }
    return availableMaps[selectedMapIndex].filename;
}

// FUNCIÓN CORREGIDA: Verificar si el mapa seleccionado es válido
bool isCurrentSelectionValid() {
    if (availableMaps.empty() || selectedMapIndex < 0 || selectedMapIndex >= static_cast<int>(availableMaps.size())) {
        return false;
    }
    return availableMaps[selectedMapIndex].isValid;
}

std::string mappath = "O tambien puedes P para usar la direccion pegada en el portapapeles.\nEj: C:\\Docs\\mymap.txt";

void mostrarSelectorMapas(RenderWindow& window, Font& font, Texture& icon) {
    static Clock selectorClock;
    float time = selectorClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    
    // Fondo degradado limpio
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
    
    // Hexagonos decorativos sutiles
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
    
    // === HEADER SECTION ===
    float headerY = centerY - 280;
    
    // Marco hexagonal para el titulo
    CircleShape titleHex(60, 6);
    titleHex.setOrigin(60, 60);
    titleHex.setPosition(centerX, headerY);
    titleHex.setFillColor(Color::Transparent);
    titleHex.setOutlineColor(Color(0, 255, 150, 150));
    titleHex.setOutlineThickness(2);
    titleHex.rotate(time * 12.0f);
    window.draw(titleHex);
    
    // Titulo principal - bien centrado
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
    
    // === INSTRUCTIONS SECTION ===
    float instructY = headerY + 80;
    
    // Panel de instrucciones
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
    
    // === MAIN CONTENT AREA ===
    float contentY = instructY + 50;
    
    // Panel principal bien dimensionado
    RectangleShape mainPanel(Vector2f(900, 400));
    mainPanel.setOrigin(450, 200);
    mainPanel.setPosition(centerX, contentY + 200);
    mainPanel.setFillColor(Color(15, 30, 70, 220));
    mainPanel.setOutlineColor(Color(0, 200, 255, 150));
    mainPanel.setOutlineThickness(2);
    window.draw(mainPanel);
    
    // MENSAJE DE ADVERTENCIA si no hay mapas
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
    
    // === MAP LIST SECTION ===
    float listStartY = contentY + 50;
    float itemHeight = 45;
    int maxVisible = 7;
    
    // Calcular rango visible con mejor logica
    int startIndex = std::max(0, std::min(selectedMapIndex - maxVisible / 2, 
                                         static_cast<int>(availableMaps.size()) - maxVisible));
    int endIndex = std::min(static_cast<int>(availableMaps.size()), startIndex + maxVisible);
    
    // Dibujar items de la lista
    for (int i = startIndex; i < endIndex; ++i) {
        float itemY = listStartY + (i - startIndex) * itemHeight;
        bool isSelected = (i == selectedMapIndex);
        
        // Fondo del item con mejor diseño
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
        
        // Icono indicador mejorado
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
        
        // Nombre del mapa con mejor tipografia
        Text mapName;
        mapName.setFont(font);
        mapName.setCharacterSize(18);
        
        std::string displayName = availableMaps[i].displayName;
        if (!availableMaps[i].isValid) {
            displayName += " [INVALIDO]";
        }
        
        // Truncar si es muy largo
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
        
        // Informacion adicional (ruta y dificultad)
        Text mapInfo;
        mapInfo.setFont(font);
        mapInfo.setCharacterSize(11);
        
        std::string infoText = availableMaps[i].filename;
        
        // Agregar info de dificultad si aplica
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
        
        // Truncar info si es muy larga
        if (infoText.length() > 70) {
            infoText = infoText.substr(0, 67) + "...";
        }
        
        mapInfo.setString(infoText);
        mapInfo.setFillColor(Color(150, 170, 200));
        mapInfo.setPosition(centerX - 350, itemY + 5);
        window.draw(mapInfo);
    }

    // === FOOTER SECTION ===
    float footerY = contentY + 320;
    
    // Indicador de posicion en la lista
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
    
    // Contador de mapas validos
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

    // Panel de fondo para la ruta pegada, centrado y con espacio para el icono
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


    Sprite iconSprite;
    iconSprite.setTexture(icon);
    iconSprite.setScale(0.12f, 0.12f);
    iconSprite.setOrigin(50, 50);
    iconSprite.setPosition(centerX + 300, centerY + 300);
    window.draw(iconSprite);

    // === SELECTION INFO SECTION ===
    if (!availableMaps.empty() && selectedMapIndex >= 0 && 
        selectedMapIndex < static_cast<int>(availableMaps.size())) {
        
        float selectionY = footerY + 40;
        
        // Panel de seleccion actual
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
    
    // Linea decorativa final
    RectangleShape bottomLine(Vector2f(700, 2));
    bottomLine.setOrigin(350, 1);
    bottomLine.setPosition(centerX, footerY + 90);
    bottomLine.setFillColor(Color(0, 150, 255, 150));
    window.draw(bottomLine);
}

// ==================== FIN SELECTOR DE MAPAS ====================

// TU FUNCIÓN ORIGINAL mostrarIntro MODIFICADA para que diga "CONTINUAR"
void mostrarIntro(RenderWindow& window, Font& font) {
    static Clock introClock; // Reloj estático para animaciones
    float time = introClock.getElapsedTime().asSeconds();
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    
    // Fondo degradado animado
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
    
    // Hexágonos decorativos flotantes
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
    
    // Marco hexagonal principal para el título
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
    
    // Título principal con efecto pulsante
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
    
    // Subtítulo elegante
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
    
    // Líneas decorativas
    RectangleShape line1(Vector2f(350, 2));
    line1.setOrigin(175, 1);
    line1.setPosition(centerX, centerY - 125);
    line1.setFillColor(Color(0, 255, 150));
    window.draw(line1);
    
    // Panel de contexto central
    RectangleShape contextPanel(Vector2f(700, 120));
    contextPanel.setOrigin(350, 60);
    contextPanel.setPosition(centerX, centerY - 40);
    contextPanel.setFillColor(Color(20, 40, 80, 180));
    contextPanel.setOutlineColor(Color(0, 255, 150));
    contextPanel.setOutlineThickness(2);
    window.draw(contextPanel);
    
    // Contexto del juego
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
    
    // Panel de controles izquierdo
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
    
    // Panel de modos derecho
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
    
    // Panel de características inferiores
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
    
    // Botón de inicio épico con animación - CAMBIADO A "CONTINUAR"
    float startPulse = sin(time * 6.0f) * 0.5f + 0.5f;
    
    // Marco del botón
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
    startText.setString("PRESIONA  ESPACIO  PARA  CONTINUAR"); // CAMBIADO
    startText.setFillColor(Color(
        static_cast<Uint8>(255 * startPulse),
        static_cast<Uint8>(255 * startPulse),
        static_cast<Uint8>(100 + 155 * startPulse)
    ));
    
    FloatRect startBounds = startText.getLocalBounds();
    startText.setOrigin(startBounds.width / 2, startBounds.height / 2);
    startText.setPosition(centerX, centerY + 350);
    window.draw(startText);
    
    // Hexágonos decorativos alrededor del botón
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
    
    // Líneas decorativas inferiores
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

// MEJORADA: Verificar si el camino actual sigue siendo válido
bool isPathStillValid(const HexGrid& grid, const std::vector<std::pair<int, int>>& pathCells,
                     int playerRow, int playerCol) {
    if (pathCells.empty()) return false;
   
    // Verificar que el jugador esté en algún punto del camino o cerca del inicio
    bool playerInPath = false;
    for (size_t i = 0; i < pathCells.size(); ++i) {
        if (pathCells[i].first == playerRow && pathCells[i].second == playerCol) {
            playerInPath = true;
            break;
        }
    }
   
    // Si el jugador no está en el camino, verificar si está cerca del inicio
    if (!playerInPath && !pathCells.empty()) {
        const auto& firstStep = pathCells[0];
        int distance = abs(firstStep.first - playerRow) + abs(firstStep.second - playerCol);
        if (distance > 2) {
            return false; // El jugador está muy lejos del camino
        }
    }
   
    // Verificar que no haya paredes bloqueando el camino
    for (const auto& cell : pathCells) {
        CellType cellType = grid.at(cell.first, cell.second).type;
        if (cellType == CellType::WALL) {
            return false;
        }
    }
   
    return true;
}

// MEJORADA: Recalcular camino con mejor manejo de estados
bool recalculatePath(HexGrid& grid, Player& player, HexCell* goal,
                    std::vector<std::pair<int, int>>& pathCells,
                    bool& showPathVisualization, bool& autoSolveMode) {
   
    std::cout << "Recalculando camino desde main..." << std::endl;
    PathfindingResult newPath = findPath(grid, player.row, player.col, goal->row, goal->col, player.energy);
   
    if (newPath.success && !newPath.path.empty()) {
        // Actualizar el camino
        pathCells.clear();
        for (auto *cell : newPath.path) {
            pathCells.emplace_back(cell->row, cell->col);
        }
       
        std::cout << "Camino recalculado exitosamente con " << pathCells.size() << " pasos." << std::endl;
        return true;
    } else {
        // No hay camino disponible - limpiar todo
        std::cout << "No se pudo recalcular el camino. Limpiando estado." << std::endl;
        pathCells.clear();
        showPathVisualization = false;
        autoSolveMode = false;
        player.isAutoMoving = false;
       
        return false;
    }
}

// FUNCIÓN CORREGIDA: Cargar mapa seleccionado con manejo de errores
bool loadSelectedMap(const std::string& mapPath, HexGrid*& grid, HexCell*& start, HexCell*& goal, Player*& player) {
    try {
        std::cout << "Intentando cargar mapa: " << mapPath << std::endl;
        
        // Liberar memoria anterior si existe
        if (grid) {
            delete grid;
            grid = nullptr;
        }
        if (player) {
            delete player;
            player = nullptr;
        }
        
        // Cargar el nuevo mapa
        HexGrid tempGrid = loadHexGridFromFile(mapPath);
        HexCell* tempStart = findStartCell(tempGrid);
        HexCell* tempGoal = findGoalCell(tempGrid);
        
        if (tempStart && tempGoal) {
            grid = new HexGrid(tempGrid);
            start = findStartCell(*grid);
            goal = findGoalCell(*grid);
            player = new Player(start->row, start->col);
            TurnSystem::resetTurnCounter();
            
            std::cout << "Mapa cargado exitosamente: " << mapPath << std::endl;
            std::cout << "Start: (" << start->row << ", " << start->col << ")" << std::endl;
            std::cout << "Goal: (" << goal->row << ", " << goal->col << ")" << std::endl;
            return true;
        } else {
            std::cout << "Error: Mapa no tiene start y/o goal válidos" << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Excepción al cargar mapa: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cout << "Error desconocido al cargar mapa" << std::endl;
        return false;
    }
}

int main()
{
    // NUEVO: Escanear mapas disponibles al inicio
    std::cout << "=== INICIANDO HEXESCAPE ===" << std::endl;
    std::cout << "Escaneando mapas en carpeta resources..." << std::endl;
    scanMapFiles();
    
    // Mostrar mapas encontrados
    std::cout << "\n=== MAPAS DISPONIBLES ===" << std::endl;
    for (size_t i = 0; i < availableMaps.size(); ++i) {
        std::cout << i << ". " << availableMaps[i].displayName 
                  << " (" << availableMaps[i].filename << ") - "
                  << (availableMaps[i].isValid ? "VÁLIDO" : "INVÁLIDO")
                  << (availableMaps[i].isDefault ? " [DEFECTO]" : "") << std::endl;
    }
    std::cout << "Total: " << availableMaps.size() << " mapas" << std::endl;
    std::cout << "========================\n" << std::endl;
    
    std::string selectedMapPath = MAP_PATH; // Por defecto

    // NUEVO: Variables de estado de las pantallas
    bool mostrandoIntro = true;
    bool mostrandoSelector = false;

    // VENTANA MÁS GRANDE Y RESPONSIVE
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                        "HexEscape: Fabrica de Rompecabezas Elite",
                        Style::Titlebar | Style::Close | Style::Resize);
    window.setFramerateLimit(60);

    // Cargar la fuente para el texto
    Font font;
    if (!font.loadFromFile(FONT_PATH))
    {
        std::cout << "Error: No se pudo cargar la fuente " << FONT_PATH << std::endl;
        return 1;
    }
    Texture icon;
	if (!icon.loadFromFile(ICON_PATH))
	{
		std::cout << "Error: No se pudo cargar el icono " << ICON_PATH << std::endl;
		return 1;
	}

    // MODIFICADO: Variables que se inicializarán después de seleccionar el mapa
    HexGrid* grid = nullptr;
    HexCell* start = nullptr;
    HexCell* goal = nullptr;
    Player* player = nullptr;

    // Crear elementos gráficos
    Text texto = createText(font, 16, Color::White);
    CircleShape hexagon = createHexagon();

    // Relojes para animaciones ultra precisas
    Clock animationClock;
    Clock backgroundClock;
    Clock victoryClock;

    // Variables de estado del juego
    bool gameWon = false;
    bool showVictoryScreen = false;
    std::vector<std::pair<int, int>> pathCells; // Vector para mostrar el camino

    // VARIABLES PARA LOS MODOS:
    bool showPathVisualization = false;  // Para mostrar el camino sin ejecutar
    bool autoSolveMode = false;          // Para activar resolución automática

    // NUEVAS VARIABLES PARA MEJOR CONTROL:
    int lastPlayerRow = -1;
    int lastPlayerCol = -1;
    int lastTurnCount = 0;
    bool pathNeedsUpdate = false;

    // Bucle principal del juego ultra optimizado
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            // Manejar redimensionamiento de ventana
            if (event.type == Event::Resized)
            {
                FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                window.setView(View(visibleArea));
            }

            if (event.type == Event::KeyPressed)
            {
                // PANTALLA DE INTRODUCCIÓN
                if (mostrandoIntro)
                {
                    if (event.key.code == Keyboard::Space)
                    {
                        mostrandoIntro = false;
                        mostrandoSelector = true; // NUEVO: Ir al selector
                        std::cout << "=== MOSTRANDO SELECTOR DE MAPAS ===" << std::endl;
                    }
                    else if (event.key.code == Keyboard::Escape)
                    {
                        window.close();
                    }
                    continue; // No procesar otros inputs durante la intro
                }
                
                // NUEVO: SELECTOR DE MAPAS CORREGIDO
                else if (mostrandoSelector)
                {
                    // DEBUG: Mostrar qué tecla específica se presionó
                    std::cout << "TECLA PRESIONADA: " << event.key.code << std::endl;
                    std::cout << "ENTER = " << Keyboard::Enter << ", ESC = " << Keyboard::Escape << std::endl;
                    std::cout << "W = " << Keyboard::W << ", S = " << Keyboard::S << std::endl;
                    std::cout << "- Número de mapas: " << availableMaps.size() << std::endl;
                    std::cout << "- Índice seleccionado: " << selectedMapIndex << std::endl;
                    
                    if (event.key.code == Keyboard::Escape)
                    {
                        std::cout << "=== ESC DETECTADO ===" << std::endl;
                        selectedMapPath = MAP_PATH;
                        mostrandoSelector = false;
                        
                        if (loadSelectedMap(selectedMapPath, grid, start, goal, player)) {
                            std::cout << "=== JUEGO INICIADO CON MAPA POR DEFECTO ===" << std::endl;
                            animationClock.restart();
                            backgroundClock.restart();
                        } else {
                            std::cout << "Error crítico: No se puede cargar el mapa por defecto" << std::endl;
                            window.close();
                        }
                    }
                    else if (event.key.code == Keyboard::Return || event.key.code == Keyboard::Enter || event.key.code == 57)
                    {
                        std::cout << "=== ENTER/RETURN/SPACE DETECTADO (código: " << event.key.code << ") ===" << std::endl;
                        
                        // FORZAR SELECCIÓN - Versión simplificada
                        if (selectedMapIndex >= 0 && selectedMapIndex < static_cast<int>(availableMaps.size())) {
                            selectedMapPath = availableMaps[selectedMapIndex].filename;
                            std::cout << "Seleccionando: " << selectedMapPath << std::endl;
                        } else {
                            selectedMapPath = MAP_PATH;
                            std::cout << "Usando mapa por defecto: " << selectedMapPath << std::endl;
                        }
                        
                        mostrandoSelector = false;
                        
                        if (loadSelectedMap(selectedMapPath, grid, start, goal, player)) {
                            std::cout << "=== ÉXITO: JUEGO INICIADO ===" << std::endl;
                            animationClock.restart();
                            backgroundClock.restart();
                        } else {
                            std::cout << "=== ERROR: No se pudo cargar ===" << std::endl;
                            if (selectedMapPath != MAP_PATH) {
                                if (loadSelectedMap(MAP_PATH, grid, start, goal, player)) {
                                    std::cout << "=== ÉXITO CON MAPA POR DEFECTO ===" << std::endl;
                                    animationClock.restart();
                                    backgroundClock.restart();
                                } else {
                                    window.close();
                                }
                            } else {
                                window.close();
                            }
                        }
                    }
                    else if (event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
                    {
                        std::cout << "=== W/UP DETECTADO ===" << std::endl;
                        if (selectedMapIndex > 0) {
                            selectedMapIndex--;
                            std::cout << "Nuevo índice: " << selectedMapIndex << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
                    {
                        std::cout << "=== S/DOWN DETECTADO ===" << std::endl;
                        if (selectedMapIndex < static_cast<int>(availableMaps.size()) - 1) {
                            selectedMapIndex++;
                            std::cout << "Nuevo índice: " << selectedMapIndex << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::F5)
                    {
                        std::cout << "=== F5 DETECTADO - REESCANEANDO ===" << std::endl;
                        scanMapFiles();
                        if (selectedMapIndex >= static_cast<int>(availableMaps.size())) {
                            selectedMapIndex = std::max(0, static_cast<int>(availableMaps.size()) - 1);
                        }
                    }
                    else 
                    {
                        std::cout << "=== TECLA NO RECONOCIDA: " << event.key.code << " ===" << std::endl;
                    }
                    continue;
                }

                // TU LÓGICA EXISTENTE DEL JUEGO (solo después de seleccionar mapa)
                else if (grid && player && start && goal)
                {
                    // Si ya ganó, solo permitir salir con ESC
                    if (event.key.code == Keyboard::Escape)
                    {
                        if (showVictoryScreen)
                        {
                            window.close();
                        }
                        else
                        {
                            // ESC cancela todo y vuelve al modo manual
                            std::cout << "=== CANCELANDO TODOS LOS MODOS - VOLVIENDO A MANUAL ===" << std::endl;
                            showPathVisualization = false;
                            autoSolveMode = false;
                            player->isAutoMoving = false;
                            pathCells.clear();
                            lastPlayerRow = -1;
                            lastPlayerCol = -1;
                            std::cout << "Modo manual activado. Usa P para mostrar camino, R para auto-resolución." << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::P && !showVictoryScreen && !autoSolveMode)
                    {
                        // MODO VISUALIZACIÓN: Solo mostrar el camino
                        std::cout << "=== MODO VISUALIZACIÓN ACTIVADO ===" << std::endl;
                        PathfindingResult path = findPath(*grid, player->row, player->col, goal->row, goal->col, player->energy);
                       
                        if (path.success && !path.path.empty()) {
                            pathCells.clear();
                            for (auto *cell : path.path)
                            {
                                pathCells.emplace_back(cell->row, cell->col);
                            }
                           
                            showPathVisualization = true;
                            autoSolveMode = false;
                            player->isAutoMoving = false;
                           
                            // Guardar posición del jugador
                            lastPlayerRow = player->row;
                            lastPlayerCol = player->col;
                           
                            std::cout << "Camino mostrado con " << pathCells.size() << " pasos. Presiona T para ejecutar." << std::endl;
                        } else {
                            // No hay camino disponible
                            pathCells.clear();
                            showPathVisualization = false;
                            lastPlayerRow = -1;
                            lastPlayerCol = -1;
                            std::cout << "No se encontró camino hacia la meta." << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::R && !showVictoryScreen)
                    {
                        // MODO AUTO-RESOLUCIÓN DIRECTA
                        std::cout << "=== MODO AUTO-RESOLUCIÓN ACTIVADO ===" << std::endl;
                        PathfindingResult path = findPath(*grid, player->row, player->col, goal->row, goal->col, player->energy);
                       
                        if (path.success && !path.path.empty()) {
                            pathCells.clear();
                            for (auto *cell : path.path)
                            {
                                pathCells.emplace_back(cell->row, cell->col);
                            }
                           
                            autoSolveMode = true;
                            player->isAutoMoving = true;
                            showPathVisualization = false; // Modo directo sin visualización
                           
                            lastPlayerRow = player->row;
                            lastPlayerCol = player->col;
                           
                            std::cout << "Auto-resolución iniciada con " << pathCells.size() << " pasos. El jugador se moverá automáticamente." << std::endl;
                        } else {
                            std::cout << "No se encontró camino para auto-resolución." << std::endl;
                        }
                    }
                    else if (event.key.code == Keyboard::T && showPathVisualization && !autoSolveMode && !showVictoryScreen)
                    {
                        // EJECUTAR EL CAMINO YA MOSTRADO
                        if (!pathCells.empty()) {
                            std::cout << "=== EJECUTANDO CAMINO MOSTRADO ===" << std::endl;
                            autoSolveMode = true;
                            player->isAutoMoving = true;
                            // Mantener showPathVisualization = true para seguir viendo el camino
                           
                            std::cout << "Ejecutando camino con " << pathCells.size() << " pasos. El camino permanecerá visible." << std::endl;
                        }
                    }
                    else if (!showVictoryScreen && !autoSolveMode && !player->isAutoMoving)
                    {
                        // MODO MANUAL: Solo permitir movimiento manual si NO está en auto-resolución
                       
                        // Recordar posición antes del movimiento
                        int oldRow = player->row;
                        int oldCol = player->col;
                       
                        handlePlayerMovement(event.key.code, *player, *grid);
                       
                        // Si el jugador se movió manualmente, limpiar visualización
                        if (player->row != oldRow || player->col != oldCol) {
                            if (showPathVisualization) {
                                pathCells.clear();
                                showPathVisualization = false;
                                lastPlayerRow = -1;
                                lastPlayerCol = -1;
                            }
                        }
                    }
                }
            }
        }

        // === LÓGICA DEL JUEGO (solo si está todo cargado) ===
        if (!mostrandoIntro && !mostrandoSelector && grid && player && goal)
        {
            // DETECCIÓN DE CAMBIOS DE POSICIÓN DEL JUGADOR (por bandas transportadoras)
            if (!autoSolveMode && showPathVisualization) {
                if (player->row != lastPlayerRow || player->col != lastPlayerCol) {
                    // El jugador se movió desde que se calculó el camino
                    // Verificar si el camino sigue siendo válido
                    if (!isPathStillValid(*grid, pathCells, player->row, player->col)) {
                        pathCells.clear();
                        showPathVisualization = false;
                        lastPlayerRow = -1;
                        lastPlayerCol = -1;
                    }
                }
            }

            // DETECCIÓN DE NUEVAS PAREDES
            int currentTurnCount = TurnSystem::getCurrentTurnCount();
            if (currentTurnCount != lastTurnCount) {
                lastTurnCount = currentTurnCount;
                pathNeedsUpdate = true;
            }

            // VERIFICACIÓN Y RECÁLCULO DE CAMINOS
            if (pathNeedsUpdate && !pathCells.empty() && (showPathVisualization || autoSolveMode)) {
                pathNeedsUpdate = false;
               
                if (!isPathStillValid(*grid, pathCells, player->row, player->col)) {
                    // El camino está bloqueado, intentar recalcular
                    bool wasExecuting = autoSolveMode;
                   
                    if (!recalculatePath(*grid, *player, goal, pathCells, showPathVisualization, autoSolveMode)) {
                        // No se pudo recalcular, detener todo
                        if (wasExecuting) {
                            autoSolveMode = false;
                            player->isAutoMoving = false;
                        }
                    }
                }
            }

            // ACTUALIZAR AUTO-MOVIMIENTO
            if (autoSolveMode && player->isAutoMoving) {
                core::updateAutoMovement(*grid, *player, pathCells, goal->row, goal->col);
               
                // Si terminó el auto-movimiento, actualizar estado
                if (!player->isAutoMoving) {
                    std::cout << "=== AUTO-MOVIMIENTO COMPLETADO ===" << std::endl;
                    autoSolveMode = false;
                   
                    // Si no estaba en modo visualización, limpiar el camino
                    if (!showPathVisualization) {
                        pathCells.clear();
                        std::cout << "Camino limpiado. Volviendo a modo manual." << std::endl;
                    } else {
                        std::cout << "Camino mantenido visible. Presiona ESC para limpiar." << std::endl;
                    }
                   
                    lastPlayerRow = player->row;
                    lastPlayerCol = player->col;
                }
            }

            // VERIFICAR CONDICIÓN DE VICTORIA
            if (!gameWon && player->hasWon)
            {
                gameWon = true;
                showVictoryScreen = true;
                autoSolveMode = false;
                showPathVisualization = false;
                player->isAutoMoving = false;
                pathCells.clear();
                lastPlayerRow = -1;
                lastPlayerCol = -1;
                victoryClock.restart();
            }

            // APLICAR EFECTOS DE BANDAS TRANSPORTADORAS
            if (!showVictoryScreen)
            {
                handleConveyorMovement(*player, *grid);
            }
        }

        // RENDERIZAR
        window.clear(Color(5, 10, 20));

        if (mostrandoIntro)
        {
            // PANTALLA DE INTRODUCCIÓN
            mostrarIntro(window, font);
        }
        else if (mostrandoSelector) // NUEVO
        {
            // SELECTOR DE MAPAS
            mostrarSelectorMapas(window, font, icon);
            if (event.key.code == Keyboard::P) {
				selectedMapPath = Clipboard::getString();
                mostrandoSelector = false;
                if (loadSelectedMap(selectedMapPath, grid, start, goal, player)) {
                    std::cout << "=== ÉXITO: JUEGO INICIADO ===" << std::endl;
                    animationClock.restart();
                    backgroundClock.restart();
                } else {
                    std::cout << "=== ERROR: No se pudo cargar ===" << std::endl;
                    if (selectedMapPath != MAP_PATH) {
                        if (loadSelectedMap(MAP_PATH, grid, start, goal, player)) {
                            std::cout << "=== ÉXITO CON MAPA POR DEFECTO ===" << std::endl;
                            animationClock.restart();
                            backgroundClock.restart();
                        }
                        else {
                            window.close();
                        }
                    }
                    else {
                        window.close();
                    }
                }
            }
        }
        else if (showVictoryScreen && grid && player)
        {
            // PANTALLA DE VICTORIA
            drawVictoryScreen(window, font, player->winTime,
                              TurnSystem::getCurrentTurnCount(), victoryClock);
        }
        else if (grid && player) // MODIFICADO: Solo dibujar si está cargado
        {
            // JUEGO NORMAL
            drawGrid(window, *grid, *player, hexagon, texto, font, animationClock, backgroundClock, pathCells);

            // Dibujar UI
            drawModernEnergyBar(window, *player, font, animationClock);
            drawGameInfo(window, font, TurnSystem::getCurrentTurnCount(), animationClock,
                        showPathVisualization, autoSolveMode);
            drawModernControls(window, font, animationClock);
        }

        window.display();
    }

    // NUEVO: Limpiar memoria
    if (grid) {
        delete grid;
        grid = nullptr;
    }
    if (player) {
        delete player;
        player = nullptr;
    }

    std::cout << "=== FINALIZANDO HEXESCAPE ===" << std::endl;
    return 0;
}