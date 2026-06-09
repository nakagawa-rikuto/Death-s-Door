#include "BossOrbitingOrbsCalculator.h"
// Component
#include "application/Game/Entity/Enemy/BossEnemy/Component/BossComponent.h"
// C++
#include <cassert>
#include <cmath>
#include <algorithm>
// Math
#include <Math/sMath.h>

// ===定数定義=== ///
namespace Orbit {
	BossComponent::OrbitingAttackComponent component_;
	const float kDegToRad = Math::Pi() / 180.0f;
	const float kTwoPi = Math::Pi() * 2.0f;
	const float kOrbSpacingRad = kTwoPi / static_cast<float>(component_.bulletCount);
} 

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void BossOrbitingOrbsCalculator::Initialize(const BossComponent::OrbitingAttackComponent& component) {
	// コンポーネントの取得
	Orbit::component_ = component;

	// 弾の情報ベクターをコンポーネントの弾数に合わせてリサイズ
	state_.bullets.resize(Orbit::component_.bulletCount);
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BossOrbitingOrbsCalculator::Update(float deltaTime) {
	// 非アクティブ時は何もしない
	if (phase_ == Phase::Idle || phase_ == Phase::Finished) return;

	/// ===タイマーの更新=== ///
	state_.orbitTimer += deltaTime;
	state_.lifeTimer += deltaTime;

	/// ===各弾の位置・方向の計算=== ///
	for (int i = 0; i < Orbit::component_.bulletCount; ++i) {
		// 公転角度の計算
		const float angleRad = CalcOrbitAngleRad(i);

		// 現在位置の計算
		Vector3 currentPos = CalcOrbitPosition(bossPosition_, angleRad);

		// 次フレームの向きの計算
		Vector3 dir = CalcOrbitDirection(bossPosition_, currentPos, angleRad, deltaTime);

		state_.bullets[i].position = currentPos;
		state_.bullets[i].direction = dir;
	}

	/// ===Finishedへの遷移=== ///
	if (state_.lifeTimer <= 0.0f) {
		phase_ = Phase::Finished;
	}
}

///-------------------------------------------/// 
/// 攻撃開始処理
///-------------------------------------------///
void BossOrbitingOrbsCalculator::StartAttack() {
	state_.orbitTimer = 0.0f;
	state_.lifeTimer = Orbit::component_.lifeTime;
	phase_ = Phase::Orbiting;
}

///-------------------------------------------/// 
/// 弾の公転角度を計算
///-------------------------------------------///
float BossOrbitingOrbsCalculator::CalcOrbitAngleRad(int index) const {
	// 全弾共通の基準角
	const float baseAngle = state_.orbitTimer * Orbit::component_.orbitSpeed * Orbit::kDegToRad;

	// 弾ごとの等間隔オフセット
	const float evenSpacing = static_cast<float>(index) * Orbit::kOrbSpacingRad;

	// 全体の初期スタート角
	const float initialAngle = Orbit::component_.initialAngle * Orbit::kDegToRad;

	// 最終的な角度
	return baseAngle + evenSpacing + initialAngle;
}

///-------------------------------------------/// 
/// 公転角度空ワールド座標を計算
///-------------------------------------------///
Vector3 BossOrbitingOrbsCalculator::CalcOrbitPosition(const Vector3& center, float angleRad) const {
	return Vector3{
		center.x + Orbit::component_.orbitRadius * std::cos(angleRad),
		center.y + Orbit::component_.orbitHeight,
		center.z + Orbit::component_.orbitRadius * std::sin(angleRad)
	};
}

///-------------------------------------------/// 
/// 次フレームの向きを現在位置と次の位置の差分から計算
///-------------------------------------------///
Vector3 BossOrbitingOrbsCalculator::CalcOrbitDirection(
	const Vector3& center,
	const Vector3& currentPos,
	float angleRad,
	float deltaTime) const {

	// 次フレームの角度と位置
	const float nextAngleRad = angleRad + Orbit::component_.orbitSpeed * Orbit::kDegToRad * deltaTime;
	const Vector3 nextPos = CalcOrbitPosition(center, nextAngleRad);
	// 差分ベクトル
	Vector3 dir = {
		nextPos.x - currentPos.x,
		nextPos.y - currentPos.y,
		nextPos.z - currentPos.z
	};

	// 正規化
	dir = Normalize(dir);

	return dir;
}
