// HexCell.hpp
#pragma once

enum class CellType {
    EMPTY,
    WALL,
    START,
    GOAL,
    ITEM,
	//Las siguientes son de bandas transportadoras
    UP_RIGHT, UP_LEFT,
    RIGHT, LEFT, 
    DOWN_RIGHT, DOWN_LEFT,
};

struct HexCell {
    int row, col; // Coordenadas en la grilla tipo offset

    CellType type;
    bool hasPlayer = false;

	HexCell(int r, int c, CellType t)
		: row(r), col(c), type(t) {
	}


};