#pragma once

#include "../3D/Mesh/Mesh.h"
#include <vector>

class IMeshLoader
{
public:
	[[nodiscard]] virtual std::vector<Mesh> GetMeshes() const = 0;

	virtual ~IMeshLoader() = default;
};