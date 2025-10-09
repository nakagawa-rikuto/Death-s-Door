#include "ColliderManager.h"
// c++
#include <algorithm>
// ColliderType
#include "Engine/Collider/SphereCollider.h"
#include "Engine/Collider/AABBCollider.h"
#include "Engine/Collider/OBBCollider.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
ColliderManager::~ColliderManager() { colliders_.clear(); }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Lightの設定
void ColliderManager::SetLight(LightType type) {
	for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA) {
		(*itA)->SetLight(type);
	}
}
void ColliderManager::SetLightData(LightInfo light) {
	for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA) {
		(*itA)->SetLightData(light);
	}
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ColliderManager::Initialize() {
	// null初期化
	for (int i = 0; i < TypeCount; ++i) {
		for (int j = 0; j < TypeCount; ++j) {
			collisionTable_[i][j] = nullptr;
		}
	}

	// 判定関数の登録
	collisionTable_[(int)ColliderType::Sphere][(int)ColliderType::Sphere] = &ColliderManager::Sphere_Sphere;
	collisionTable_[(int)ColliderType::AABB][(int)ColliderType::AABB] = &ColliderManager::AABB_AABB;
	collisionTable_[(int)ColliderType::OBB][(int)ColliderType::OBB] = &ColliderManager::OBB_OBB;

	collisionTable_[(int)ColliderType::Sphere][(int)ColliderType::AABB] = &ColliderManager::Sphere_AABB;
	collisionTable_[(int)ColliderType::AABB][(int)ColliderType::Sphere] = &ColliderManager::AABB_Sphere;
	collisionTable_[(int)ColliderType::OBB][(int)ColliderType::Sphere] = &ColliderManager::OBB_Sphere;
	collisionTable_[(int)ColliderType::Sphere][(int)ColliderType::OBB] = &ColliderManager::Sphere_OBB;

	collisionTable_[(int)ColliderType::AABB][(int)ColliderType::OBB] = &ColliderManager::AABB_OBB;
	collisionTable_[(int)ColliderType::OBB][(int)ColliderType::AABB] = &ColliderManager::OBB_AABB;
}

///-------------------------------------------/// 
/// リセット
///-------------------------------------------///
void ColliderManager::Reset() {
	// リストを空っぽにする
	colliders_.clear();
}

///-------------------------------------------/// 
/// 削除
///-------------------------------------------///
void ColliderManager::RemoveCollider(Collider* collider) {
	//引数コライダーを削除
	colliders_.remove(collider);
}

///-------------------------------------------/// 
/// コライダーの追加
///-------------------------------------------///
void ColliderManager::AddCollider(Collider* collider) {
	// 引数のコライダーをpush_back
	colliders_.push_back(collider);
}

///-------------------------------------------/// 
/// ペアの当たり判定
///-------------------------------------------///
void ColliderManager::CheckPairCollision(Collider* colliderA, Collider* colliderB) {
	int typeA = static_cast<int>(colliderA->GetColliderType());
	int typeB = static_cast<int>(colliderB->GetColliderType());

	// 登録されている関数を取得
	auto func = collisionTable_[typeA][typeB];
	if (!func) {
		// 関数が登録されていない場合は何もしない
		return;
	}

	// 衝突判定の結果を受け取る
	bool isHit = (this->*func)(colliderA, colliderB);


	// Colliderが持っているフラグを変更
	colliderA->SetIsCollisison(isHit);
	colliderB->SetIsCollisison(isHit);

	if (isHit) {
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	}
}

///-------------------------------------------/// 
/// 全てのColliderの当たり判定
///-------------------------------------------///
void ColliderManager::CheckAllCollisions() {
	for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA) {
		auto itB = itA;
		++itB;
		for (; itB != colliders_.end(); ++itB) {
			CheckPairCollision(*itA, *itB);
		}
	}
}

///-------------------------------------------/// 
/// 衝突判定関数
///-------------------------------------------///
bool ColliderManager::Sphere_Sphere(Collider* a, Collider* b) { return SphereToSphereCollision(static_cast<SphereCollider*>(a), static_cast<SphereCollider*>(b)); }
bool ColliderManager::AABB_AABB(Collider* a, Collider* b) { return AABBToAABBCollision(static_cast<AABBCollider*>(a), static_cast<AABBCollider*>(b)); }
bool ColliderManager::OBB_OBB(Collider* a, Collider* b) { return OBBToOBBCollision(static_cast<OBBCollider*>(a), static_cast<OBBCollider*>(b)); }
bool ColliderManager::Sphere_AABB(Collider* a, Collider* b) { return SphereToAABBCollision(static_cast<SphereCollider*>(a), static_cast<AABBCollider*>(b)); }
bool ColliderManager::AABB_Sphere(Collider* a, Collider* b) { return SphereToAABBCollision(static_cast<SphereCollider*>(b), static_cast<AABBCollider*>(a)); }
bool ColliderManager::AABB_OBB(Collider* a, Collider* b) { return AABBToOBBCollision(static_cast<AABBCollider*>(a), static_cast<OBBCollider*>(b)); }
bool ColliderManager::OBB_AABB(Collider* a, Collider* b) { return AABBToOBBCollision(static_cast<AABBCollider*>(b), static_cast<OBBCollider*>(a)); }
bool ColliderManager::Sphere_OBB(Collider* a, Collider* b) { return SphereToOBBCollision(static_cast<SphereCollider*>(a), static_cast<OBBCollider*>(b)); }
bool ColliderManager::OBB_Sphere(Collider* a, Collider* b) { return SphereToOBBCollision(static_cast<SphereCollider*>(b), static_cast<OBBCollider*>(a)); }

///-------------------------------------------/// 
/// 当たり判定の関数
///-------------------------------------------///
// 球と球
bool ColliderManager::SphereToSphereCollision(SphereCollider* a, SphereCollider* b) {
	Sphere s1 = a->GetSphere();
	Sphere s2 = b->GetSphere();

	Vector3 diff = s1.center - s2.center;
	float distSq = Dot(diff);
	float radiusSum = s1.radius + s2.radius;

	// 判定
	if (distSq <= (radiusSum * radiusSum)) {
		return true;
	} else {
		return false;
	}
}
// AABBとAABB
bool ColliderManager::AABBToAABBCollision(AABBCollider* a, AABBCollider* b) {
	AABB aabb1 = a->GetAABB();
	AABB aabb2 = b->GetAABB();

	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);

	// x軸方向の判定
	if (aabb1.max.x < aabb2.min.x || aabb1.max.x > aabb2.max.x) {
		return false;
	}
	// y軸方向の判定
	if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) {
		return false;
	}
	// z軸方向の判定
	if (aabb1.max.z < aabb2.min.z || aabb1.min.z > aabb2.max.z) {
		return false;
	}

	// 全ての軸が重なっている場合は衝突
	return true;
}
// OBBとOBB
bool ColliderManager::OBBToOBBCollision(OBBCollider* a, OBBCollider* b) {
	OBB aCol = a->GetOBB();
	OBB bCol = b->GetOBB();

	return OBBSATCollision(aCol, bCol);
}
// 球とAABB
bool ColliderManager::SphereToAABBCollision(SphereCollider* sphere, AABBCollider* aabb) {
	Sphere spherecol = sphere->GetSphere();
	AABB aabbcol = aabb->GetAABB();

	Vector3 closestPoint = {
		std::clamp(spherecol.center.x, aabbcol.min.x, aabbcol.max.x),
		std::clamp(spherecol.center.y, aabbcol.min.y, aabbcol.max.y),
		std::clamp(spherecol.center.z, aabbcol.min.z, aabbcol.max.z)
	};

	Vector3 diff = spherecol.center - closestPoint;

	// 衝突の判定
	if (Dot(diff) <= (spherecol.radius * spherecol.radius)) {
		return true;
	} else {
		return false;
	}
}
// AABBとOBB
bool ColliderManager::AABBToOBBCollision(AABBCollider* aabb, OBBCollider* obb) {
	AABB aabbCol = aabb->GetAABB();
	OBB oobbColbb = obb->GetOBB();

	// AABBをOBBに変換
	OBB fakeAABB;
	fakeAABB.center = (aabbCol.min + aabbCol.max) * 0.5f;
	fakeAABB.halfSize = (aabbCol.max - aabbCol.min) * 0.5f;
	fakeAABB.axis[0] = { 1.0f, 0.0f, 0.0f };
	fakeAABB.axis[1] = { 0.0f, 1.0f, 0.0f };
	fakeAABB.axis[2] = { 0.0f, 0.0f, 1.0f };

	return OBBSATCollision(fakeAABB, oobbColbb);
}
// 球とOBB
bool ColliderManager::SphereToOBBCollision(SphereCollider* sphere, OBBCollider* obb) {
	Sphere sphereCol = sphere->GetSphere();
	OBB obbCol = obb->GetOBB();

	Vector3 dir = sphereCol.center - obbCol.center;
	Vector3 closest = obbCol.center;

	// OBB空間における最近点を計算
	for (int i = 0; i < 3; ++i) {
		float axisExtent = 0.0f;

		if (i == 0) axisExtent = obbCol.halfSize.x;
		else if (i == 1) axisExtent = obbCol.halfSize.y;
		else if (i == 2) axisExtent = obbCol.halfSize.z;

		float dist = Dot(dir, obbCol.axis[i]);
		dist = std::clamp(dist, -axisExtent, axisExtent);

		closest += obbCol.axis[i] * dist;
	}

	Vector3 diff = sphereCol.center - closest;

	// 衝突判定
	if (Dot(diff) <= (sphereCol.radius * sphereCol.radius)) {
		return true;
	} else {
		return false;
	}
}

///-------------------------------------------/// 
/// SATによるOBBvsOBB判定
///-------------------------------------------///
bool ColliderManager::OBBSATCollision(const OBB& a, const OBB& b) {
	const float EPSILON = 1e-6f;

	Vector3 axes[15];
	int axisCount = 0;

	// 3軸 + 3軸
	for (int i = 0; i < 3; ++i) {
		axes[axisCount++] = a.axis[i];
		axes[axisCount++] = b.axis[i];
	}

	// 交差軸
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			Vector3 cross = Math::Cross(a.axis[i], b.axis[j]);
			if (Dot(cross) > EPSILON) {
				axes[axisCount++] = Normalize(cross);
			}
		}
	}

	for (int i = 0; i < axisCount; ++i) {
		const Vector3& axis = axes[i];

		float aMin, aMax, bMin, bMax;
		ProjectOBBOntoAxis(a, axis, aMin, aMax);
		ProjectOBBOntoAxis(b, axis, bMin, bMax);

		if (aMax < bMin || bMax < aMin) {
			return false; // 分離軸あり → 衝突していない
		}
	}

	return true; // 全軸で重なりあり → 衝突
}
void ColliderManager::ProjectOBBOntoAxis(const OBB& obb, const Vector3& axis, float& outMin, float& outMax) {
	float centerProjection = Dot(obb.center, axis);

	float extents =
		std::abs(Dot(obb.axis[0] * obb.halfSize.x, axis)) +
		std::abs(Dot(obb.axis[1] * obb.halfSize.y, axis)) +
		std::abs(Dot(obb.axis[2] * obb.halfSize.z, axis));

	outMin = centerProjection - extents;
	outMax = centerProjection + extents;
}
