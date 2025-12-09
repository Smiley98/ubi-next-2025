#pragma once
#include "Mesh.h"
// All "Renderer" functions will be prefixed by "Draw" (just like how all "Mesh" functions are prefixed by "Mesh")

struct UniformData
{
	Matrix world;
	Matrix mvp;

	Vector3 light_color;
	Vector3 light_direction;
};

void DrawMesh(const Mesh& mesh, const UniformData& data, bool wireframe = false);
