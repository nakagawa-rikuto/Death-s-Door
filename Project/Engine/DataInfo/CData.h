#pragma once
/// ===include=== ///
// C++
#include <Windows.h>
#include <chrono>
#include <cstdlib>
#include <map>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>

// Math
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3x3.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"

#pragma region Buffer情報
/// ===VertexData(2D)=== ///
struct VertexData2D {
	Vector4 position;
	Vector2 texcoord;
};
/// ===VertexData(3D)=== ///
struct VertexData3D {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
/// ===Material(2D)=== ///
struct MaterialData2D {
	Vector4 color;
	Matrix4x4 uvTransform;
};
/// ===Material(3D)=== ///
struct MaterialData3D {

	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};
/// ===TransformationMatrix(2D)=== ///
struct TransformationMatrix2D {
	Matrix4x4 WVP;
};
/// ===TransformationMatrix(3D)=== ///
struct TransformationMatrix3D {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};
#pragma endregion
#pragma region Transform情報
/// ===EulerTransform=== ///
struct EulerTransform {
	Vector3 scale;
	Vector3 rotate; // Eulerでの回転
	Vector3 translate;
};
/// ===QuaternionTransform=== ///
struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};
#pragma endregion
#pragma region モデル情報
/// ===VertexWeightData=== ///
struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};
/// ===JointWeightData=== ///
struct jointWeightData {
	Matrix4x4 inverseBindPosematrix;
	std::vector<VertexWeightData> vertexWeights;
};
/// ===マテリアルデータ=== ///
struct  MaterialData {
	std::string textureFilePath;
};
/// ===ノード=== ///
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};
/// ===モデルデータ=== ///
struct ModelData {
	std::map<std::string, jointWeightData> skinClusterData;
	std::vector<VertexData3D> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
	bool haveBone;
};
#pragma endregion
#pragma region ライト情報
/// ===平行光源の拡張=== ///
struct DirectionalLight {
	Vector4 color;     // ライトの色
	Vector3 direction; // ライトの向き
	float intensity;   // ライトの明るさ(輝度)
};
/// ===カメラの位置=== ///
struct CameraForGPU {
	Vector3 worldPosition; // カメラのワールド位置W
};
/// ===ポイントライト=== ///
struct PointLight {
	Vector4 color; 		// ライトの色
	Vector3 position;	// ライトの位置
	float intensity;	// ライトの明るさ(輝度)
	float radius;		// ライトの半径
	float decay;		// 減衰率
	float padding[2];	// パディング
};
/// ===スポットライト=== ///
struct SpotLight {
	Vector4 color;		// ライトの色
	Vector3 position;	// ライトの位置
	float intensity;	// ライトの明るさ(輝度)
	Vector3 direction;	// ライトの向き
	float distance;		// ライトの距離
	float decay;		// 減衰率
	float cosAngle;		// コサイン角度(スポットライトの角度)
	float padding[3];	// パディング
};
#pragma endregion
#pragma region 環境マップ
/// ===環境マップ=== ///
struct EnviromentMap {
	int32_t enable;		// 環境マップの有効化
	float strength;		// 環境マップの強度
	float padding[2];   // パディング
};
#pragma endregion