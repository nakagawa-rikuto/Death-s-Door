#include "Line.h"
#include "Engine/System/Service/ServiceLocator.h"
#include "Engine/Graphics/3d/Line/LineObject3D.h"

#include "Math/MatrixMath.h"
#include "Math/EasingMath.h"
#include "Math/sMath.h"

///-------------------------------------------/// 
/// Line
///-------------------------------------------///
void Line::DrawLine(const Vector3& start, const Vector3& end, const Vector4& color) {
	ServiceLocator::GetLineObject3D()->CreateLine(start, end, color);
}

///-------------------------------------------/// 
/// OBB
///-------------------------------------------///
void Line::DrawOBB(const OBB& obb, const Vector4& color) {
	// OBBの各頂点を定義（ローカル座標）
	Vector3 localVertices[8] = {
		{-obb.halfSize.x, -obb.halfSize.y, -obb.halfSize.z}, {obb.halfSize.x, -obb.halfSize.y, -obb.halfSize.z},
		{obb.halfSize.x,  obb.halfSize.y, -obb.halfSize.z}, {-obb.halfSize.x,  obb.halfSize.y, -obb.halfSize.z},
		{-obb.halfSize.x, -obb.halfSize.y,  obb.halfSize.z}, {obb.halfSize.x, -obb.halfSize.y,  obb.halfSize.z},
		{obb.halfSize.x,  obb.halfSize.y,  obb.halfSize.z}, {-obb.halfSize.x,  obb.halfSize.y,  obb.halfSize.z}
	};

	// ワールド座標に変換（回転適用 & 平行移動）
	Vector3 worldVertices[8];
	for (int i = 0; i < 8; i++) {
		worldVertices[i] =
			obb.center +
			obb.axis[0] * localVertices[i].x +
			obb.axis[1] * localVertices[i].y +
			obb.axis[2] * localVertices[i].z;
	}

	// OBBのエッジを結ぶ
	int edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0}, // 底面
		{4, 5}, {5, 6}, {6, 7}, {7, 4}, // 上面
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // 側面
	};

	for (int i = 0; i < 12; i++) {
		DrawLine(worldVertices[edges[i][0]], worldVertices[edges[i][1]], color);
	}
}

///-------------------------------------------/// 
/// AABB
///-------------------------------------------///
void Line::DrawAABB(const AABB & aabb, const Vector4& color) {
	// グリッドを6面描画
	Vector3 size = aabb.max - aabb.min;
	Vector3 halfSize = size * 0.5f;
	uint32_t division = 10;

	// AABBの中心を算出
	Vector3 offset = (aabb.min + aabb.max) * 0.5f;

	// 前面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);

	// 後面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);

	// 左面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);

	// 右面
	DrawGridLines(offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);

	// 上面
	DrawGridLines(offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);

	// 下面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);
}

///-------------------------------------------/// 
/// Sphere
///-------------------------------------------///
void Line::DrawSphere(const Sphere& sphere, const Vector4 & color) {
	const uint32_t div = 8;
	const float lonStep = 2.0f * Math::Pi() / float(div);
	const float latStep = Math::Pi() / float(div);

	for (uint32_t lat = 0; lat < div; ++lat) {
		for (uint32_t lon = 0; lon < div; ++lon) {
			Vector3 a, b, c;

			float lat1 = -Math::Pi() / 2.0f + latStep * lat;
			float lon1 = lonStep * lon;

			a = {
				cos(lat1) * cos(lon1),
				sin(lat1),
				cos(lat1) * sin(lon1)
			};
			b = {
				cos(lat1 + latStep) * cos(lon1),
				sin(lat1 + latStep),
				cos(lat1 + latStep) * sin(lon1)
			};
			c = {
				cos(lat1) * cos(lon1 + lonStep),
				sin(lat1),
				cos(lat1) * sin(lon1 + lonStep)
			};

			// スケール & 中心変換
			a = a * sphere.radius + sphere.center;
			b = b * sphere.radius + sphere.center;
			c = c * sphere.radius + sphere.center;

			DrawLine(a, b, color);
			DrawLine(a, c, color);
		}
	}
}

///-------------------------------------------/// 
/// Grid
///-------------------------------------------///
void Line::DrawGrid(const Vector3 & center, const Vector3 & size, uint32_t division, const Vector4& color) {
	Vector3 halfSize = size * 0.5f;
	Vector3 start = center + Vector3(-halfSize.x, 0.0f, -halfSize.z);
	Vector3 end = center + Vector3(halfSize.x, 0.0f, -halfSize.z);

	// 横線
	for (uint32_t i = 0; i <= division; i++) {
		float t = float(i) / float(division);
		Vector3 s = Math::Lerp(start, end, t);
		Vector3 e = s + Vector3(0.0f, 0.0f, size.z);
		DrawLine(s, e, color);
	}
	start = center + Vector3(-halfSize.x, 0.0f, -halfSize.z);
	end = center + Vector3(-halfSize.x, 0.0f, halfSize.z);
	// 縦線
	for (uint32_t i = 0; i <= division; i++) {
		float t = float(i) / float(division);
		Vector3 s = Math::Lerp(start, end, t);
		Vector3 e = s + Vector3(size.x, 0.0f, 0.0f);
		DrawLine(s, e, color);
	}

	//各座標軸の中心線のみ色変更
	/*DrawLine(center, center + Vector3(size.x, 0.0f, 0.0f), color);

	DrawLine(center, center + Vector3(0.0f, 0.0f, size.z), color);

	DrawLine(center, center + Vector3(0.0f, size.y, 0.0f), color);*/
}

///-------------------------------------------/// 
/// GridBox
///-------------------------------------------///
void Line::DrawGirdBox(const AABB& aabb, uint32_t division, const Vector3& center, const Vector4& color) {
	// グリッドを6面描画
	Vector3 size = aabb.max - aabb.min;
	Vector3 halfSize = size * 0.5f;

	// 中心を基準に座標を調整
	Vector3 offset = center;

	// 前面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);

	// 後面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);

	// 左面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);

	// 右面
	DrawGridLines(offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, halfSize.z),
		Vector3(0.0f, halfSize.y * 2, 0.0f), division, color);

	// 上面
	DrawGridLines(offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, halfSize.y, halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);

	// 下面
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		Vector3(0.0f, 0.0f, halfSize.z * 2), division, color);
	DrawGridLines(offset + Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		offset + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		Vector3(halfSize.x * 2, 0.0f, 0.0f), division, color);
}

///-------------------------------------------/// 
/// GridLine
///-------------------------------------------///
void Line::DrawGridLines(const Vector3 & start, const Vector3 & end, const Vector3 & offset, uint32_t division, const Vector4 & color) {
	for (uint32_t i = 0; i <= division; i++) {
		float t = float(i) / float(division);
		Vector3 startPos = Math::Lerp(start, end, t);
		Vector3 endPos = startPos + offset;
		DrawLine(startPos, endPos, color);
	}
}
