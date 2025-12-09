#include "Renderer.h"
#include "../ContestAPI/app.h"

void DrawMesh(const Mesh& mesh, const UniformData& data, bool wireframe)
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

		// Backface culling
		Vector3 face_normal = Vector3Normalize(Vector3CrossProduct(Vector3Normalize(v1 - v0), Vector3Normalize(v2 - v0)));
		if (Vector3DotProduct(face_normal, Vector3UnitZ) < 0.0f) continue;

		Vector3 p = (world_positions[0] + world_positions[1] + world_positions[2]) / 3.0f;
		Vector3 n = world_normal;

		//Vector3 c = n * 0.5f + Vector3Ones * 0.5f;
		Vector3 c = Vector3Normalize(p) * 0.5f + Vector3Ones * 0.5f;

		App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, c.x, c.y, c.z, wireframe);
	}
}
