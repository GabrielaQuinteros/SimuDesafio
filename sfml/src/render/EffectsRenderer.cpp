#include "EffectsRenderer.hpp"

using namespace sf;

Color lerpColor(const Color& a, const Color& b, float t) {
    return Color(
        static_cast<Uint8>(a.r + (b.r - a.r) * t),
        static_cast<Uint8>(a.g + (b.g - a.g) * t),
        static_cast<Uint8>(a.b + (b.b - a.b) * t),
        static_cast<Uint8>(a.a + (b.a - a.a) * t)
    );
}

void drawDecorativeHex(RenderWindow& window, Vector2f position, float size, Color color, float time, bool rotating) {
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