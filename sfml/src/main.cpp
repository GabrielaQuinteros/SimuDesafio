#include <SFML/Graphics.hpp>
#include "MapLoader.hpp"
#include "Utils.hpp"
#include "HexCell.hpp"
#include <vector>
#include <iostream>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define HEXAGON_RADIUS 25
#define HEXAGON_OUTLINE_THICKNESS 4

#define MAP_PATH "resources/map.txt"
#define FONT_PATH "resources/arial.ttf"

int main()
{
	// Inicializar las celdas del grid
	std::vector<std::vector<HexCell>> grid = loadMapFromFile(MAP_PATH);


	// Crear un objeto de tipo sf::CircleShape para representar un hexágono
	sf::CircleShape hexagon(HEXAGON_RADIUS, 6);
	hexagon.setOutlineColor(sf::Color::Black);
	hexagon.setFillColor(sf::Color::White);
	hexagon.setOrigin(HEXAGON_RADIUS, HEXAGON_RADIUS);
	hexagon.setOutlineThickness(HEXAGON_OUTLINE_THICKNESS);
	sf::Color color[2] = { sf::Color(220,220,220), sf::Color(192,192,192) };

	// Crear una ventana de SFML
	sf::RenderWindow window({ WINDOW_WIDTH, WINDOW_HEIGHT }, "Hexagons", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(0, 0, 8));

	sf::Font font;
	if (!font.loadFromFile(FONT_PATH)) {
		std::cerr << "No se pudo cargar la fuente.\n";
	}
	sf::Text texto = createText(font, 16, sf::Color::Black);

	// Ciclo principal
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

		}


		// Logica del juego



		// Desde aqui, logica del dibujo
		window.clear(sf::Color::White);


		// Dibujar el grid de hexágonos
		for (int y = 0; y <= grid.size() - 1; ++y) {
			for (int x = 0; x <= grid[y].size() - 1; ++x) {

				// Ajustar la posición del hexágono según las coordenadas x, y
				hexagon.setPosition((y % 2 ? 75 : 50) + x * 50.f, 50 + y * 40.f);
				hexagon.setFillColor(color[y % 2]);
				window.draw(hexagon);

				texto.setString(CellTypeToString(grid[y][x].type));
				texto.setPosition((y % 2 ? 65 : 40) + x * 50.f, 45 + y * 40.f);
				window.draw(texto);
			}
		}

		window.display();

	}

}



