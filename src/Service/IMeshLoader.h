#pragma once

#include "../3D/Mesh/Mesh.h"
#include <string>
#include <vector>

class IMeshLoader
{
public:
	[[nodiscard]] virtual std::vector<Mesh> GetMeshes(const std::string& fileName) = 0;

	virtual ~IMeshLoader() = default;
};