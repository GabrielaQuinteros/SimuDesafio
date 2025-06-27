#ifndef MAPSELECTOR_HPP
#define MAPSELECTOR_HPP

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

struct MapInfo {
    std::string filename;
    std::string displayName;
    bool isDefault;
    bool isValid;
};

extern std::vector<MapInfo> availableMaps;
extern int selectedMapIndex;
extern std::string mappath;

bool isValidMapFile(const std::string& filepath);
void scanMapFiles();
std::string getCurrentSelectedMap();
bool isCurrentSelectionValid();
void drawClipboardIcon(sf::RenderWindow& window, float x, float y, float scale = 1.0f);
void mostrarSelectorMapas(sf::RenderWindow& window, sf::Font& font);

#endif 