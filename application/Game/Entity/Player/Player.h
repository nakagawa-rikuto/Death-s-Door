#pragma once
/// ===Include=== ///
// Collider
#include "Engine/Collider/OBBCollider.h"
// State
#include "State/Base/PlayerState.h"

/// ===前方宣言=== ///
class Enemy;

/// ===StateType=== ///
enum class actionType {
	kAvoidance,   // 回避状態
	kCharge,      // 突進状態
	kAttack       // 攻撃状態
};

///=====================================================/// 
/// Player
///=====================================================///
class Player : public OBBCollider {
public:

	Player() = default;
	~Player();

	// 初期化
	void Initialize()override;
	// 更新
	void Update()override;
	// 描画
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal)override;
	// ImGui
	void UpdateImGui();

public: /// ===衝突判定=== ///
	void OnCollision(Collider* collider) override;

public: /// ===Getter=== ///

	Vector3 GetTranslate()const;
	Quaternion GetRotate()const;
	Vector3 GetVelocity() const;
	float GetDeltaTime() const;

	// フラグ
	bool GetStateFlag(actionType type) const;
	bool GetpreparationFlag(actionType type) const;
	
	// タイマー
	float GetTimer(actionType type);

public: /// ===Setter=== ///

	void SetTranslate(const Vector3& translate);
	void SetRotate(const Quaternion& rotate);
	void SetVelocity(const Vector3& velocity);
	void SetVelocityX(const float& x);
	void SetVeloctiyY(const float& y);
	void SetVeloctiyZ(const float& z);

	// フラグ
	void SetStateFlag(actionType type, bool flag);
	void SetpreparationFlag(actionType type, bool flag);
	
	// タイマーの設定
	void SetTimer(actionType type, const float& timer);
	void SetInvicibleTime(const float& time);

public: /// ===State用関数=== ///

	// 減速処理
	void ApplyDeceleration(const float& develeration);

	// Stateの変更
	void ChangState(std::unique_ptr<PlayerState> newState);

private: /// ===変数の宣言=== ///

	Camera* camera_ = nullptr; // カメラ

	/// ===State=== ///
	std::unique_ptr<PlayerState> currentState_;

	/// ===基本情報=== ///
	struct BaseInfo {
		Vector3 translate = { 0.0f, 1.0f, 0.0f };
		Quaternion rotate = { 0.0f, 0.0f, 0.0f, 1.0f };
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		Vector4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
		Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	};
	BaseInfo baseInfo_;

	/// ===無敵時間の情報=== ///
	struct InvicibleInfo {
		float time = 1.0f;   // 無敵時間
		float timer = 0.0f;  // 無敵タイマー
		bool isFlag = false; // 無敵フラグ
	};
	InvicibleInfo invicibleInfo_;

	/// ===ChargeInfo=== ///
	struct ChargeInfo {
		float timer = 0.0f;
		bool isPreparation = false; // 突進の準備フラグ
		bool isFlag = false;        // 突進のフラグ
	};
	ChargeInfo chargeInfo_;

	/// ===回避情報=== ///
	struct AvoidanceInfo {
		float timer = 0.0f;         // 回避のタイマー
		bool isPreparation = false; // 回避の準備フラグ
		bool isFlag = false;        // 回避のフラグ
	};
	AvoidanceInfo avoidanceInfo_;

	/// ===攻撃情報=== ///
	struct AttackInfo {
		float timer = 0.0f;         // 攻撃のタイマー
		bool isPreparation = false; // 攻撃の準備フラグ
		bool isFlag = false;        // 攻撃のフラグ
	};
	AttackInfo attackInfo_;

	/// ===移動情報=== ///
	struct MoveInfo {
		float speed = 0.5f;               // 移動速度
		Vector3 direction = { 0.0f, 0.0f, 0.0f };
	};
	MoveInfo moveInfo_;

	// 時間の経過速度
	const float deltaTime_ = 1.0f / 60.0f;
private:

	// 時間を進める
	void advanceTimer();
};