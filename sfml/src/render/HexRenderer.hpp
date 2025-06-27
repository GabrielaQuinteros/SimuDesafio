#ifndef HEX_RENDERER_HPP
#define HEX_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include "model/HexGrid.hpp"
#include "model/Player.hpp"
#include <vector>

using namespace sf;
using namespace model;

CircleShape createHexagon();

Color getCellColor(CellType type, Clock& animClock);

void drawGrid(RenderWindow& window, const HexGrid& grid,
    Player& player, CircleShape& hexagon,
    Text& text, Font& font, Clock& animClock, Clock& bgClock,
    std::vector<std::pair<int, int>>& pathCells);

#endif