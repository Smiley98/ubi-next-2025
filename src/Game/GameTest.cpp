#if BUILD_PLATFORM_WINDOWS
#include <windows.h> 
#endif

#include <iostream>
#include <vector>
#include "raymath.h"
#include "../ContestAPI/app.h"

void Draw(std::vector<Vector3> positions, Matrix mvp, Vector3 color = Vector3Ones, bool wireframe = false)
{
	for (Vector3& v : positions)
		v = MatrixPerspectiveDivide(mvp, v);

	Vector3 v0 = positions[0];
	Vector3 v1 = positions[1];
	Vector3 v2 = positions[2];
	App::DrawTriangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, color.x, color.y, color.z, wireframe);
}

static float tt = 0.0f;

void Init()
{
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

	std::vector<Vector3> triangle;
	triangle.resize(3);
	triangle[0] = { 0.5f, -0.5f, 0.0f };
	triangle[1] = { 0.0f,  0.5f, 0.0f };
	triangle[2] = { -0.5f, -0.5f, 0.0f };

	Draw(triangle, mvp);
}

void Shutdown()
{
}
