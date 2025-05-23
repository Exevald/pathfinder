#pragma once

#include "IMeshLoader.h"
#include <vector>

class MockMeshLoader final : public IMeshLoader
{
public:
	void SetMeshes(const std::vector<Mesh>& meshes)
	{
		m_meshes = meshes;
	}

	[[nodiscard]] std::vector<Mesh> GetMeshes() const override
	{
		return m_meshes;
	}

private:
	std::vector<Mesh> m_meshes;
};