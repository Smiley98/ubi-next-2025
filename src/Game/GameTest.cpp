#if BUILD_PLATFORM_WINDOWS
#include <windows.h> 
#endif

#include <iostream>
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
	std::vector<Vector3> normals;	// size is face_count * 3
};

struct UniformData
{
	Matrix world;
	Matrix mvp;

	Vector3 light_color;
	Vector3 light_direction;
};

void TriangulateMesh(Mesh* mesh, std::vector<uint16_t> indices);
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

		m.normals.resize(m.face_count * 3);
		for (Vector3& n : m.normals)
			n = Vector3UnitZ;
	}

	{
		Mesh& m = meshes[MESH_PLANE];
		m.positions.resize(4);
		m.positions[0] = { 0.5f, -0.5f, 0.0f };		// bottom-right
		m.positions[1] = { 0.5f,  0.5f, 0.0f };		// top-right
		m.positions[2] = { -0.5f,  0.5f, 0.0f };	// top-left
		m.positions[3] = { -0.5f, -0.5f, 0.0f };	// bottom-left

		m.normals.resize(4);
		for (Vector3& n : m.normals)
			n = Vector3UnitZ;

		std::vector<uint16_t> indices
		{
			0, 1, 3,
			1, 2, 3
		};

		TriangulateMesh(&m, indices);
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

void TriangulateMesh(Mesh* mesh, std::vector<uint16_t> indices)
{
	std::vector<Vector3> positions_src = mesh->positions;
	std::vector<Vector3> normals_src = mesh->normals;

	mesh->positions.resize(indices.size());
	mesh->normals.resize(indices.size());
	mesh->face_count = indices.size() / 3;

	for (size_t i = 0; i < mesh->face_count * 3; i++)
	{
		uint16_t index = indices[i];
		mesh->positions[i] = positions_src[index];
		mesh->normals[i] = normals_src[index];
	}
}

void DrawMesh(Mesh mesh, const UniformData& data, bool wireframe)
{
	Matrix normal_matrix = MatrixNormal(data.world);
	for (size_t f = 0; f < mesh.face_count; f++)
	{
		// Note -- just copy each vertex attribute 1 triangle at a time because we have better things to do than debug memory corruption...
		size_t v = f * 3;

		Vector3 world_positions[] =
		{
			mesh.positions[v + 0],
			mesh.positions[v + 1],
			mesh.positions[v + 2]
		};

		Vector3 world_normals[] =
		{
			mesh.normals[v + 0],
			mesh.normals[v + 1],
			mesh.normals[v + 2]
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
			world_normals[i] *= normal_matrix;
			clip[i] = MatrixPerspectiveDivide(data.mvp, clip[i]);
		}

		Vector3 v0 = clip[0];
		Vector3 v1 = clip[1];
		Vector3 v2 = clip[2];

		// Potential data reduction -- divide normals by 3 when defining mesh because we know all 3 vertex normals are identical per-triangle
		Vector3 n0 = world_normals[0];
		Vector3 n1 = world_normals[1];
		Vector3 n2 = world_normals[2];
		Vector3 n = n0;	// <-- all 3 normals are identical per-face.

		// Color based on normals remapped from [-1, 1] --> [0, 1]
		Vector3 c = n;
		c *= 0.5f;
		c += Vector3Ones * 0.5f;
		App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, c.x, c.y, c.z, wireframe);
	}
}

void UnloadMesh(Mesh* mesh)
{
	mesh->positions.resize(0);
	mesh->normals.resize(0);
	mesh->face_count = 0;
}
