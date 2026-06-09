#pragma once
#include <Math/Vector3.h>

///=====================================================/// 
/// BossComponent
/// ボスのコンポーネントを宣言するヘッダーファイル
///=====================================================///
namespace BossComponent {
	///-------------------------------------------/// 
	/// Move
	/// 設定パラメータをここで宣言する
	///-------------------------------------------///
	struct MoveComponent {
		float moveSpeed = 0.2f;			// 移動速度
		float rotationSpeed = 0.8f;		// 回転速度
	};

	///-------------------------------------------/// 
	/// Teleport
	/// 設定パラメータをここで宣言する
	///-------------------------------------------///
	struct TeleportComponent {
		float rotationSpeed = 5.0f;		// 向き補間速度
		float spinOutDuration = 0.6f;   // SpinOut の時間
		float warpDuration = 0.1f;      // Warp の時間
		float spinInDuration = 0.6f;	// SpinIn  の時間
	};

	///-------------------------------------------/// 
	/// HitReaction
	/// 設定パラメータをここで宣言する
	///-------------------------------------------///
	struct HitReactionComponent {
		float knockBackForce = 1.5f;	// ノックバックの強さ
		float slowdownFactor = 0.3f;	// 減速率(0.0~1.0、小さいほど遅くなる)
		float slowdownDuration = 0.2f;	// 減速効果の持続時間
		float alphaDuration = 0.2f;		// 透明度変化の持続時間
		float hitAlpha = 0.6f;			// ヒット時の透明度
		float flashSpeed = 10.0f;		// 点滅速度
	};

	///-------------------------------------------/// 
	/// Attack
	/// 設定パラメータをここで宣言する
	///-------------------------------------------///
	/// ===近距離攻撃=== ///
	// RotateAttack（回転攻撃）
	struct RotateAttackComponent {
		float windUpDuration = 0.5f;	// 溜め時間
		float strikeAngle = 30.0f;		// 回転角度
		float strikeDuration = 0.5f;	// 攻撃時間
		float recoveryDuration = 0.5f;  // 戻り（Recovery）時間
		Vector3 weaponOffset = { 0.0f, 0.0f, 0.0f }; // 武器の定位置オフセット
	};
	// DownwardSwingAttack（振り下ろし攻撃）
	struct DownwardSwingAttackComponent {
		float windUpPitch = 15.0f;		  // タメの前傾角度
		float windUpDuration = 0.35f;	  // タメ時間
		float strikeForwardPitch = 15.0f; // 振り下ろしの前傾角度
		float strikeDuration = 0.10f;	  // 振り下ろし時間
		float holdDownDuration = 0.08f;   // 叩きつけ後の前傾維持時間
		float recoveryDuration = 0.40f;   // 正位置へ戻る時間
		float strikeStepForward = 0.5f;   // Strike中にボスを前方へ踏み込ませる距離
		Vector3 weaponOffset = { 0.0f, 0.0f, 0.0f }; // 武器の定位置オフセット
	};
	// JumpSmashAttack（ジャンプ叩きつけ攻撃）
	struct JumpSmashAttackComponent {
		float minDistance = 2.0f;			   // 攻撃開始可能な最小距離
		float maxDistance = 5.0f;			   // 攻撃開始可能な最大距離
		float leapWindUpCrouchPitch = -10.0f;  // 跳躍前の溜めでの屈み前傾角度
		float leapWindUpDuration = 0.30f;	   // 跳躍前の溜め時間
		float leapDuration = 0.60f;			   // 飛行にかける時間
		float leapArcHeight = 4.0f;			   // 飛行の放物線の頂点高さ
		float leapAscentPitch = 20.0f;		   // 飛行中の上昇での前傾角度
		float leapDescentPitch = 30.0f;		   // 飛行中の降下での仰け反り角度
		float strikeForwardPitch = 25.0f;	   // 着地時の前傾角度
		float strikeDuration = 0.10f;		   // 着地叩きつけ時間
		float holdDownDuration = 0.08f;		   // 着地叩きつけ後の前傾維持時間
		float recoveryDuration = 0.45f;		   // 正位置へ戻る時間
		Vector3 weaponOffset = { 0.0f, 0.0f, 0.0f }; // 武器の定位置オフセット
	};

	/// ===遠距離攻撃=== ///
	// OrbitingAttack（周回攻撃）
	struct OrbitingAttackComponent {
		int bulletCount = 3;			// 周回する弾の数
		float orbitRadius = 3.0f;		// 周回半径
		float orbitSpeed = 1.0f;		// 周回速度
		float orbitHeight = 1.0f;		// 周回の高さ
		float lifeTime = 5.0f;			// 周回攻撃の寿命
		float initialAngle = 0.0f;		// 周回開始時の角度
	};
	// ParabolicShotAttack（放物線攻撃）
	struct ParabolicShotAttackComponent {
		float launchAngle = 45.0f;			// 発射角度
		float gravity = 9.8f;				// 重力加速度
		float lifeTime = 5.0f;				// 弾の寿命
		float trebleDuration = 0.5f;		// 三段階変化の時間
		float rotationSpeed = 8.0f;			// Bossの回転速度
		bool enableGroundHit = true;		// 地面ヒットを有効にするか
		float maxHorizontalSpeed = 2.0f;	// 水平速度の上限
	};

	///-------------------------------------------/// 
	/// 攻撃の射程
	///-------------------------------------------///
	struct AttackRange {
		float rotateAttack = 14.0f;		   // 回転攻撃の射程
		float downwardSwingAttack = 20.0f; // 振り下ろし攻撃の射程
		float jumpSmashMin = 30.0f;		   // ジャンプ叩きつけ攻撃の最小射程
		float jumpSmashMax = 50.0f;		   // ジャンプ叩きつけ攻撃の最大射程
		float orbitingOrbs = 40.0f;		   // 周回攻撃の射程
		float parabolicShot = 35.0f;	   // 放物線攻撃の射程
	};

	///-------------------------------------------/// 
	/// 攻撃のクールダウン
	///-------------------------------------------///
	struct AttackCooldown {
		float rotateAttack = 2.5f;		  // 回転攻撃のクールダウン
		float downwardSwingAttack = 4.0f; // 振り下ろし攻撃のクールダウン
		float jumpSmashAttack = 8.0f;	  // ジャンプ叩きつけ攻撃のクールダウン
		float orbitingOrbs = 6.0f;		  // 周回攻撃のクールダウン
		float parabolicShot = 5.0f;		  // 放物線攻撃のクールダウン
	};
	
	///-------------------------------------------/// 
	/// Parameters
	/// 全てのパラメータをまとめる構造体
	///-------------------------------------------///
	struct Parameters {
		/// ===攻撃の射程=== ///
		AttackRange attackRange;

		/// ===攻撃のクールダウン=== ///
		AttackCooldown attackCooldown;

		/// ===移動=== ///
		MoveComponent move;
		TeleportComponent teleport;

		/// ===ヒットリアクション=== ///
		HitReactionComponent hitReaction;

		/// ===近距離攻撃=== ///
		RotateAttackComponent rotateAttack;
		DownwardSwingAttackComponent downwardSwingAttack;
		JumpSmashAttackComponent jumpSmashAttack;

		/// ===遠距離攻撃=== ///
		OrbitingAttackComponent orbitingAttack;
		ParabolicShotAttackComponent parabolicShotAttack;
	};

	///-------------------------------------------/// 
	/// Information
	/// パラメータの情報を表示する
	///-------------------------------------------///
	void Information(const Parameters& parameters);
}
//NOTE:後々この情報をJsonで読み込むことができるようにする。