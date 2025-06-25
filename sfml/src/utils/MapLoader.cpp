#include "MapLoader.hpp"
#include "model/HexCell.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace model;
using namespace std;

HexGrid loadHexGridFromFile(const string &filepath)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Error al abrir el archivo: " << filepath << "\n";
        return HexGrid(0, 0);
    }

    vector<string> lines;
    string line;

    while (getline(file, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    if (lines.empty()) {
        cerr << "El archivo está vacío: " << filepath << "\n";
        return HexGrid(0, 0);
    }

    int rows = lines.size();
    
    // Encontrar la longitud máxima de todas las filas
    int maxCols = 0;
    for (const auto& row : lines) {
        maxCols = max(maxCols, static_cast<int>(row.size()));
    }
    
    HexGrid grid(rows, maxCols);

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < maxCols; ++c)
        {
            char ch = '.'; // Valor por defecto si la fila es más corta
            
            // Solo acceder al carácter si existe en esta fila
            if (c < static_cast<int>(lines[r].size())) {
                ch = lines[r][c];
            }
            
            CellType type;

            switch (ch)
            {
            case 'S':
                type = CellType::START;
                break;
            case 'G':
                type = CellType::GOAL;
                break;
            case '#':
                type = CellType::WALL;
                break;
            case 'K':
                type = CellType::ITEM;
                break;
            case 'A':
                type = CellType::UP_RIGHT;
                break;
            case 'B':
                type = CellType::RIGHT;
                break;
            case 'C':
                type = CellType::DOWN_RIGHT;
                break;
            case 'D':
                type = CellType::DOWN_LEFT;
                break;
            case 'E':
                type = CellType::LEFT;
                break;
            case 'F':
                type = CellType::UP_LEFT;
                break;
            default:
                type = CellType::EMPTY;
                break;
            }

            grid.at(r, c).type = type;
        }
    }

    return grid;
}

string CellTypeToString(CellType type)
{
    using model::CellType;
    switch (type)
    {
    case CellType::EMPTY:
        return " ";
    case CellType::WALL:
        return "#";
    case CellType::START:
        return "S";
    case CellType::GOAL:
        return "G";
    case CellType::ITEM:
        return "K";
    case CellType::UP_RIGHT:
        return "A";
    case CellType::RIGHT:
        return "B";
    case CellType::DOWN_RIGHT:
        return "C";
    case CellType::DOWN_LEFT:
        return "D";
    case CellType::LEFT:
        return "E";
    case CellType::UP_LEFT:
        return "F";
    default:
        return "?";
    }
}