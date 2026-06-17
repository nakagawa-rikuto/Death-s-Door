#pragma once

///-------------------------------------------/// 
/// PlayerComponent
/// Playerクラスのコンポーネントをまとめるための名前空間
///-------------------------------------------///
namespace PlayerComponent {
	/// ===移動コンポーネント=== ///
	struct MoveComponent {
		float speed = 0.5f;			  // 移動速度
		float rotationSpeed = 0.15f;  // 回転速度
		float deceleration = 0.95f;   // 減速率
	};

	/// ===回避コンポーネント=== ///
	struct DodgeComponent {
		float speed = 15.0f;          // 回避速度
		float activeTime = 0.3f;      // 回避の有効時間
		float coolTime = 1.0f;        // 回避のクールタイム
		float invincibleTime = 0.01f; // 回避時の無敵時間
	};

	/// ===ヒットリアクションコンポーネント=== ///
	struct HitReactionComponent {
		float knockbackSpeed = 1.5f; // ノックバックの速度
	};

	/// ===遠距離攻撃コンポーネント=== ///
	struct RangedAttackComponent {};

	/// ===パラメーター=== ///
	struct Parameters {
		MoveComponent move;				  // 移動コンポーネントのパラメーター
		DodgeComponent dodge;			  // 回避コンポーネントのパラメーター
		HitReactionComponent hitReaction; // ヒットリアクションコンポーネントのパラメーター
		RangedAttackComponent rangedAttack; // 遠距離攻撃コンポーネントのパラメーター
	};

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	/// <param name="params"></param>
	void Information(const Parameters& params);
}