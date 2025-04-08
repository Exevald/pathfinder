#pragma once

#include "../Model/CMesh.h"
#include <mesh.h>
#include <vector>

class IMeshLoader
{
public:
	[[nodiscard]] virtual std::vector<CMesh> GetMeshes() const = 0;

	virtual ~IMeshLoader() = default;
};