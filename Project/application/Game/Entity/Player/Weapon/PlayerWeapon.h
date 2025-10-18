#pragma once
/// ===Include=== ///
#include "Engine/Collider/OBBCollider.h"

/// ===前方宣言=== ///
class Player;

///=====================================================/// 
/// PlayerWeapon
///=====================================================///
class PlayerWeapon : public OBBCollider {
public:
	PlayerWeapon() = default;
	~PlayerWeapon() override;

	// 初期化（Player）
	void InitPlayer(Player* player);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw(BlendMode mode) override;
	// ImGui
	void Information() override;

	// 攻撃処理
	//NOTE:Start, EndPoint = 攻撃の開始、終了地点, time = 時間
	void Attack(const Vector3& startPoint, const Vector3& endPoint, float time);

public: /// ===衝突=== ///
	void OnCollision(Collider* collider) override;

public: /// ===Setter, Getter=== ///
	// 攻撃フラグ
	bool GetIsAttack() const;

private:
	// Player
	Player* player_ = nullptr;

	/// ===基本情報=== ///
	struct BaseInfo {
		Vector3 offset_ = { 0.0f, 0.0f, 0.0f }; // プレイヤーからのオフセット
		Vector3 velocity_ = { 0.0f, 0.0f, 0.0f }; // 速度
	};
	BaseInfo baseInfo_;

	/// ===攻撃情報=== ///
	struct AttackInfo {
		bool isAttack = false;
	};

private: 

	// 攻撃の回転を計算
	//NOTE: Start, EndPoint = 回転の最初と最後の地点、 time = 時間
	void CalculateAttackRotation(const Vector3& startPoint, const Vector3& endPoint, float time);
};

