#pragma once
/// ===Include=== ///
// Math
#include "Math/Vector3.h"
// C++
#include <vector>

/// ===前方宣言=== ///
namespace BossComponent {
	struct OrbitingAttackComponent;
}

///=====================================================/// 
/// BossOrbitingOrbsCalculator
/// BossEnemyの周回する弾の位置と向きを計算するクラス
///=====================================================///
class BossOrbitingOrbsCalculator {
public:

	/// ===Bullet情報=== ///
	struct BulletInfo {
		Vector3 position{};   // ワールド座標
		Vector3 direction{};  // 移動方向
	};

public:

	BossOrbitingOrbsCalculator() = default;
	~BossOrbitingOrbsCalculator() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="component">軌道攻撃コンポーネント</param>
	void Initialize(const BossComponent::OrbitingAttackComponent& component);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void Update(float deltaTime);

	/// <summary>
	/// 攻撃開始処理
	/// </summary>
	void StartAttack();

public: /// ===Getter=== ///

	// 攻撃がアクティブかどうか
	bool IsActive() const { return phase_ == Phase::Orbiting; }
	// BulletInfoの取得
	const std::vector<BulletInfo> GetBulletInfo() const { return state_.bullets; }

private:

	/// ===フェーズ=== ///
	enum class Phase {
		Idle,       // 非アクティブ（待機）
		Orbiting,   // 公転中（生存タイマーが尽きるまで）
		Finished,   // 攻撃終了
	};
	Phase phase_ = Phase::Idle; // 現在のフェーズ


	/// ===状態=== ///
	struct State {
		float orbitTimer = 0.0f; // 公転中の経過時間
		float lifeTimer = 0.0f;  // 公転の生存時間
		std::vector<BulletInfo> bullets;
	};
	State state_{};

	// BossEnemyの場所
	Vector3 bossPosition_{};

private:

	/// <summary>
   /// 弾 index の公転角度（ラジアン）を計算する。
   /// </summary>
   /// <param name="index">弾のインデックス</param>
	float CalcOrbitAngleRad(int index) const;

	/// <summary>
	/// 公転角度からワールド座標を計算する。
	/// </summary>
	/// <param name="center">公転中心の座標</param>
	/// <param name="angleRad">現在の公転角度（ラジアン）</param>
	Vector3 CalcOrbitPosition(const Vector3& center, float angleRad) const;

	/// <summary>
	/// 次フレームの向きを現在位置と次位置の差分から計算する。
	/// </summary>
	/// <param name="center">公転中心の座標</param>
	/// <param name="currentPos">現在の弾の座標</param>
	/// <param name="angleRad">現在の公転角度（ラジアン）</param>
	/// <param name="deltaTime">前フレームからの経過時間（秒）</param>
	Vector3 CalcOrbitDirection(const Vector3& center, const Vector3& currentPos, float angleRad, float deltaTime) const;
};

