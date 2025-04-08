#pragma once

#include "../Model/CGrid.h"
#include "IMeshLoader.h"
#include <memory>

class CFileLoader
{
public:
	explicit CFileLoader(std::unique_ptr<IMeshLoader> meshLoader);

	CGrid LoadAndConvert();

private:
	std::unique_ptr<IMeshLoader> m_meshLoader;
};