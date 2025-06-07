#pragma once

#include "../Model/Grid.h"
#include "IMeshLoader.h"
#include "Model/Model.h"

#include <memory>

class FileLoader
{
public:
	explicit FileLoader(std::unique_ptr<IMeshLoader> meshLoader);
	[[nodiscard]] Grid LoadAndConvert(const std::string& fileName) const;
	[[nodiscard]] std::shared_ptr<Model> GetModel() const;

private:
	std::unique_ptr<IMeshLoader> m_meshLoader;
};