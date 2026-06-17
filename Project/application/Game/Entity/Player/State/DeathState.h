#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// DeathState
/// 死亡時のステート
///=====================================================///
class DeathState : public PlayerState {
public:

	~DeathState() = default;

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

	// 減速率
	float decelerationRate_ = 0.7f;
};

