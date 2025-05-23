#pragma once

#include "Vector3.h"

struct Vertex
{
	CVector3f position;
	CVector3f normal;
};

struct TexturedVertex : public Vertex
{
	CVector3f texCoord;
};