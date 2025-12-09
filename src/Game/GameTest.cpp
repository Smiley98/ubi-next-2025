#if BUILD_PLATFORM_WINDOWS
#include <windows.h> 
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include "raymath.h"
#include "../ContestAPI/app.h"

enum MeshType
{
	MESH_TRIANGLE,
	MESH_PLANE,
	MESH_TYPE_COUNT
};

struct Mesh
{
	size_t face_count = 0;
	std::vector<Vector3> positions;	// size is face_count * 3
	std::vector<Vector3> normals;	// size is face_count
};

// Mesh binary (.vbo_nxt) file structure:
//struct MeshVboNxt
//{
//	size_t position_count = 0;
//	size_t index_count = 0;
//	std::vector<Vector3> positions;
//	std::vector<Vector3> indices;
//};

struct UniformData
{
	Matrix world;
	Matrix mvp;

	Vector3 light_color;
	Vector3 light_direction;
};

void ImportMesh(Mesh* mesh, const char* filename);
void TriangulateMesh(Mesh* mesh, const std::vector<Vector3>& positions, const std::vector<uint16_t>& indices);
void UnloadMesh(Mesh* mesh);
void DrawMesh(Mesh mesh, const UniformData& data, bool wireframe = false);

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

		m.normals.resize(m.face_count);
		m.normals[0] = Vector3UnitZ;
	}

	{
		std::vector<Vector3> positions;
		positions.resize(4);
		positions[0] = { 0.5f, -0.5f, 0.0f };	// bottom-right
		positions[1] = { 0.5f,  0.5f, 0.0f };	// top-right
		positions[2] = { -0.5f,  0.5f, 0.0f };	// top-left
		positions[3] = { -0.5f, -0.5f, 0.0f };	// bottom-left

		std::vector<uint16_t> indices
		{
			0, 1, 3,
			1, 2, 3
		};

		TriangulateMesh(&meshes[MESH_PLANE], positions, indices);
	}

	// Initialization sanity-check
	for (int i = 0; i < MESH_TYPE_COUNT; i++)
		assert(meshes[i].face_count > 0);
}

void Update(const float deltaTime)
{
	const float dt = deltaTime / 1000.0f;
	tt += dt;
}

void Render()
{
	Matrix world = MatrixRotateZ(100.0f * tt * DEG2RAD) * MatrixRotateY(50.0f * tt * DEG2RAD) * MatrixTranslate(0.0f, 0.0f, 5.0f + sinf(tt) * 4.5f);
	Matrix view = MatrixLookAt({ 0.0f, 0.0f, 10.0f }, Vector3Zeros, Vector3UnitY);
	Matrix proj = MatrixPerspective(90.0f * DEG2RAD, APP_VIRTUAL_WIDTH / (float)APP_VIRTUAL_HEIGHT, 0.1f, 100.0f);

	UniformData data;
	data.world = world;
	data.mvp = world * view * proj;

	const CController& cont = CSimpleControllers::GetInstance().GetController();

	static int mesh = MESH_TRIANGLE;
	if (cont.CheckButton(App::BTN_DPAD_DOWN)) // Emulated as KEY_K
		++mesh %= MESH_TYPE_COUNT;
	
	static bool wireframe = false;
	if (cont.CheckButton(App::BTN_DPAD_UP)) // Emulated as KEY_I
		wireframe = !wireframe;

	DrawMesh(meshes[mesh], data, wireframe);
}

void Shutdown()
{
	for (int i = 0; i < MESH_TYPE_COUNT; i++)
		UnloadMesh(&meshes[i]);
}

void ImportMesh(Mesh* mesh, const char* filename)
{
	std::ifstream in;
	in.open(filename, std::ios::binary);

	size_t position_count = 0;
	size_t index_count = 0;
	in.read((char*)&position_count, sizeof(position_count));
	in.read((char*)&index_count, sizeof(index_count));

	std::vector<Vector3> positions;
	std::vector<uint16_t> indices;
	positions.resize(position_count);
	indices.resize(index_count);

	in.read((char*)positions.data(), sizeof(Vector3) * position_count);
	in.read((char*)indices.data(), sizeof(Vector3) * index_count);
	in.close();

	TriangulateMesh(mesh, positions, indices);
}

void TriangulateMesh(Mesh* mesh, const std::vector<Vector3>& positions, const std::vector<uint16_t>& indices)
{
	mesh->face_count = indices.size() / 3;
	mesh->positions.resize(indices.size());
	mesh->normals.resize(mesh->face_count);

	for (size_t f = 0; f < mesh->face_count; f++)
	{
		size_t v = f * 3;
		Vector3 v0 = positions[indices[v + 0]];
		Vector3 v1 = positions[indices[v + 1]];
		Vector3 v2 = positions[indices[v + 2]];
		Vector3 n = Vector3Normalize(Vector3CrossProduct(Vector3Normalize(v1 - v0), Vector3Normalize(v2 - v0)));

		mesh->positions[v + 0] = v0;
		mesh->positions[v + 1] = v1;
		mesh->positions[v + 2] = v2;
		mesh->normals[f] = n;
	}
}

void DrawMesh(Mesh mesh, const UniformData& data, bool wireframe)
{
	Matrix normal_matrix = MatrixNormal(data.world);
	for (size_t f = 0; f < mesh.face_count; f++)
	{
		Vector3 world_normal = mesh.normals[f] * normal_matrix;
		size_t v = f * 3;

		Vector3 world_positions[] =
		{
			mesh.positions[v + 0],
			mesh.positions[v + 1],
			mesh.positions[v + 2]
		};

		Vector3 clip[] =
		{
			mesh.positions[v + 0],
			mesh.positions[v + 1],
			mesh.positions[v + 2]
		};

		for (size_t i = 0; i < 3; i++)
		{
			world_positions[i] *= data.world;
			clip[i] = MatrixPerspectiveDivide(data.mvp, clip[i]);
		}

		Vector3 v0 = clip[0];
		Vector3 v1 = clip[1];
		Vector3 v2 = clip[2];

		Vector3 n = world_normal * 0.5f + Vector3Ones * 0.5f; // [-1, 1] --> [0, 1]
		App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, n.x, n.y, n.z, wireframe);
	}
}

void UnloadMesh(Mesh* mesh)
{
	mesh->positions.resize(0);
	mesh->normals.resize(0);
	mesh->face_count = 0;
}
