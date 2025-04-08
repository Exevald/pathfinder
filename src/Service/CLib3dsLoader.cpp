#include "CLib3dsLoader.h"
#include <stdexcept>

CLib3dsLoader::CLib3dsLoader(Lib3dsFile* file)
	: m_file(file)
{
}

CLib3dsLoader::CLib3dsLoader(const char* fileName)
{
	auto pFile = lib3ds_file_load(fileName);
	if (!pFile)
	{
		throw std::runtime_error("Unable to read 3ds file");
	}
	m_file = pFile;
}

CLib3dsLoader::~CLib3dsLoader()
{
	if (m_file)
	{
		lib3ds_file_free(m_file);
	}
}

CLib3dsLoader::CLib3dsLoader(const CLib3dsLoader& other) noexcept
	: CLib3dsLoader(other.m_file)
{
}

CLib3dsLoader::CLib3dsLoader(CLib3dsLoader&& other) noexcept
	: CLib3dsLoader(other.m_file)
{
	other.m_file = nullptr;
}

std::vector<CMesh> CLib3dsLoader::GetMeshes() const
{
	return {};
}

CLib3dsLoader& CLib3dsLoader::operator=(const CLib3dsLoader& other)
{
	if (this != &other)
	{
		m_file = other.m_file;
	}
	return *this;
}

CLib3dsLoader& CLib3dsLoader::operator=(CLib3dsLoader&& other) noexcept
{
	if (this != &other)
	{
		m_file = other.m_file;
		other.m_file = nullptr;
	}
	return *this;
}