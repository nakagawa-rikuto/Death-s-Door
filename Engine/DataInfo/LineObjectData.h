#pragma once
/// ===Include=== ///
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

struct LineVertexData3D {
	Vector3 position;
	Vector4 color;
};

struct LineTransformMatrixData3D {
	Matrix4x4 WVP;
};
