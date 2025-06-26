#include "HexGrid.hpp"
#include <cassert>    


using namespace model;

HexGrid::HexGrid(int rows, int cols)
    : m_rows(rows), m_cols(cols), m_cells(rows, std::vector<HexCell>()) 
{
    
    for (int r = 0; r < m_rows; ++r)
    {
        m_cells[r].reserve(m_cols);
    
        for (int c = 0; c < m_cols; ++c)
        {
            
            m_cells[r].emplace_back(r, c, CellType::EMPTY);
        }
    }
}


const HexCell &HexGrid::at(int row, int col) const
{
    assert(row >= 0 && row < m_rows && col >= 0 && col < m_cols);
    return m_cells[row][col]; 
}

HexCell &HexGrid::at(int row, int col)
{
    assert(row >= 0 && row < m_rows && col >= 0 && col < m_cols);
    return m_cells[row][col]; 
}

std::vector<HexCell*> HexGrid::neighbors(const HexCell& cell)
{
    static const int dRow[6] = { 0, 1, 1, 0, -1, -1 };


    static const int dColEven[6] = { 1, 0, -1, -1, -1, 0 }; 
    static const int dColOdd[6] = { 1, 1, 0, -1, 0, 1 };   


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
sf::Vector2f HexGrid::toPixel(int row, int col) const
{
    const float baseX = (row % 2 == 0) ? 50.f : 75.f;
    const float x = baseX + col * 50.f; 
    const float y = 50.f + row * 40.f; 

    return {x, y}; 
}
