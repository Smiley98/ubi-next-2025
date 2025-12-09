#include "Renderer.h"
#include "../ContestAPI/app.h"
#include <algorithm>

struct Face
{
	Vector3 positions_world[3];
	Vector3 positions_clip[3];
	Vector3 normal_world;
};

void DrawMesh(const Mesh& mesh, const UniformData& data, FragmentShader shader, bool wireframe)
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
		faces[f].normal_world = Vector3Normalize(mesh.normals[f] * normal_matrix);
	}

	auto pr = [](const Face& a, const Face& b)
	{
		float avg_depth_a = (a.positions_clip[0].z + a.positions_clip[1].z + a.positions_clip[2].z) / 3.0f;
		float avg_depth_b = (b.positions_clip[0].z + b.positions_clip[1].z + b.positions_clip[2].z) / 3.0f;
		return avg_depth_a > avg_depth_b;
	};

	// Painter's Algorithm -- render furthest faces first, effectively removing the need for depth-testing!
	std::sort(faces.begin(), faces.end(), pr);

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

		Fragment frag;
		frag.p = p;
		frag.n = n;

		Vector3 color = shader(data, frag);
		App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, color.x, color.y, color.z, wireframe);
	}
}
