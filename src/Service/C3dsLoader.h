#pragma once

#include "IMeshLoader.h"
#include <file.h>
#include <vector>

class C3dsLoader final : public IMeshLoader
{
public:
	explicit C3dsLoader(Lib3dsFile* file);
	explicit C3dsLoader(const char* fileName);
	~C3dsLoader() override;

	[[nodiscard]] std::vector<CMesh> GetMeshes() const override;

	C3dsLoader& operator=(const C3dsLoader& other);
	C3dsLoader& operator=(C3dsLoader&& other) noexcept;

private:
	Lib3dsFile* m_file;
	std::vector<Lib3dsMesh> m_meshes;
};