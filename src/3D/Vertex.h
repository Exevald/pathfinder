#pragma once

namespace vector
{
struct Vector3
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};
} // namespace vector

struct Vertex
{
	vector::Vector3 position;
	vector::Vector3 normal;
};

struct TexturedVertex : Vertex
{
	vector::Vector2 texCoord;
};