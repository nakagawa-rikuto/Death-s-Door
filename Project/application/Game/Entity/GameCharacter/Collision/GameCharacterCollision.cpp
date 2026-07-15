#include "GameCharacterCollision.h"
// GameCharacter
#include "application/Game/Entity/GameCharacter/GameCharacter.h"
// C++
#include <algorithm>
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// テンプレート候補の明示的インスタンス化
///-------------------------------------------///
// 球体とOBBのGameCharacter衝突処理
template void GameCharacterCollision::HandleSphereOBBCollision<MiiEngine::SphereCollider, MiiEngine::OBBCollider>(GameCharacter<MiiEngine::SphereCollider>*, GameCharacter<MiiEngine::OBBCollider>*, float);
template void GameCharacterCollision::HandleSphereOBBCollision<MiiEngine::OBBCollider, MiiEngine::SphereCollider>(GameCharacter<MiiEngine::OBBCollider>*, GameCharacter<MiiEngine::SphereCollider>*, float);
// 汎用衝突処理
template void GameCharacterCollision::ProcessCollision<MiiEngine::SphereCollider, MiiEngine::SphereCollider>(GameCharacter<MiiEngine::SphereCollider>*, GameCharacter<MiiEngine::SphereCollider>*, float);
template void GameCharacterCollision::ProcessCollision<MiiEngine::SphereCollider, MiiEngine::OBBCollider>(GameCharacter<MiiEngine::SphereCollider>*, GameCharacter<MiiEngine::OBBCollider>*, float);
template void GameCharacterCollision::ProcessCollision<MiiEngine::OBBCollider, MiiEngine::OBBCollider>(GameCharacter<MiiEngine::OBBCollider>*, GameCharacter<MiiEngine::OBBCollider>*, float);
template void GameCharacterCollision::ProcessCollision<MiiEngine::OBBCollider, MiiEngine::SphereCollider>(GameCharacter<MiiEngine::OBBCollider>*, GameCharacter<MiiEngine::SphereCollider>*, float);

///-------------------------------------------/// 
/// 球体GameCharacter同士の衝突処理
///-------------------------------------------///
void GameCharacterCollision::HandleSphereSphereCollision(GameCharacter<MiiEngine::SphereCollider>* characterA, GameCharacter<MiiEngine::SphereCollider>* characterB, const float pushBackRatio) {
	// 位置情報の取得
	Vector3 posA = characterA->GetTransform().translate;
	Vector3 posB = characterB->GetTransform().translate;

	// 中心点間の距離ベクトル
	float distance = 0.0f;
	Vector3 direction = CalculatePushBackDirection(posA, posB, Vector3(1.0f, 0.0f, 0.0f), EPSILON_, distance);

	// 半径の合計
	float radiusA = characterA->GetSphere().radius;
	float radiusB = characterB->GetSphere().radius;
	float totalRadius = radiusA + radiusB;

	// 重なり分
	float overlap = totalRadius - distance;

	// 押し戻し処理
	if (ApplyPushBack(posA, posB, direction, overlap, pushBackRatio)) {
		// 位置更新
		characterA->SetTranslate(posA);
		characterB->SetTranslate(posB);
	}
}

///-------------------------------------------/// 
/// OBBGameCharacter同士の衝突処理
///-------------------------------------------///
void GameCharacterCollision::HandleOBBOBBCollision(GameCharacter<MiiEngine::OBBCollider>* characterA, GameCharacter<MiiEngine::OBBCollider>* characterB, const float pushBackRatio) {
	// 位置情報の取得
	Vector3 posA = characterA->GetTransform().translate;
	Vector3 posB = characterB->GetTransform().translate;

	/// 中心間の距離ベクトル
	float distance = 0.0f;
	Vector3 direction = CalculatePushBackDirection(posA, posB, Vector3(1.0f, 0.0f, 0.0f), EPSILON_, distance);

	// OBBのサイズから推定される最小押し戻し距離
	Vector3 sizeA = characterA->GetOBB().halfSize * 2.0f;
	Vector3 sizeB = characterB->GetOBB().halfSize * 2.0f;
	float avgSizeA = (sizeA.x + sizeA.y + sizeA.z) / 3.0f;
	float avgSizeB = (sizeB.x + sizeB.y + sizeB.z) / 3.0f;

	// 重なり分
	float minDistance = (avgSizeA + avgSizeB) * 0.5f;
	float overlap = minDistance - distance;

	// 押し戻し処理
	if (ApplyPushBack(posA, posB, direction, overlap, pushBackRatio)) {
		// 位置更新
		characterA->SetTranslate(posA);
		characterB->SetTranslate(posB);
	}
}

///-------------------------------------------/// 
/// 球体とOBBのGameCharacter衝突処理
///-------------------------------------------///
template<typename TColliderA, typename TColliderB> requires IsCollider<TColliderA>&& IsCollider<TColliderB>
void GameCharacterCollision::HandleSphereOBBCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, const float pushBackRatio) {

	// ===キャラクターの型を判定してキャスト=== ///
	GameCharacter<MiiEngine::OBBCollider>* obbCharacter = nullptr;
	GameCharacter<MiiEngine::SphereCollider>* sphereCharacter = nullptr;
	/// ===キャスト=== ///
	if (GetColliderTypeFromCharacter(characterA) == MiiEngine::ColliderType::OBB) {
		obbCharacter = dynamic_cast<GameCharacter<MiiEngine::OBBCollider>*>(characterA);
		sphereCharacter = dynamic_cast<GameCharacter<MiiEngine::SphereCollider>*>(characterB);

	} else if (GetColliderTypeFromCharacter(characterA) == MiiEngine::ColliderType::Sphere) {
		sphereCharacter = dynamic_cast<GameCharacter<MiiEngine::SphereCollider>*>(characterA);
		obbCharacter = dynamic_cast<GameCharacter<MiiEngine::OBBCollider>*>(characterB);
	}

	/// ===Nullチェック=== ///
	if (!obbCharacter || !sphereCharacter) {
		return;
	}

	// 位置情報の取得
	Vector3 obbPos = obbCharacter->GetTransform().translate;
	Vector3 spherePos = sphereCharacter->GetTransform().translate;

	// 球の半径
	float sphereRadius = sphereCharacter->GetSphere().radius;
	Vector3 closestPoint = CalculateClosestPointOnOBBFromCharacter(spherePos, obbCharacter);

	// 球体の中心から最も近い点への方向
	float distance = 0.0f;
	Vector3 direction = CalculatePushBackDirection(spherePos, closestPoint, Vector3(0.0f, 1.0f, 0.0f), EPSILON_, distance);

	// 重なり分
	float overlap = sphereRadius - distance;

	// 押し戻し処理
	if (ApplyPushBack(spherePos, obbPos, direction, overlap, pushBackRatio)) {
		// 位置更新
		obbCharacter->SetTranslate(obbPos);
		sphereCharacter->SetTranslate(spherePos);
	}
}

///-------------------------------------------/// 
/// 汎用衝突処理（自動判定）
///-------------------------------------------///
template<typename TColliderA, typename TColliderB> requires IsCollider<TColliderA>&& IsCollider<TColliderB>
void GameCharacterCollision::ProcessCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, const float pushBackRatio) {

	MiiEngine::ColliderType typeA = GetColliderTypeFromCharacter(characterA);
	MiiEngine::ColliderType typeB = GetColliderTypeFromCharacter(characterB);

	// 同じ型の場合
	if (typeA == typeB) {

		if (typeA == MiiEngine::ColliderType::Sphere) {
			GameCharacter<MiiEngine::SphereCollider>* sphereA = dynamic_cast<GameCharacter<MiiEngine::SphereCollider>*>(characterA);
			GameCharacter<MiiEngine::SphereCollider>* sphereB = dynamic_cast<GameCharacter<MiiEngine::SphereCollider>*>(characterB);
			HandleSphereSphereCollision(sphereA, sphereB, pushBackRatio);
		} else if (typeA == MiiEngine::ColliderType::OBB) {
			GameCharacter<MiiEngine::OBBCollider>* obbA = dynamic_cast<GameCharacter<MiiEngine::OBBCollider>*>(characterA);
			GameCharacter<MiiEngine::OBBCollider>* obbB = dynamic_cast<GameCharacter<MiiEngine::OBBCollider>*>(characterB);
			HandleOBBOBBCollision(obbA, obbB, pushBackRatio);
		}
		// 同じでない場合
	} else if (typeA != typeB) {
		HandleSphereOBBCollision(characterA, characterB, pushBackRatio);
	}
}

///-------------------------------------------/// 
/// 押し戻し処理の汎用関数
///-------------------------------------------///
Vector3 GameCharacterCollision::CalculatePushBackDirection(const Vector3& from, const Vector3& to, const Vector3& fallbackDirection, float epsilon, float& outDistance) const {
	Vector3 direction = from - to;
	float distance = Length(direction);
	// 距離が0に近い場合は適当な方向に設定	
	if (distance < epsilon) {
		direction = fallbackDirection;
		distance = epsilon;
	} else {
		direction = Normalize(direction);
	}
	outDistance = distance;
	return direction;
}
///-------------------------------------------///	
/// 押し戻し処理の適用（共通処理）	
///-------------------------------------------///	
bool GameCharacterCollision::ApplyPushBack(Vector3& posA, Vector3& posB, const Vector3& direction, float overlap, float pushBackRatio) const {
	if (overlap <= 0.0f) {
		return false;
	}
	// 押し戻し処理	
	float pushBackDistanceA = overlap * pushBackRatio;
	float pushBackDistanceB = overlap * (1.0f - pushBackRatio);
	posA += direction * pushBackDistanceA;
	posB -= direction * pushBackDistanceB;
	return true;
}

///-------------------------------------------/// 
/// コライダー型の判定
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
MiiEngine::ColliderType GameCharacterCollision::GetColliderTypeFromCharacter(GameCharacter<TCollider>* character) const {
	return character->GetColliderType();
}

///-------------------------------------------/// 
/// OBB上の最近点計算（GameCharacter用）
///-------------------------------------------///
Vector3 GameCharacterCollision::CalculateClosestPointOnOBBFromCharacter(const Vector3& point, GameCharacter<MiiEngine::OBBCollider>* obbCharacter) const {
	if (obbCharacter->GetColliderType() != MiiEngine::ColliderType::OBB) {
		return point; // OBBでない場合は元の点を返す
	}

	MiiEngine::OBBCollider* obb = static_cast<MiiEngine::OBBCollider*>(obbCharacter);
	MiiEngine::OBB obbData = obb->GetOBB();

	return CalculateClosestPointOnOBB(point, obbData);
}

///-------------------------------------------/// 
/// OBB上の最近点計算
///-------------------------------------------///
Vector3 GameCharacterCollision::CalculateClosestPointOnOBB(const Vector3& point, const MiiEngine::OBB& obb) const {
	Vector3 dir = point - obb.center;
	Vector3 closest = obb.center;

	// OBB空間における最近点を計算
	for (int i = 0; i < 3; ++i) {
		float axisExtent = 0.0f;

		if (i == 0) axisExtent = obb.halfSize.x;
		else if (i == 1) axisExtent = obb.halfSize.y;
		else if (i == 2) axisExtent = obb.halfSize.z;

		float dist = Dot(dir, obb.axis[i]);
		dist = std::clamp(dist, -axisExtent, axisExtent);

		closest += obb.axis[i] * dist;
	}

	return closest;
}

///-------------------------------------------/// 
/// OBBの軸上での重なりを計算
///-------------------------------------------///
float GameCharacterCollision::CalculateOverlapOnAxis(const MiiEngine::OBB& obb1, const MiiEngine::OBB& obb2, const Vector3& axis) const {
	// 各OBBの軸への射影の半径を計算
	float r1 = 0.0f;
	for (int i = 0; i < 3; ++i) {
		r1 += fabs(Dot(obb1.axis[i] * obb1.halfSize, axis));
	}

	float r2 = 0.0f;
	for (int i = 0; i < 3; ++i) {
		r2 += fabs(Dot(obb2.axis[i] * obb2.halfSize, axis));
	}

	// 中心間距離の射影
	Vector3 centerDiff = obb1.center - obb2.center;
	float distance = fabs(Dot(centerDiff, axis));

	// 重なり量を返す（正の値なら重なっている）
	return (r1 + r2) - distance;
}