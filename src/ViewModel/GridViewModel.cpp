#include "GridViewModel.h"
#include "../Service/ObjLoader.h"
#include <algorithm>
#include <iostream>

namespace
{
std::string GetFileExtension(const std::string& filePath)
{
	const size_t dotPos = filePath.find_last_of('.');
	if (dotPos == std::string::npos)
	{
		return "";
	}
	std::string extension = filePath.substr(dotPos + 1);
	std::ranges::transform(extension, extension.begin(), ::tolower);

	return extension;
}

std::unique_ptr<IMeshLoader> CreateMeshLoader(const std::string& filePath)
{
	const std::string extension = GetFileExtension(filePath);

	if (extension == "obj")
	{
		return std::make_unique<ObjLoader>();
	}
	throw std::runtime_error("Unsupported file format: " + extension);
}
} // namespace

GridViewModel::GridViewModel()
	: m_fileLoader(nullptr)
	, m_grid(nullptr)
{
}

void GridViewModel::LoadData(const std::string& filePath)
{
	try
	{
		auto meshLoader = CreateMeshLoader(filePath);
		auto fileLoader = FileLoader(std::move(meshLoader));
		m_fileLoader = std::move(fileLoader);

		m_grid = std::make_unique<Grid>(m_fileLoader.LoadAndConvert(filePath));
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

[[nodiscard]] Grid* GridViewModel::GetGrid() const
{
	return m_grid.get();
}