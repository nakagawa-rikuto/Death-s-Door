#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
#include "Math/Vector3.h"

///=====================================================/// 
/// BossMoveState
/// ボスの移動状態を表すクラス
///=====================================================///
class BossMoveState : public BossState {
public:
	~BossMoveState() override = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="enemy">状態に入るボスエネミーへのポインタ。</param>
	void Enter(BossEnemy* enemy) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

private:

	/// ===現在の状態=== ///
	struct MoveState {
		Vector3 direction = {0.0f, 0.0f, 0.0f};         // 現在の移動方向
	};
	MoveState state_{};

private:

	/// <summary>
	/// 移動処理を更新する。
	/// </summary>
	void UpdateMove();

	/// <summary>
	/// Stateの変更が必要か判定して、必要なら変更する。
	/// </summary>
	/// <param name="dist">プレイヤーとの距離。</param>
	/// <returns>状態を変更した場合はtrueを返す。</returns>
	bool ChangeStateIfNeeded(float dist);
};

