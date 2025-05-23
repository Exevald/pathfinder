#pragma once

#include "CFileLoader.h"
#include "Grid.h"
#include <memory>
#include <string>

class CGridViewModel
{
public:
	CGridViewModel();

	void LoadData(const std::string& filePath);
	[[nodiscard]] Grid* GetGrid() const;

private:
	CFileLoader m_fileLoader;
	std::unique_ptr<Grid> m_grid;
};