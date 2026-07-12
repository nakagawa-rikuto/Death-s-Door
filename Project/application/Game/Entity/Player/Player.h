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
// C++
#include <unordered_map>

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
	// Stick
	const Vector2& GetLeftStickValue() const { return leftStickValue_; }
	const Vector2& GetKeybordValue() const { return keybordValue_; }
	// 攻撃データをIDで取得できるようにする
	const AttackData* GetAttackData(int attackID) const;
	// 回避行動可能判定
	bool CanDodge() const { return coolDownInfo_.timer <= 0.0f; }
	// アクティブフラグの取得
	bool GetActiveAttackFlag() const { return activeInfo_.isAttacking; }
	bool GetActiveDodgeFlag() const { return activeInfo_.isDodge; }

public: /// ===Setter=== ///
	// Camera
	void SetCameraTargetPlayer();
	// 無敵時間の設定
	void SetInvincibleTime(const float& time);
	// Gravityの設定
	void SetGravity(const float& gravity) { baseInfo_.gravity = gravity; };
	// 回避行動のクールタイムの設定
	void SetDodgeCoolDown() { coolDownInfo_.timer = parameters_.dodge.coolTime; }
	// アクティブフラグの設定
	void SetActiveAttackFlag(bool isActive) { activeInfo_.isAttacking = isActive; }
	void SetActiveDodgeFlag(bool isDodge) { activeInfo_.isDodge = isDodge; }

public: /// ===State用関数=== ///

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

	/// ===攻撃データ=== ///
	std::unordered_map<int, AttackData> attackDataMap_;

	/// ===入力情報=== ///
	Vector2 leftStickValue_;  // 左スティックの入力値
	Vector2 keybordValue_;    // キーボードの入力値

	/// ===無敵時間の情報=== ///
	struct InvincibleInfo {
		float time = 1.0f;   // 無敵時間
		float timer = 0.0f;  // 無敵タイマー
		bool isFlag = false; // 無敵フラグ
	};
	InvincibleInfo invincibleInfo_;

	/// ===アクティブ情報=== ///
	struct ActiveInfo {
		bool isAttacking = false; // 攻撃中フラグ
		bool isDodge = false;     // 回避中フラグ
	};
	ActiveInfo activeInfo_;

	/// ===クールダウン情報=== ///
	struct CoolDownInfo {
		float timer = 0.0f;      // 回避タイマー
	};
	CoolDownInfo coolDownInfo_;

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
	/// 攻撃データをJSONから読み込みます。
	/// </summary>
	/// <param name="attackID">攻撃データのID。</param>
	/// <param name="filePath">JSONファイルのパス。</param>	
	void LoadAttackData(int attackID, const std::string& filePath);

	/// <summary>
	/// タイマーを進める関数
	/// </summary>
	void advanceTimer();

	/// <summary>
	/// エリアの衝突判定を処理します。
	/// </summary>
	void AreaCollision();
};