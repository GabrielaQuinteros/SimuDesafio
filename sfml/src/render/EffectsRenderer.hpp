#ifndef EFFECTS_RENDERER_HPP
#define EFFECTS_RENDERER_HPP

#include <SFML/Graphics.hpp>

using namespace sf;

Color lerpColor(const Color& a, const Color& b, float t);

void drawDecorativeHex(RenderWindow& window, Vector2f position, float size, Color color, float time, bool rotating = true);

#endif