#pragma once
/// ===Include=== ///
// GameCharacter
#include "application/Game/Entity/GameCharacter/GameCharacter.h"
// c++
#include <random>

/// ===前方宣言=== ///
namespace MiiEngine {
	class ParticleGroup;
}
class Player;

///=====================================================/// 
/// Enemy
///=====================================================///
class BaseEnemy : public GameCharacter<MiiEngine::OBBCollider> {
public:
	BaseEnemy() = default;
	virtual ~BaseEnemy();

	/// <summary>
	/// ゲームシーンで呼び出す初期化処理の純粋仮想関数
	/// </summary>
	/// <param name="translate">シーンの位置を指定する平行移動ベクトル。const参照で渡される。</param>
	virtual void InitGameScene(const Vector3& translate) = 0;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update()override = 0;

	/// <summary>
	/// アニメーション時の更新処理
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。既定値は BlendMode::KBlendModeNormal。</param>
	virtual void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	virtual void Information()override = 0;

	/// <summary>
	/// 回転の更新処理
	/// </summary>
	/// <param name="direction">回転目標の方向</param>
	/// <param name="lerpT">回転完了までの時間</param>
	void UpdateRotationTowards(const Vector3& direction, float lerpT);

public: /// ===衝突判定=== ///

	/// <summary>
    /// 衝突時の処理
    /// </summary>
    /// <param name="collider">衝突した相手を表す Collider へのポインター。</param>
	virtual void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===Getter=== ///
	// Player
	Player* GetPlayer() const { return player_; };
	// isRotationComplete
	bool GetIsRotationComplete() const { return isRotationComplete_; };
	// isTentativeDeath
	bool GetIsTentativeDeath() const { return isTentativeDeath_; };

public: /// ===Setter=== ///
	// Player
	void SetPlayer(Player* player) { player_ = player; };
	// isRotationComplete
	void SetIsRotationComplete(bool flag) { isRotationComplete_ = flag; };
	// isTentativeDeath
	void SetIsTentativeDeath(bool flag) { isTentativeDeath_ = flag; };
	// 無敵時間の設定
	void SetInvincibleTime();
	// Colliderの有効/無効の切り替え
	void SetColliderActive(bool isActive);

protected: /// ===変数の宣言=== ///

	MiiEngine::CameraCommon* camera_ = nullptr; // カメラ
	Player* player_ = nullptr; // Player

	bool isTentativeDeath_ = false; // 仮死亡フラグ

	/// ===無敵時間情報=== ///
	struct InvincibleInfo {
		float time = 0.5f;			// 無敵時間
		float timer = 0.0f;			// 無敵タイマー
		bool isInvincible = false;	// 無敵フラグ
	};
	InvincibleInfo invincibleInfo_; // 無敵時間情報

private:
	
	// 回転完了フラグ
	bool isRotationComplete_ = false;
};
