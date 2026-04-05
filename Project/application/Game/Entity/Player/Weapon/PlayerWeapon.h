#pragma once
/// ===Include=== ///
// OBB
#include "Engine/Collider/OBBCollider.h"
// AttackData
#include <Engine/System/Editor/Attack/Data/AttackData.h>
#include "Engine/DataInfo/LineObjectData.h"

/// ===前方宣言=== ///
namespace MiiEngine {
	class ParticleGroup;
}
class Player;

///=====================================================/// 
/// PlayerWeapon
///=====================================================///
class PlayerWeapon : public MiiEngine::OBBCollider {
public:
	PlayerWeapon() = default;
	~PlayerWeapon() override;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。</param>
	void Draw(MiiEngine::BlendMode mode) override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information() override;

	/// <summary>
	/// 親子関係を設定し、指定したオフセットで子オブジェクトの位置を調整
	/// </summary>
	/// <param name="parent">親となる Player オブジェクトへのポインタ。子をこの親に関連付けます。</param>
	void SetUpParent(Player* parent);

	/// <summary>
	/// 攻撃を開始する（ベジェ曲線）
	/// </summary>
	/// <param name="trajectoryPoints">ベジェ曲線の制御点リスト</param>
	/// <param name="duration">攻撃にかける時間（秒）</param>
	/// <param name="startRotation">開始時の回転</param>
	/// <param name="endRotation">終了時の回転</param>
	void StartAttack(
		const std::vector<MiiEngine::BezierControlPointData>& trajectoryPoints,
		float duration
	);


public: /// ===衝突=== ///
	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collider">衝突した相手の Collider オブジェクトを指すポインター。</param>
	void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===Getter=== ///
	// 攻撃中かどうか
	bool GetIsAttack() const { return attackInfo_.isAttacking; }
	// 攻撃の進行度を取得
	float GetAttackProgress() const { return attackInfo_.progress; }

public: /// ===Setter=== ///

	// アクティブ状態の設定
	void SetActive(bool flag) { baseInfo_.isActive = flag; }

private:
	// Player
	Player* player_ = nullptr;

	// Particle
	MiiEngine::ParticleGroup* attackParticle_ = nullptr;

	/// ===基本情報=== ///
	struct BaseInfo {
		bool isActive = false;					 // アクティブフラグ
		float deltaTime = 0.0f;					 // デルタタイム
	};
	BaseInfo baseInfo_;

	/// ===攻撃情報=== ///
	struct AttackInfo {
		bool isAttacking = false;      // 攻撃中フラグ
		bool hasHit = false;            // ヒット済みフラグ

		float timer = 0.0f;             // 攻撃タイマー
		float duration = 0.0f;          // 攻撃の持続時間
		float progress = 0.0f;          // 攻撃の進行度（0.0～1.0）

		// ベジェ曲線の制御点
		std::vector<MiiEngine::BezierControlPointData> trajectoryPoints;
	};
	AttackInfo attackInfo_;

private:
	/// <summary>
	/// ベジェ曲線上の位置を計算
	/// </summary>
	/// <param name="controlPoints">制御点のリスト</param>
	/// <param name="t">進行度（0.0～1.0）</param>
	/// <returns>ベジェ曲線上の位置</returns>
	Vector3 CalculateBezierPoint(const std::vector<MiiEngine::BezierControlPointData>& controlPoints, float t);

	/// <summary>
	/// ベジェ曲線上の回転を計算
	/// </summary>
	/// <param name="controlPoints">制御点のリスト</param>
	/// <param name="t">進行度（0.0～1.0）</param>
	/// <returns>ベジェ曲線上の回転</returns>
	Quaternion CalculateBezierRotation(const std::vector<MiiEngine::BezierControlPointData>& controlPoints, float t);

	/// <summary>
	/// ベジェ曲線に沿った攻撃軌道の更新処理
	/// </summary>
	void UpdateBezierTrajectory();
};