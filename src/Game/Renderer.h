#pragma once
#include "Mesh.h"
// All "Renderer" functions will be prefixed by "Draw" (just like how all "Mesh" functions are prefixed by "Mesh")

struct UniformData
{
	Matrix world;
	Matrix mvp;

	Vector3 camera_position;
	Vector3 object_color;

	Vector3 light_color;
	Vector3 light_position;

	float ambient_strength;
	float diffuse_strength;
	float specular_strength;
	float specular_exponent;
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
	Vector3 N = f.n;
	Vector3 L = Vector3Normalize(u.light_position - f.p);
	Vector3 V = Vector3Normalize(u.camera_position - f.p);
	Vector3 R = Vector3Reflect(L * -1.0f, N);
	float dotNL = fmaxf(Vector3DotProduct(N, L), 0.0f);
	float dotVR = fmaxf(Vector3DotProduct(V, R), 0.0f);

	Vector3 ambient = u.light_color * u.ambient_strength;
	Vector3 diffuse = u.light_color * u.diffuse_strength * dotNL;
	Vector3 specular = u.light_color * u.specular_strength * powf(dotVR, u.specular_exponent);
	Vector3 lighting = ambient + diffuse + specular;

	Vector3 c = u.object_color * lighting;
	return c;
}
