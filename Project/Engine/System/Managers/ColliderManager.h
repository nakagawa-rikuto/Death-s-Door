#pragma once
/// ===Include=== ///
// c++
#include <list>
// Collider
#include "Engine/Collider/Base/Collider.h"

/// ===前方宣言=== ///
class SphereCollider;
class AABBCollider;
class OBBCollider;


///=====================================================/// 
/// ColliderManager
///=====================================================///
class ColliderManager {
public:

	ColliderManager() = default;
	~ColliderManager();

	// 初期化
	void Initialize();	

	// リセット
	void Reset();
	// 削除
	void RemoveCollider(Collider* collider);
	// 追加
	void AddCollider(Collider* collider);
	// 二つのColliderの当たり判定
	void CheckPairCollision(Collider* colliderA, Collider* colliderB);
	// 全てのColliderの当たり判定
	void CheckAllCollisions();

public: /// ===Setter=== ///
	// Lightの設定
	void SetLight(LightType type);
	void SetLightData(LightInfo light);

private: /// ===変数=== ///

	// コライダーのリスト
	std::list<Collider*> colliders_;

	// 衝突判定関数の型定義
	using CollisionFunc = bool (ColliderManager::*)(Collider*, Collider*);
	// 衝突関数ディスパッチテーブル
	static constexpr int TypeCount = (int)ColliderType::Count;
	CollisionFunc collisionTable_[TypeCount][TypeCount];

private: /// ===関数=== ///

	// 各当たり判定関数（Collider*を受け取るラッパー）
	bool Sphere_Sphere(Collider* a, Collider* b);
	bool AABB_AABB(Collider* a, Collider* b);
	bool OBB_OBB(Collider* a, Collider* b);
	bool Sphere_AABB(Collider* a, Collider* b);
	bool AABB_Sphere(Collider* a, Collider* b);
	bool AABB_OBB(Collider* a, Collider* b);
	bool OBB_AABB(Collider* a, Collider* b);
	bool Sphere_OBB(Collider* a, Collider* b);
	bool OBB_Sphere(Collider* a, Collider* b);

	// 球と球の当たり判定
	bool SphereToSphereCollision(class SphereCollider* a, class SphereCollider* b);
	// AABBとAABBの当たり判定
	bool AABBToAABBCollision(class AABBCollider* a, class AABBCollider* b);
	// OBBとOBBの当たり判定
	bool OBBToOBBCollision(class OBBCollider* a, class OBBCollider* b);
	// 球とAABBの当たり判定
	bool SphereToAABBCollision(class SphereCollider* sphere, class AABBCollider* aabb);
	// AABBとOBBの当たり判定
	bool AABBToOBBCollision(class AABBCollider* aabb, class OBBCollider* obb);
	// 球とOBBの当たり判定
	bool SphereToOBBCollision(class SphereCollider* sphere, class OBBCollider* obb);

	// SATによる OBB vs OBB 判定
	bool OBBSATCollision(const OBB& a, const OBB& b);
	void ProjectOBBOntoAxis(const OBB& obb, const Vector3& axis, float& outMin, float& outMax);
};

