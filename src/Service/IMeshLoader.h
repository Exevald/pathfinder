#pragma once

#include "../3D/Mesh/Mesh.h"
#include "../3D/Model/Model.h"
#include <string>
#include <vector>

class IMeshLoader
{
public:
	[[nodiscard]] virtual std::vector<Mesh> GetMeshes(const std::string& fileName) = 0;
	[[nodiscard]] virtual std::shared_ptr<Model> GetModel() const = 0;

	virtual ~IMeshLoader() = default;
};