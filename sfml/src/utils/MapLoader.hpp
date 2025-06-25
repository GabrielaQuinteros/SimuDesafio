#pragma once


#include <string>
#include "model/HexGrid.hpp"


model::HexGrid loadHexGridFromFile(const std::string &filepath);
std::string CellTypeToString(model::CellType type);


