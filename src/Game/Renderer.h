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

struct Fragment
{
	Vector3 p;
	Vector3 n;
};

using FragmentShader = Vector3(*)(const UniformData& u, const Fragment& f);

void DrawMesh(const Mesh& mesh, const UniformData& data, FragmentShader shader, bool wireframe = false);

inline Vector3 ShadePositions(const UniformData& u, const Fragment& f)
{
	Vector3 c = Vector3Normalize(f.p) * 0.5f + Vector3Ones * 0.5f;
	return c;
}

inline Vector3 ShadeNormals(const UniformData& u, const Fragment& f)
{
	Vector3 c = f.n * 0.5f + Vector3Ones * 0.5f;
	return c;
}

inline Vector3 ShadePhong(const UniformData& u, const Fragment& f)
{
	Vector3 c = u.light_color;
	return c;
}
