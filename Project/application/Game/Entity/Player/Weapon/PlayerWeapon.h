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
		bool isAttack = false;       // 攻撃中フラグ
		float timer = 0.0f;          // 攻撃経過時間
		float duration = 0.0f;       // 攻撃の持続時間
		Vector3 startOffset = { 0.0f, 0.0f, 0.0f }; // 攻撃開始位置のオフセット
		Vector3 endOffset = { 0.0f, 0.0f, 0.0f };   // 攻撃終了位置のオフセット
	};
	AttackInfo attackInfo_;

	/// ===武器の種類=== ///
	enum class WeaponType {
		kSword,    // 剣
		kHammer,   // ハンマー
		kSpear     // 槍
	};
	WeaponType weaponType_ = WeaponType::kSword;

private:
	// 攻撃の回転を計算
	void CalculateAttackRotation(const Vector3& startPoint, const Vector3& endPoint, float time);
};