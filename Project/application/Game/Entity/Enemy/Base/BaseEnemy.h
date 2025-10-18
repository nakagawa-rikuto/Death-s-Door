#pragma once
/// ===Include=== ///
// Collider
#include "application/Game/Entity/GameCharacter/GameCharacter.h"
// State
#include "application/Game/Entity/Enemy/State/Base/EnemyState.h"
// c++
#include <random>

/// ===前方宣言=== ///
class Player;

/// ===StateType=== ///
enum class StateType {
	Move,
	Attack
};

///=====================================================/// 
/// Enemy
///=====================================================///
class BaseEnemy : public GameCharacter<OBBCollider> {
public:
	BaseEnemy() = default;
	~BaseEnemy();

	/// <summary>
	/// ゲームシーンで呼び出す初期化処理の純粋仮想関数
	/// </summary>
	/// <param name="translate">シーンの位置を指定する平行移動ベクトル。const参照で渡される。</param>
	virtual void InitGameScene(const Vector3& translate) = 0;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。既定値は BlendMode::KBlendModeNormal。</param>
	virtual void Draw(BlendMode mode = BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	virtual void Information()override;

	/// <summary>
	/// 攻撃処理の純粋仮想関数
	/// </summary>
	virtual void Attack() = 0;

	/// <summary>
	/// このオブジェクトのチューニング設定を指定した情報を他のBaseEnemyオブジェクトにコピー
	/// </summary>
	/// <param name="enemy">コピー先の BaseEnemy オブジェクトへのポインター。</param>
	void CopyTuningTo(BaseEnemy* enemy) const;

public: /// ===衝突判定=== ///

	/// <summary>
   /// 衝突時の処理
   /// </summary>
   /// <param name="collider">衝突した相手を表す Collider へのポインター。</param>
	void OnCollision(Collider* collider) override;

public: /// ===State用関数=== ///
	// 移動処理の開始処理
	void CommonMoveInit();
	// 移動処理の共通部分
	void CommonMove();
	// 攻撃可能かチェック
	bool CheckAttackable();
	// Stateの変更
	//NOTE:newState = 次のステート
	void ChangeState(std::unique_ptr<EnemyState> nextState);

public: /// ===Getter=== ///
	// Timer
	float GetAttackTimer()const { return attackInfo_.timer; };
	// Flag
	bool GetAttackFlag()const { return attackInfo_.isAttack;};

public: /// ===Setter=== ///
	// Player
	void SetPlayer(Player* player) { player_ = player; };
	// Timer
	void SetTimer(StateType type, float time);

protected: /// ===変数の宣言=== ///

	GameCamera* camera_ = nullptr; // カメラ
	Player* player_ = nullptr; // Player

	/// ===State=== ///
	std::unique_ptr<EnemyState> currentState_;

	/// ===移動情報=== ///
	struct MoveInfo {
		float timer;	// タイマー
		float speed;	// 移動速度
		float range;	// 移動範囲
		float interval;	// 移動間隔
		float waitTime;	// 待機時間

		Vector3 rangeCenter; // 移動範囲の中心
		Vector3 direction;	 // 移動方向

		bool isWating;	// 待機フラグ
	};
	MoveInfo moveInfo_;

	/// ===攻撃情報=== ///
	struct AttackInfo {
		float timer;	// タイマー
		float range;	// 攻撃範囲（回転の情報から±）
		float distance;	// 攻撃可能距離
		float interval;	// 攻撃間隔
		int32_t power;	// 待機時間

		Vector3 direction;  // 攻撃方向
		Vector3 playerPos;  // プレイヤーの位置

		bool isAttack;	// 攻撃フラグ
	};
	AttackInfo attackInfo_;

	// ランダムシード
	std::mt19937 randomEngine_;

protected: /// ===関数の宣言=== ///

	/// <summary>
	/// 回転の更新処理
	/// </summary>
	/// <param name="direction">回転目標の方向</param>
	/// <param name="lerpT">回転完了までの時間</param>
	void UpdateRotationTowards(const Vector3& direction, float lerpT);

	/// <summary>
	/// 派生側で型固有のチューニング値をコピーするための関数
	/// </summary>
	/// <param name="enemy">親のBaseEnemy</param>
	virtual void CopyTypeTuningFromThisTo(BaseEnemy* enemy) const {};

private:

	/// <summary>
	/// 方向の設定と待機時間の設定
	/// </summary>
	/// <param name="vector">設定する方向のベクトル</param>
	void PreparNextMove(const Vector3& vector);

	/// <summary>
	/// タイマーを進める処理
	/// </summary>
	void advanceTimer();
	
};

