#pragma once


namespace model
{


    //Tipos de celdas
    enum class CellType
    {
        EMPTY, //Celda común
        WALL,
        START,
        GOAL,
        ITEM,
        //Bandas transportadoras
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
        //Verifica si el jugador estás obre la celda
        bool hasPlayer = false;


        HexCell(int r, int c, CellType t) //Fila, columna y tipo de celda
            : row(r), col(c), type(t) {}
    };
}
