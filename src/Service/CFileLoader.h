#pragma once

#include "../Model/Grid.h"
#include "IMeshLoader.h"
#include <memory>

class CFileLoader
{
public:
	explicit CFileLoader(std::unique_ptr<IMeshLoader> meshLoader);
	Grid LoadAndConvert();

private:
	std::unique_ptr<IMeshLoader> m_meshLoader;
};