// MapLoader.cpp
#include "MapLoader.hpp"


std::vector<std::vector<HexCell>> loadMapFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    std::vector<std::vector<HexCell>> grid;

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filepath << "\n";
        return grid;
    }

    std::string line;
    int row = 0;

    while (std::getline(file, line)) {
        std::vector<HexCell> currentRow;
        for (int col = 0; col < line.length(); ++col) {
            char c = line[col];

            CellType type = CellType::EMPTY;

            switch (c) {
            case 'S': type = CellType::START; break;
            case 'G': case 'g': type = CellType::GOAL; break;
            case '#': type = CellType::WALL; break;
            case 'K': case 'k': type = CellType::ITEM; break;
            case 'A': type = CellType::UP_RIGHT; break;
            case 'B': type = CellType::RIGHT; break;
            case 'C': type = CellType::DOWN_RIGHT; break;
            case 'D': type = CellType::DOWN_LEFT; break;
            case 'E': type = CellType::LEFT; break;
            case 'F': type = CellType::UP_LEFT; break;
            case '.': default: type = CellType::EMPTY; break;
            }


            currentRow.emplace_back(row, col, type);
        }
        grid.push_back(currentRow);
        row++;
    }

    return grid;
}


std::string CellTypeToString(CellType type) {
	switch (type) {
	case CellType::EMPTY: return " ";
	case CellType::WALL: return "#";
	case CellType::START: return "S";
	case CellType::GOAL: return "G";
	case CellType::ITEM: return "K";
	case CellType::UP_RIGHT: return "A";
	case CellType::RIGHT: return "B";
	case CellType::DOWN_RIGHT: return "C";
	case CellType::DOWN_LEFT: return "D";
	case CellType::LEFT: return "E";
	case CellType::UP_LEFT: return "F";
	default: return "?"; // Para tipos desconocidos
	}
}