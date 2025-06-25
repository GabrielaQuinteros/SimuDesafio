#include "HexGrid.hpp"
#include <cassert>    


using namespace model;


// Constructor: inicializa un grid hexagonal con el número de filas y columnas especificado
HexGrid::HexGrid(int rows, int cols)
    : m_rows(rows), m_cols(cols), m_cells(rows, std::vector<HexCell>()) // Inicializa las dimensiones del grid y el contenedor de celdas
{
    // Itera sobre cada fila del grid
    for (int r = 0; r < m_rows; ++r)
    {
        m_cells[r].reserve(m_cols); // Reserva espacio para las columnas en la fila actual
        // Itera sobre cada columna de la fila actual
        for (int c = 0; c < m_cols; ++c)
        {
            // Crea una nueva celda HexCell en la posición (r, c) con el tipo inicial EMPTY
            m_cells[r].emplace_back(r, c, CellType::EMPTY);
        }
    }
}


// Método para acceder a una celda específica del grid
// Devuelve una referencia constante a la celda en la posición (row, col)
const HexCell &HexGrid::at(int row, int col) const
{
    // Verifica que las coordenadas estén dentro de los límites del grid
    assert(row >= 0 && row < m_rows && col >= 0 && col < m_cols);
    return m_cells[row][col]; // Devuelve la celda correspondiente
}


// Método para acceder a una celda específica del grid (versión no constante)
// Devuelve una referencia modificable a la celda en la posición (row, col)
HexCell &HexGrid::at(int row, int col)
{
    // Verifica que las coordenadas estén dentro de los límites del grid
    assert(row >= 0 && row < m_rows && col >= 0 && col < m_cols);
    return m_cells[row][col]; // Devuelve la celda correspondiente
}


// Método para obtener los vecinos de una celda específica
std::vector<HexCell*> HexGrid::neighbors(const HexCell& cell)
{
    // Dirección: E, SE, SW, W, NW, NE (en sentido horario desde la derecha)
    static const int dRow[6] = { 0, 1, 1, 0, -1, -1 };


    static const int dColEven[6] = { 1, 0, -1, -1, -1, 0 }; // filas pares
    static const int dColOdd[6] = { 1, 1, 0, -1, 0, 1 };   // filas impares


    std::vector<HexCell*> result;
    int r = cell.row, c = cell.col;
    bool odd = r % 2 != 0;


    for (int i = 0; i < 6; ++i) {
        int nr = r + dRow[i];
        int nc = c + (odd ? dColOdd[i] : dColEven[i]);


        if (nr >= 0 && nr < m_rows && nc >= 0 && nc < m_cols) {
            result.push_back(&m_cells[nr][nc]);
        }
    }
    return result;
}


// Método para convertir las coordenadas del grid a coordenadas de píxeles
sf::Vector2f HexGrid::toPixel(int row, int col) const
{
    // Calcula la posición base en el eje X dependiendo de si la fila es par o impar
    const float baseX = (row % 2 == 0) ? 50.f : 75.f;
    const float x = baseX + col * 50.f; // Calcula la posición X en píxeles
    const float y = 50.f + row * 40.f;  // Calcula la posición Y en píxeles


    return {x, y}; // Devuelve las coordenadas de píxeles como un vector 2D
}
