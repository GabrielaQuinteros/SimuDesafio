#pragma once


#include <vector>
#include <SFML/System.hpp>
#include "HexCell.hpp"


namespace model
{


   class HexGrid
   {
   public:
       HexGrid(int rows, int cols);


       const HexCell &at(int row, int col) const;
       HexCell &at(int row, int col);


       std::vector<HexCell *> neighbors(const HexCell &cell);


       sf::Vector2f toPixel(int row, int col) const;


       int rows() const { return m_rows; }
       int cols() const { return m_cols; }

       bool inBounds(int row, int col) const {
           return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
       };


   private:
       int m_rows;
       int m_cols;
       std::vector<std::vector<HexCell>> m_cells;
   };


}
