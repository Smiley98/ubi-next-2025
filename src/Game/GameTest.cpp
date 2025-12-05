#if BUILD_PLATFORM_WINDOWS
#include <windows.h> 
#endif

#include <iostream>
#include <vector>
#include "raymath.h"
#include "../ContestAPI/app.h"

struct Mesh
{
	size_t face_count = 0;
	std::vector<Vector3> positions;	// size is face_count * 3 because triangles have 3 vertices xD xD xD
	std::vector<Vector3> colors;	// size is face_count because vertices are flat-shaded
	std::vector<uint16_t> indices;
};

void UnloadMesh(Mesh* mesh)
{
	mesh->positions.resize(0);
	mesh->colors.resize(0);
	mesh->indices.resize(0);
	mesh->face_count = 0;
}

void DrawMesh(Mesh mesh, Matrix mvp, Vector3 color = Vector3Ones, bool wireframe = false)
{
	for (size_t f = 0; f < mesh.face_count; f++)
	{
		size_t v = f * 3;

		Vector3 v0 = MatrixPerspectiveDivide(mvp, mesh.positions[v + 0]);
		Vector3 v1 = MatrixPerspectiveDivide(mvp, mesh.positions[v + 1]);
		Vector3 v2 = MatrixPerspectiveDivide(mvp, mesh.positions[v + 2]);

		Vector3 c = mesh.colors.empty() ? color : mesh.colors[f];
		App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, c.x, c.y, c.z, wireframe);
	}
}

enum MeshType
{
	MESH_TRIANGLE,
	MESH_PLANE,
	MESH_TYPE_COUNT
};

static float tt = 0.0f;
static Mesh meshes[MESH_TYPE_COUNT];

void Init()
{
	{
		Mesh& m = meshes[MESH_TRIANGLE];
		m.face_count = 1;

		m.positions.resize(m.face_count * 3);
		m.positions[0] = { 0.5f, -0.5f, 0.0f };
		m.positions[1] = { 0.0f,  0.5f, 0.0f };
		m.positions[2] = { -0.5f, -0.5f, 0.0f };

		m.colors.resize(m.face_count * 1);
		m.colors[0] = Vector3UnitX;
	}
	
	// TODO -- figure out how to use index buffer for meshes with more than 3 vertices xD xD xD
}

void Update(const float deltaTime)
{
	const float dt = deltaTime / 1000.0f;
	tt += dt;
}

void Render()
{
	Matrix world = MatrixRotateZ(100.0f * tt * DEG2RAD) * MatrixTranslate(0.0f, 0.0f, 5.0f + sinf(tt) * 3.0f);
	Matrix view = MatrixLookAt({ 0.0f, 0.0f, 10.0f }, Vector3Zeros, Vector3UnitY);
	Matrix proj = MatrixPerspective(90.0f * DEG2RAD, APP_VIRTUAL_WIDTH / (float)APP_VIRTUAL_HEIGHT, 0.1f, 100.0f);
	Matrix mvp = world * view * proj;

	static int mesh = MESH_TRIANGLE;
	if (App::IsKeyPressed(App::KEY_TAB))
		++mesh %= MESH_TYPE_COUNT;

	DrawMesh(meshes[mesh], mvp);
}

void Shutdown()
{
	for (int i = 0; i < MESH_TYPE_COUNT; i++)
		UnloadMesh(&meshes[i]);
}
