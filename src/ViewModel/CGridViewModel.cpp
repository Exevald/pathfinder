#include "CGridViewModel.h"
#include "../Service/CObjLoader.h"
#include <algorithm>
#include <iostream>

namespace
{
std::string GetFileExtension(const std::string& filePath)
{
	size_t dotPos = filePath.find_last_of('.');
	if (dotPos == std::string::npos)
	{
		return "";
	}
	std::string extension = filePath.substr(dotPos + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	return extension;
}

std::unique_ptr<IMeshLoader> CreateMeshLoader(const std::string& filePath)
{
	const std::string extension = GetFileExtension(filePath);

	if (extension == "obj")
	{
		return std::make_unique<CObjLoader>(filePath.c_str());
	}
	throw std::runtime_error("Unsupported file format: " + extension);
}
} // namespace

CGridViewModel::CGridViewModel()
	: m_fileLoader(nullptr)
	, m_grid(nullptr)
{
}

void CGridViewModel::LoadData(const std::string& filePath)
{
	try
	{
		auto meshLoader = CreateMeshLoader(filePath);
		auto fileLoader = CFileLoader(std::move(meshLoader));
		m_fileLoader = std::move(fileLoader);

		m_grid = std::make_unique<Grid>(m_fileLoader.LoadAndConvert());
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

[[nodiscard]] Grid* CGridViewModel::GetGrid() const
{
	return m_grid.get();
}