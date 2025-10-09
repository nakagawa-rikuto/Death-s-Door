#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <string>

/// ===Lightの種類のenum=== ///
enum class LightType {
	Lambert,
	HalfLambert,
	PointLight,
	SpotLight,
	None
};

/// ===DirectionalLight=== ///
struct DirectionalLightInfo {
	Vector4 color;		// ライトの色
	Vector3 direction;	// ライトの向き
	float intensity;	// ライトの明るさ(輝度)
};
/// ===ポイントライト=== ///
struct PointLightInfo {
	Vector4 color; 		// ライトの色
	Vector3 position;	// ライトの位置
	float intensity;	// ライトの明るさ(輝度)
	float radius;		// ライトの半径
	float decay;		// 減衰率
};
/// ===スポットライト=== ///
struct SpotLightInfo {
	Vector4 color; 		// ライトの色
	Vector3 position;	// ライトの位置
	float intensity;	// ライトの明るさ(輝度)
	Vector3 direction;	// ライトの向き
	float distance;		// ライトの距離
	float decay;		// 減衰率
	float cosAngle;		// コサイン角度(スポットライトの角度)
};
/// ===LightInfo=== ///
struct LightInfo {
	float shininess; 				  // 光沢度
	DirectionalLightInfo directional; // 平行光源の情報
	PointLightInfo point; 			  // ポイントライトの情報
	SpotLightInfo spot; 			  // スポットライトの情報
};
/// ===EnviromentMapInfo=== ///
struct EnviromentMapInfo {
	std::string textureName; // 環境マップのテクスチャ名
	bool isEnviromentMap; // 環境マップの有効/無効
	float strength;      // 環境マップの強度
};