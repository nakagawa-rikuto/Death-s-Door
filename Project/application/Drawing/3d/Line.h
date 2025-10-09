#pragma once
/// ===Include=== ///
// Data
#include "Engine/DataInfo/ColliderData.h"
#include "Engine/DataInfo/LineObjectData.h"
// C++
#include <cstdint>

///=====================================================/// 
/// Line
///=====================================================///
class Line {
public:
	// Line
	void DrawLine(const Vector3& start, const Vector3& end, const Vector4& color);
	// OBB
	void DrawOBB(const OBB& obb, const Vector4& color);
	// AABB
	void DrawAABB(const AABB& aabb, const Vector4& color);
	// Sphere
	void DrawSphere(const Sphere& sphere, const Vector4& color);
	// Grid
	void DrawGrid(const Vector3& center, const Vector3& size, uint32_t division, const Vector4& color);
	// GridBox
	void DrawGirdBox(const AABB& aabb, uint32_t division, const Vector3& center, const Vector4& color);

	// 1方向に対して分割グリッド線を描画するユーティリティ関数
	void DrawGridLines(const Vector3& start, const Vector3& end, const Vector3& offset, uint32_t division, const Vector4& color);
};

