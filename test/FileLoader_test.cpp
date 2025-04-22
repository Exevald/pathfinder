#include "../src/Service/CFileLoader.h"
#include "MockMeshLoader.h"
#include <gtest/gtest.h>

TEST(CFileLoader, LoadAndConvert_EmptyMeshes)
{
	MockMeshLoader mockLoader;
	mockLoader.SetMeshes({});

	CFileLoader fileLoader(std::make_unique<MockMeshLoader>(mockLoader));
	CGrid grid = fileLoader.LoadAndConvert();

	EXPECT_EQ(grid.GetCells().size(), 0);
}
