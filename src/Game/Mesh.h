#pragma once
#include <cstdint>
#include <vector>
#include "raymath.h"

struct Mesh
{
	size_t face_count = 0;
	std::vector<Vector3> positions;	// size is face_count * 3
	std::vector<Vector3> normals;	// size is face_count
};

void MeshImport(Mesh* mesh, const char* filename);
void MeshTriangulate(Mesh* mesh, const std::vector<Vector3>& positions, const std::vector<uint16_t>& indices);
void MeshUnload(Mesh* mesh);
