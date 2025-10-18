#include "GameCharacterCollision.h"
#include "GameCharacter.h"
#include "Engine/Collider/SphereCollider.h"
#include "Engine/Collider/OBBCollider.h"
#include <algorithm>

///-------------------------------------------/// 
/// テンプレート候補の明示的インスタンス化
///-------------------------------------------///
// 球体とOBBのGameCharacter衝突処理
template void GameCharacterCollision::HandleSphereOBBCollision<SphereCollider, OBBCollider>(GameCharacter<SphereCollider>*, GameCharacter<OBBCollider>*, float);
template void GameCharacterCollision::HandleSphereOBBCollision<OBBCollider, SphereCollider>(GameCharacter<OBBCollider>*, GameCharacter<SphereCollider>*, float);
// 汎用衝突処理
template void GameCharacterCollision::ProcessCollision<SphereCollider, SphereCollider>(GameCharacter<SphereCollider>*, GameCharacter<SphereCollider>*, float);
template void GameCharacterCollision::ProcessCollision<SphereCollider, OBBCollider>(GameCharacter<SphereCollider>*, GameCharacter<OBBCollider>*, float);
template void GameCharacterCollision::ProcessCollision<OBBCollider, OBBCollider>(GameCharacter<OBBCollider>*, GameCharacter<OBBCollider>*, float);
template void GameCharacterCollision::ProcessCollision<OBBCollider, SphereCollider>(GameCharacter<OBBCollider>*, GameCharacter<SphereCollider>*, float);

///-------------------------------------------/// 
/// 球体GameCharacter同士の衝突処理
///-------------------------------------------///
void GameCharacterCollision::HandleSphereSphereCollision(GameCharacter<SphereCollider>* characterA, GameCharacter<SphereCollider>* characterB, float pushBackRatio) {

	Vector3 posA = characterA->GetTransform().translate;
	Vector3 posB = characterB->GetTransform().translate;

	// 中心点間の距離ベクトル
	Vector3 direction = posA - posB;
	float distance = Length(direction);

	// 距離が0に近い場合は適当な方向に設定
	if (distance < 1e-6f) {
		direction = Vector3(1.0f, 0.0f, 0.0f);
		distance = 1e-6f;
	} else {
		direction = Normalize(direction);
	}

	// 半径の合計
	float radiusA = characterA->GetSphere().radius;
	float radiusB = characterB->GetSphere().radius;
	float totalRadius = radiusA + radiusB;

	// 重なり分
	float overlap = totalRadius - distance;
	if (overlap > 0.0f) {
		// 押し戻し処理
		float pushBackDistanceA = overlap * pushBackRatio;
		float pushBackDistanceB = overlap * (1.0f - pushBackRatio);

		posA += direction * pushBackDistanceA;
		posB -= direction * pushBackDistanceB;

		characterA->SetTranslate(posA);
		characterB->SetTranslate(posB);
	}
}

///-------------------------------------------/// 
/// OBBGameCharacter同士の衝突処理
///-------------------------------------------///
void GameCharacterCollision::HandleOBBOBBCollision(GameCharacter<OBBCollider>* characterA, GameCharacter<OBBCollider>* characterB, float pushBackRatio) {

	Vector3 posA = characterA->GetTransform().translate;
	Vector3 posB = characterB->GetTransform().translate;

	// 簡略化された押し戻し処理（中心点間のベクトルを使用）
	Vector3 direction = posA - posB;
	float distance = Length(direction);

	if (distance < 1e-6f) {
		direction = Vector3(1.0f, 0.0f, 0.0f);
		distance = 1e-6f;
	} else {
		direction = Normalize(direction);
	}

	// OBBのサイズから推定される最小押し戻し距離
	Vector3 sizeA = characterA->GetOBB().halfSize * 2.0f;
	Vector3 sizeB = characterB->GetOBB().halfSize * 2.0f;
	float avgSizeA = (sizeA.x + sizeA.y + sizeA.z) / 3.0f;
	float avgSizeB = (sizeB.x + sizeB.y + sizeB.z) / 3.0f;

	float minDistance = (avgSizeA + avgSizeB) * 0.5f;
	float overlap = minDistance - distance;

	if (overlap > 0.0f) {
		// 押し戻し処理
		float pushBackDistanceA = overlap * pushBackRatio;
		float pushBackDistanceB = overlap * (1.0f - pushBackRatio);

		posA += direction * pushBackDistanceA;
		posB -= direction * pushBackDistanceB;

		characterA->SetTranslate(posA);
		characterB->SetTranslate(posB);
	}
}

///-------------------------------------------/// 
/// 球体とOBBのGameCharacter衝突処理
///-------------------------------------------///
template<typename TColliderA, typename TColliderB>
void GameCharacterCollision::HandleSphereOBBCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, float pushBackRatio) {

	GameCharacter<OBBCollider>* obbCharacter = nullptr;
	GameCharacter<SphereCollider>* sphereCharacter = nullptr;
	/// ===キャスト=== ///
	if (GetColliderTypeFromCharacter(characterA) == ColliderType::OBB) {
		obbCharacter = dynamic_cast<GameCharacter<OBBCollider>*>(characterA);
		sphereCharacter = dynamic_cast<GameCharacter<SphereCollider>*>(characterB);

	} else if (GetColliderTypeFromCharacter(characterA) == ColliderType::Sphere) {
		sphereCharacter = dynamic_cast<GameCharacter<SphereCollider>*>(characterA);
		obbCharacter = dynamic_cast<GameCharacter<OBBCollider>*>(characterB);
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
	Vector3 direction = spherePos - closestPoint;
	float distance = Length(direction);

	// 距離が0に近い場合は適当な方向に設定
	if (distance < 1e-6f) {
		direction = Vector3(0.0f, 1.0f, 0.0f);
		distance = 1e-6f;
	} else {
		direction = Normalize(direction);
	}

	// 重なり分
	float overlap = sphereRadius - distance;
	if (overlap > 0.0f) {
		// 押し戻し処理
		float pushBackDistanceSphere = overlap * pushBackRatio;
		float pushBackDistanceOBB = overlap * (1.0f - pushBackRatio);

		spherePos += direction * pushBackDistanceSphere;
		obbPos -= direction * pushBackDistanceOBB;

		// 位置の更新
		obbCharacter->SetTranslate(obbPos);
		sphereCharacter->SetTranslate(spherePos);
	}
}

///-------------------------------------------/// 
/// 汎用衝突処理（自動判定）
///-------------------------------------------///
template<typename TColliderA, typename TColliderB>
void GameCharacterCollision::ProcessCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, float pushBackRatio) {

	ColliderType typeA = GetColliderTypeFromCharacter(characterA);
	ColliderType typeB = GetColliderTypeFromCharacter(characterB);

	// 同じ型の場合
	if (typeA == typeB) {

		if (typeA == ColliderType::Sphere) {
			GameCharacter<SphereCollider>* sphereA = dynamic_cast<GameCharacter<SphereCollider>*>(characterA);
			GameCharacter<SphereCollider>* sphereB = dynamic_cast<GameCharacter<SphereCollider>*>(characterB);
			HandleSphereSphereCollision(sphereA, sphereB, pushBackRatio);
		} else if (typeA == ColliderType::OBB) {
			GameCharacter<OBBCollider>* obbA = dynamic_cast<GameCharacter<OBBCollider>*>(characterA);
			GameCharacter<OBBCollider>* obbB = dynamic_cast<GameCharacter<OBBCollider>*>(characterB);
			HandleOBBOBBCollision(obbA, obbB, pushBackRatio);
		}
	// 同じでない場合
	} else if (typeA != typeB) {
		HandleSphereOBBCollision(characterA, characterB, pushBackRatio);
	}

}

///-------------------------------------------/// 
/// コライダー型の判定
///-------------------------------------------///
template<typename TCollider>
ColliderType GameCharacterCollision::GetColliderTypeFromCharacter(GameCharacter<TCollider>* character) {
	return character->GetColliderType();
}

///-------------------------------------------/// 
/// OBB上の最近点計算
///-------------------------------------------///
Vector3 GameCharacterCollision::CalculateClosestPointOnOBBFromCharacter(const Vector3& point, GameCharacter<OBBCollider>* obbCharacter) {
	if (obbCharacter->GetColliderType() != ColliderType::OBB) {
		return point; // OBBでない場合は元の点を返す
	}

	OBBCollider* obb = static_cast<OBBCollider*>(obbCharacter);
	OBB obbData = obb->GetOBB();

	Vector3 dir = point - obbData.center;
	Vector3 closest = obbData.center;

	// OBB空間における最近点を計算
	for (int i = 0; i < 3; ++i) {
		float axisExtent = 0.0f;

		if (i == 0) axisExtent = obbData.halfSize.x;
		else if (i == 1) axisExtent = obbData.halfSize.y;
		else if (i == 2) axisExtent = obbData.halfSize.z;

		float dist = Dot(dir, obbData.axis[i]);
		dist = std::clamp(dist, -axisExtent, axisExtent);

		closest += obbData.axis[i] * dist;
	}

	return closest;
}