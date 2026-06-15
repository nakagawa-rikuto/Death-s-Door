#pragma once
/// ===Include=== ///
// GameCharacter
#include "application/Game/Entity/GameCharacter/GameCharacter.h"
// Component
#include "Component/PlayerComponent.h"
// State
#include "State/Base/PlayerState.h"
// Hand
#include "Hand/PlayerHand.h"
// Weapon
#include "Weapon/PlayerWeapon.h"

/// ===前方宣言=== ///
class Enemy;
namespace MiiEngine {
	class FollowCamera;
}

///=====================================================/// 
/// Player
///=====================================================///
class Player : public GameCharacter<MiiEngine::OBBCollider> {
public:

	Player() = default;
	~Player();

	/// <summary>
	/// ゲームの初期化処理
	/// </summary>
	/// <param name="translate">初期位置を表すベクトル。</param>
	void InitGame(const Vector3& translate, MiiEngine::FollowCamera* camera);

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// アニメーション時の更新処理
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode"></param>
	void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information() override;

public: /// ===衝突判定=== ///
	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collider">衝突した相手の Collider へのポインター。</param>
	void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===Getter=== ///
	// Hand
	PlayerHand* GetRightHand() const { return rightHand_.get(); };
	PlayerHand* GetLeftHand() const { return leftHand_.get(); };
	// Weapon
	PlayerWeapon* GetWeapon() const { return weapon_.get(); };
	// Component
	const PlayerComponent::Parameters& GetParameters() const { return parameters_; }
	// StickState
	Vector2 GetLeftStickState() const { return stickState_.leftStick; };
	Vector2 GetRightStickState() const { return stickState_.rightStick; };

public: /// ===Setter=== ///
	// Camera
	void SetCameraTargetPlayer();
	// 無敵時間の設定
	void SetInvincibleTime(const float& time);
	// Gravityの設定
	void SetGravity(const float& gravity) { baseInfo_.gravity = gravity; };

public: /// ===State用関数=== ///

	/// <summary>
	/// 減速処理
	/// </summary>
	/// <param name="deceleration">適用する減速量を表す浮動小数点数への const 参照。</param>
	void ApplyDeceleration(const float& deceleration);

	/// <summary>
	/// Stateの変更処理
	/// </summary>
	/// <param name="newState">新しい状態を表す std::unique_ptr<PlayerState>。この引数は呼び出し側からムーブされ、所有権は関数に移ります。</param>
	void ChangState(std::unique_ptr<PlayerState> newState);

private: /// ===変数の宣言=== ///
	// カメラ
	MiiEngine::FollowCamera* camera_ = nullptr;

	/// ===Hand=== ///
	std::unique_ptr<PlayerHand> rightHand_;
	std::unique_ptr<PlayerHand> leftHand_;

	/// ===Weapon=== ///
	std::unique_ptr<PlayerWeapon> weapon_;

	/// ===Component=== ///
	PlayerComponent::Parameters parameters_{};

	/// ===State=== ///
	std::unique_ptr<PlayerState> currentState_;

	/// ===無敵時間の情報=== ///
	struct InvincibleInfo {
		float time = 1.0f;   // 無敵時間
		float timer = 0.0f;  // 無敵タイマー
		bool isFlag = false; // 無敵フラグ
	};
	InvincibleInfo invincibleInfo_;

	/// ===StickState=== ///
	struct ControllerStickState {
		Vector2 leftStick = {0.0f, 0.0f};
		Vector2 rightStick = {0.0f, 0.0f};
	};
	ControllerStickState stickState_;

	/// ===AreaInfo=== ///
	struct AreaInfo {
		Vector3 center;    // 衝突エリアの中心位置
		Vector3 halfSize;  // 衝突エリアの半サイズ
	};
	AreaInfo areaInfo_;

private:

	/// <summary>
	/// パラメータの設定
	/// </summary>
	void SettingParamita();

	/// <summary>
	/// タイマーを進める関数
	/// </summary>
	void advanceTimer();

	/// <summary>
	/// エリアの衝突判定を処理します。
	/// </summary>
	void AreaCollision();
};