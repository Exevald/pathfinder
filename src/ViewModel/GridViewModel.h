#pragma once

#include "../Service/FileLoader.h"
#include "../Model/Grid.h"
#include <memory>
#include <string>

class GridViewModel
{
public:
	GridViewModel();

	void LoadData(const std::string& filePath);
	[[nodiscard]] Grid* GetGrid() const;

private:
	FileLoader m_fileLoader;
	std::unique_ptr<Grid> m_grid;
};