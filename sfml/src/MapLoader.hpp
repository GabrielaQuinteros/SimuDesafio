// MapLoader.hpp
#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "HexCell.hpp"

std::vector<std::vector<HexCell>> loadMapFromFile(const std::string& filepath);

std::string CellTypeToString(CellType type);