#pragma once

#include "../3D/Model/Model.h"
#include "../Model/Grid.h"
#include "../Service/FileLoader.h"
#include <memory>
#include <string>

class GridViewModel
{
public:
	GridViewModel();

	void LoadData(const std::string& filePath);
	[[nodiscard]] Grid* GetGrid() const;
	std::shared_ptr<Model> GetModel();

private:
	FileLoader m_fileLoader;
	std::unique_ptr<Grid> m_grid;
	std::shared_ptr<Model> m_model;
};