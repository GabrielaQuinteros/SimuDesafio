#pragma once


namespace model
{
    enum class CellType
    {
        EMPTY,
        WALL,
        START,
        GOAL,
        ITEM,

        UP_RIGHT,
        UP_LEFT,
        RIGHT,
        LEFT,
        DOWN_RIGHT,
        DOWN_LEFT,
    };


    struct HexCell
    {
        int row, col;
        CellType type;

        bool hasPlayer = false;


        HexCell(int r, int c, CellType t) 
            : row(r), col(c), type(t) {}
    };
}
