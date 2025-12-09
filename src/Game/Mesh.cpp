#include "Mesh.h"
#include <fstream>

void MeshImport(Mesh* mesh, const char* filename)
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

	MeshTriangulate(mesh, positions, indices);
}

void MeshTriangulate(Mesh* mesh, const std::vector<Vector3>& positions, const std::vector<uint16_t>& indices)
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

void MeshUnload(Mesh* mesh)
{
	mesh->positions.resize(0);
	mesh->normals.resize(0);
	mesh->face_count = 0;
}
