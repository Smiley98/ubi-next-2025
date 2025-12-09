#include "Renderer.h"
#include "../ContestAPI/app.h"

struct Face
{
	Vector3 positions_world[3];
	Vector3 positions_clip[3];
	Vector3 normal_world;
};

void DrawMesh(const Mesh& mesh, const UniformData& data, bool wireframe)
{
	Matrix normal_matrix = MatrixNormal(data.world);

	std::vector<Face> faces;
	faces.resize(mesh.face_count);
	for (size_t f = 0; f < mesh.face_count; f++)
	{
		size_t v = f * 3;
		for (size_t i = 0; i < 3; i++)
		{
			Vector3 position_local = mesh.positions[v + i];
			faces[f].positions_world[i] = position_local * data.world;
			faces[f].positions_clip[i] = MatrixPerspectiveDivide(data.mvp, position_local);
		}
		faces[f].normal_world = mesh.normals[f] * normal_matrix;
	}

	for (const Face& face : faces)
	{
		Vector3 v0 = face.positions_clip[0];
		Vector3 v1 = face.positions_clip[1];
		Vector3 v2 = face.positions_clip[2];

		// Backface culling
		Vector3 face_normal = Vector3Normalize(Vector3CrossProduct(Vector3Normalize(v1 - v0), Vector3Normalize(v2 - v0)));
		if (Vector3DotProduct(face_normal, Vector3UnitZ) < 0.0f) continue;

		Vector3 p = (face.positions_world[0] + face.positions_world[1] + face.positions_world[2]) / 3.0f;
		Vector3 n = face.normal_world;

		Vector3 c = n * 0.5f + Vector3Ones * 0.5f;
		//Vector3 c = Vector3Normalize(p) * 0.5f + Vector3Ones * 0.5f;

		App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, c.x, c.y, c.z, wireframe);
	}
}
