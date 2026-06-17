#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// DodgeState
///=====================================================///
class DodgeState : public PlayerState {
public:
	DodgeState(const Vector3& direction);
	~DodgeState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="player">処理対象のプレイヤーを指すポインタ。</param>
	/// <param name="camera">現在のゲームカメラを指すポインタ。エントリ処理でカメラの状態や視点を使用する可能性がある。</param>
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
		float timer = 0.0f;		// タイマー
		float acceleration;		// 加速度
		Vector3 direction;		// 回避方向
		Vector3 velocity;		// 回避ベクトル
	};
	State state_{};

private:

	/// <summary>
	/// 回避処理の開始
	/// </summary>
	void StartAvoidance();

	/// <summary>
	/// タイマーの更新処理
	/// </summary>
	void UpdateTimer();

	/// <summary>
	/// 更新処理
	/// </summary>
	void UpdateAvoidance();
};

