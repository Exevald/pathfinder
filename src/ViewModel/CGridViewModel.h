#pragma once

#include "CFileLoader.h"
#include "CGrid.h"
#include <memory>
#include <string>

class CGridViewModel
{
public:
	CGridViewModel();

	void LoadData(const std::string& filePath);
	[[nodiscard]] CGrid* GetGrid() const;

private:
	CFileLoader m_fileLoader;
	std::unique_ptr<CGrid> m_grid;
};