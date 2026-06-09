#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
// Math
#include <Math/Vector3.h>
#include <Math/Vector4.h>

///=====================================================/// 
/// BossHitReactionState
/// ヒットリアクションの状態
///=====================================================///
class BossHitReactionState : public BossState {
public:
	BossHitReactionState(const Vector3& hitDirection);
	~BossHitReactionState() override = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="enemy">状態に入るボスエネミーへのポインタ。</param>
	void Enter(BossEnemy* enemy) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

private:
	/// ===状態=== ///
	struct State {
		float slowdownTimer = 0.0f;	     // 減速用タイマー
		float slowdownMultiplier = 1.0f; // 減速率の乗数
		float colorTimer = 0.0f;    // 色変化用タイマー
		float colorInterval = 0.0f; // 色変化の間隔

		Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // ノックバックによる速度
	};
	State state_{};

	// ヒット方向
	Vector3 hitDirection_ = { 0.0f, 0.0f, 0.0f };

	// ヒットしたかのフラグ
	bool isHit_ = false;

	// 色変化中かのフラグ
	bool isColorChange_ = false;

private:

	/// <summary>
	/// タイマーの更新処理
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void UpdateTimer(const float deltaTime);

	/// <summary>
	/// 回転の更新処理
	/// </summary>
	void UpdateRotation();

	/// <summary>
	/// 減速係数の更新処理
	/// </summary>
	void UpdateSlowdownMultiplier();

	/// <summary>
	/// 渡された色を元に更新された色を返します。`
	/// </summary>
	/// <param name="color">更新の基となる色。参照渡しされた const Vector4 オブジェクト。</param>
	/// <returns>更新された色を表す Vector4 オブジェクト。</returns>
	Vector4 UpdateColor(const Vector4& currentColor) const;

	/// <summary>
	/// ノックバックの更新処理
	/// </summary>
	void UpdateKnockBack();
};

