#if BUILD_PLATFORM_WINDOWS
#include <windows.h> 
#endif

#include <cassert>
#include "Renderer.h"
#include "../ContestAPI/app.h"

enum MeshType
{
	MESH_TRIANGLE,
	MESH_PLANE,
	MESH_SPHERE,
	MESH_TYPE_COUNT
};

static Mesh meshes[MESH_TYPE_COUNT];
static void InitMeshes();

void Init()
{
	InitMeshes();
}

static float tt = 0.0f;
void Update(const float deltaTime)
{
	const float dt = deltaTime / 1000.0f;
	tt += dt;
}

void Render()
{
	Matrix world = MatrixTranslate(Vector3UnitX * 5.0f) * MatrixRotateZ(100.0f * tt * DEG2RAD);
	//Matrix world = MatrixRotateZ(100.0f * tt * DEG2RAD) * MatrixRotateY(50.0f * tt * DEG2RAD) * MatrixTranslate(0.0f, 0.0f, 8.0f);
	Matrix view = MatrixLookAt({ 0.0f, 0.0f, 10.0f }, Vector3Zeros, Vector3UnitY);
	Matrix proj = MatrixPerspective(90.0f * DEG2RAD, APP_VIRTUAL_WIDTH / (float)APP_VIRTUAL_HEIGHT, 0.1f, 100.0f);

	UniformData data;
	data.world = world;
	data.mvp = world * view * proj;

	const CController& cont = CSimpleControllers::GetInstance().GetController();

	static int mesh = MESH_SPHERE;
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
		MeshUnload(&meshes[i]);
}

void InitMeshes()
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

		MeshTriangulate(&meshes[MESH_PLANE], positions, indices);
	}

	MeshImport(&meshes[MESH_SPHERE], "./data/TestData/sphere.vbo_nxt");

	// Initialization sanity-check
	for (int i = 0; i < MESH_TYPE_COUNT; i++)
		assert(meshes[i].face_count > 0);
}
