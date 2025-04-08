#pragma once

#include "IMeshLoader.h"
#include <file.h>
#include <vector>

class CLib3dsLoader final : public IMeshLoader
{
public:
	explicit CLib3dsLoader(Lib3dsFile* file);
	explicit CLib3dsLoader(const char* fileName);
	~CLib3dsLoader() override;

	CLib3dsLoader(const CLib3dsLoader& other) noexcept;
	CLib3dsLoader(CLib3dsLoader&& other) noexcept;

	[[nodiscard]] std::vector<CMesh> GetMeshes() const override;

	CLib3dsLoader& operator=(const CLib3dsLoader& other);
	CLib3dsLoader& operator=(CLib3dsLoader&& other) noexcept;

private:
	Lib3dsFile* m_file;
	std::vector<Lib3dsMesh> m_meshes;
};