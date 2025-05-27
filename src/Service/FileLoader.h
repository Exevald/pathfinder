#pragma once

#include "../Model/Grid.h"
#include "IMeshLoader.h"
#include <memory>

class FileLoader
{
public:
	explicit FileLoader(std::unique_ptr<IMeshLoader> meshLoader);
	Grid LoadAndConvert() const;

private:
	std::unique_ptr<IMeshLoader> m_meshLoader;
};