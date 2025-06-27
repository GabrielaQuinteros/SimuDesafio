#include "HexRenderer.hpp"
#include "ScreenRenderer.hpp"
#include "utils/MapLoader.hpp"
#include <cmath>

using namespace model;
using namespace sf;

const Color NEON_BLUE = Color(0, 200, 255);
const Color NEON_GREEN = Color(0, 255, 150);
const Color NEON_PURPLE = Color(180, 100, 255);
const Color NEON_ORANGE = Color(255, 150, 0);
const Color ELECTRIC_YELLOW = Color(255, 255, 0);
const Color CYBER_WHITE = Color(240, 240, 255);
const Color TECH_GRAY = Color(70, 90, 120);

CircleShape createHexagon()
{
    CircleShape hexagon(25, 6);
    hexagon.setOutlineColor(Color(80, 80, 80));
    hexagon.setFillColor(Color::White);
    hexagon.setOrigin(25, 25);
    hexagon.setOutlineThickness(2);
    return hexagon;
}

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

void drawGrid(RenderWindow& window, const HexGrid& grid,
    Player& player, CircleShape& hexagon,
    Text& text, Font& font, Clock& animClock, Clock& bgClock,
    std::vector<std::pair<int, int>>& pathCells)
{
    drawAnimatedBackground(window, bgClock);

    drawGameTitle(window, font, animClock);

    player.updateMovement();

    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    
    float availableWidth = windowWidth - 400;
    float availableHeight = windowHeight - 200;
    
    float hexSizeByWidth = availableWidth / (grid.cols() * 1.5f + 0.5f);
    float hexSizeByHeight = availableHeight / (grid.rows() * sqrt(3.0f));
    
    float hexSize = std::min(hexSizeByWidth, hexSizeByHeight);
    
    hexSize = std::max(8.0f, std::min(hexSize, 25.0f));
    
    float hexSpacingX = hexSize * 1.5f;             
    float hexSpacingY = hexSize * sqrt(3.0f);       
    
    float gridWidth = (grid.cols() - 1) * hexSpacingX + hexSize * 2;
    float gridHeight = (grid.rows() - 1) * hexSpacingY + hexSize * 2;
    float offsetX = (availableWidth - gridWidth) / 2.0f + 50.0f;
    float offsetY = (availableHeight - gridHeight) / 2.0f + 100.0f;

    float time = animClock.getElapsedTime().asSeconds();

    CircleShape dynamicHex(hexSize, 6);
    dynamicHex.setOrigin(hexSize, hexSize);
    
    int textSize = static_cast<int>(hexSize * 0.4f);
    textSize = std::max(6, std::min(textSize, 12));

    for (int y = 0; y < grid.rows(); ++y) {
        for (int x = 0; x < grid.cols(); ++x) {
            const HexCell& cell = grid.at(y, x);
            
            float posX = offsetX + x * hexSpacingX + (y % 2 == 1 ? hexSpacingX * 0.5f : 0);
            float posY = offsetY + y * hexSpacingY;
            Vector2f pos(posX, posY);

            bool isInPath = false;
            for (const auto& pathCell : pathCells) {
                if (pathCell.first == y && pathCell.second == x) {
                    isInPath = true;
                    break;
                }
            }

            if (!isInPath && hexSize > 10) {
                CircleShape shadow = dynamicHex;
                shadow.setPosition(pos.x + 1, pos.y + 1);
                shadow.setFillColor(Color(0, 0, 0, 60));
                shadow.setOutlineThickness(0);
                window.draw(shadow);
            }

            dynamicHex.setPosition(pos);
            dynamicHex.setFillColor(getCellColor(cell.type, animClock));
            dynamicHex.setOutlineColor(Color(120, 140, 160));
            dynamicHex.setOutlineThickness(std::max(0.5f, hexSize * 0.03f));

            window.draw(dynamicHex);

            if (!isInPath && hexSize > 10) {
                text.setString(CellTypeToString(cell.type));
                text.setCharacterSize(textSize);
                text.setStyle(Text::Bold);
                text.setFillColor(Color(30, 30, 50));
                text.setPosition(pos.x - hexSize * 0.2f, pos.y - hexSize * 0.2f);
                window.draw(text);
            }
        }
    }

    if (!pathCells.empty()) {
        for (size_t i = 0; i < pathCells.size(); ++i) {
            const std::pair<int, int>& pathCell = pathCells[i];
            
            float posX = offsetX + pathCell.second * hexSpacingX + (pathCell.first % 2 == 1 ? hexSpacingX * 0.5f : 0);
            float posY = offsetY + pathCell.first * hexSpacingY;
            Vector2f pos(posX, posY);
           
            float pathOuterSize = hexSize + 2;
            float pathInnerSize = hexSize - 1;
           
            CircleShape pathHexOuter(pathOuterSize, 6);
            pathHexOuter.setOrigin(pathOuterSize, pathOuterSize);
            pathHexOuter.setPosition(pos);
            pathHexOuter.setFillColor(Color::Transparent);
            pathHexOuter.setOutlineColor(Color(255, 0, 0, 255));
            pathHexOuter.setOutlineThickness(std::max(1.0f, hexSize * 0.1f));
            window.draw(pathHexOuter);
           
            CircleShape pathHexInner(pathInnerSize, 6);
            pathHexInner.setOrigin(pathInnerSize, pathInnerSize);
            pathHexInner.setPosition(pos);
            pathHexInner.setFillColor(Color(255, 100, 100, 150));
            pathHexInner.setOutlineColor(Color(255, 255, 255));
            pathHexInner.setOutlineThickness(1);
            window.draw(pathHexInner);
        }
       
        if (hexSize > 12) {
            for (size_t i = 0; i < pathCells.size(); ++i) {
                const std::pair<int, int>& pathCell = pathCells[i];
                
                float posX = offsetX + pathCell.second * hexSpacingX + (pathCell.first % 2 == 1 ? hexSpacingX * 0.5f : 0);
                float posY = offsetY + pathCell.first * hexSpacingY;
                Vector2f pos(posX, posY);
               
                float numberBgSize = hexSize * 0.4f;
                CircleShape numberBg(numberBgSize);
                numberBg.setOrigin(numberBgSize, numberBgSize);
                numberBg.setPosition(pos);
                numberBg.setFillColor(Color(0, 0, 0, 180));
                numberBg.setOutlineColor(Color(255, 255, 255));
                numberBg.setOutlineThickness(1);
                window.draw(numberBg);
               
                Text seqNumber;
                seqNumber.setFont(font);
                seqNumber.setCharacterSize(static_cast<int>(hexSize * 0.5f));
                seqNumber.setStyle(Text::Bold);
                seqNumber.setFillColor(Color::White);
                seqNumber.setString(std::to_string(static_cast<int>(i + 1)));
               
                FloatRect bounds = seqNumber.getLocalBounds();
                seqNumber.setOrigin(bounds.width / 2, bounds.height / 2);
                seqNumber.setPosition(pos);
                window.draw(seqNumber);
            }
        }
    }

    float playerPosX = offsetX + player.col * hexSpacingX + (player.row % 2 == 1 ? hexSpacingX * 0.5f : 0);
    float playerPosY = offsetY + player.row * hexSpacingY;
    
    if (player.isMoving) {
        Vector2f visualPos = player.getVisualPosition(grid);
        float deltaX = (visualPos.x - grid.toPixel(player.row, player.col).x) / 50.0f;
        float deltaY = (visualPos.y - grid.toPixel(player.row, player.col).y) / 40.0f;
        playerPosX += deltaX * hexSpacingX;
        playerPosY += deltaY * hexSpacingY;
    }
    
    Vector2f playerPos(playerPosX, playerPosY);

    float playerSize = hexSize * 0.6f;
    CircleShape playerCircle(playerSize);

    if (hexSize > 8) {
        CircleShape playerShadow(playerSize + 1);
        playerShadow.setPosition(playerPos.x + 1, playerPos.y + 1);
        playerShadow.setFillColor(Color(0, 0, 0, 100));
        playerShadow.setOrigin(playerSize + 1, playerSize + 1);
        window.draw(playerShadow);
    }

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

    playerCircle.setFillColor(playerColor);
    playerCircle.setOutlineColor(CYBER_WHITE);
    playerCircle.setOutlineThickness(std::max(1.0f, hexSize * 0.08f));
    playerCircle.setOrigin(playerSize, playerSize);
    playerCircle.setPosition(playerPos);
    window.draw(playerCircle);

    if (hexSize > 8) {
        float coreSize = playerSize * 0.4f;
        CircleShape playerCore(coreSize, 6);
        playerCore.setFillColor(CYBER_WHITE);
        playerCore.setOrigin(coreSize, coreSize);
        playerCore.setPosition(playerPos);
        window.draw(playerCore);

        if (hexSize > 12) {
            float dotSize = playerSize * 0.15f;
            CircleShape centerDot(dotSize);
            centerDot.setFillColor(Color(50, 50, 80));
            centerDot.setOrigin(dotSize, dotSize);
            centerDot.setPosition(playerPos);
            window.draw(centerDot);
        }
    }
}