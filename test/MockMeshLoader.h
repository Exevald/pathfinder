#pragma once

#include <vector>

class MockMeshLoader : public IMeshLoader
{
public:
	void SetMeshes(const std::vector<CMesh>& meshes)
	{
		m_meshes = meshes;
	}

	[[nodiscard]] std::vector<CMesh> GetMeshes() const override
	{
		return m_meshes;
	}

private:
	std::vector<CMesh> m_meshes;
};