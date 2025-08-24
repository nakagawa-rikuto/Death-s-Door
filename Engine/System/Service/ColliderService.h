#pragma once
/// ===Include=== ///
// Data
#include "Engine/DataInfo/LightData.h"

/// ===前方宣言=== ///
class Collider;
class Camera;
class Light;

///-------------------------------------------/// 
/// ColliderService
///-------------------------------------------///
class ColliderService {
public:

	// リセット
	static void Reset();
	// 削除
	static void RemoveCollider(Collider* collider);
	// 追加
	static void AddCollider(Collider* collider);

	// Lightの設定
	static void SetLight(LightType type);
	static void SetLightData(LightInfo light);
};

