#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// HitReactionState
///=====================================================///
class HitReactionState : public PlayerState {
public:

	HitReactionState(const Vector3& direction);
	~HitReactionState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="player">処理対象のプレイヤーを指すポインタ。状態遷移時にプレイヤーに対する初期化や設定を行うために使用される。</param>
	/// <param name="camera">状態で使用するゲームカメラを指すポインタ。表示や視点の初期設定に使用される。</param>
	void Enter(Player* player, MiiEngine::CameraCommon* camera) override;

	/// <summary>
	/// ステート時の更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// ステートの終了処理
	/// </summary>
	void Finalize() override;

private:
	/// ===状態=== ///
	struct State {
		Vector3 knockbackDirection; // ノックバックの方向
		Vector3 velocity; // ノックバックの速度
	};
	State state_{};

private:

	/// <summary>
	/// ヒットリアクションを開始します。
	/// </summary>
	void StartHitReaction();
};

