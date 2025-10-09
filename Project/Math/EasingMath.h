#pragma once
#include "Vector3.h"
#include "Quaternion.h"

namespace Math {
	/// ===ラープ関数=== ///
	// float
	float Lerp(float start, float end, float t);
	// Vector3
	Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
	// Quaternion
	Quaternion Lerp(const Quaternion& start, const Quaternion& end, float t);

	/// ===Sラープ関数=== ///
	// Vector3
	Vector3 SLerp(const Vector3& start, const Vector3& end, float t);
	// Quaternion
	Quaternion SLerp(const Quaternion& start, const Quaternion& end, float t);
}
