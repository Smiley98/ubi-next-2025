#if BUILD_PLATFORM_WINDOWS
#include <windows.h> 
#endif

#include <iostream>
#include <vector>
#include "raymath.h"
#include "../ContestAPI/app.h"

void Init()
{
}

void Update(const float dt)
{
}

void Render()
{
	Matrix world = MatrixRotateZ(45.0f * DEG2RAD); //MatrixIdentity();
	Matrix view = MatrixLookAt({ 0.0f, 0.0f, 10.0f }, Vector3Zeros, Vector3UnitY);
	Matrix proj = MatrixPerspective(90.0f * DEG2RAD, APP_VIRTUAL_WIDTH / (float)APP_VIRTUAL_HEIGHT, 0.1f, 100.0f);
	Matrix mvp = world * view * proj;

	std::vector<Vector3> vertices;
	vertices.resize(2);
	vertices[0] = { -0.5f, 0.0f, 0.0f };
	vertices[1] = { 0.5f, 0.0f, 0.0f };

	for (Vector3& v : vertices)
		v = MatrixPerspectiveDivide(mvp, v);

	App::DrawLine(vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y);
}

void Shutdown()
{
}
