#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector2.h"

/// ===前方宣言=== ///
namespace MiiEngine {
	class ParticleGroup;
}

///=====================================================/// 
/// MoveState
///=====================================================///
class MoveState : public PlayerState{
public:

	~MoveState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="player">処理対象のプレイヤーを指すポインタ。状態遷移時にプレイヤーに対する初期化や設定を行うために使用される。</param>
	/// <param name="camera">状態で使用するゲームカメラを指すポインタ。表示や視点の初期設定に使用される。</param>
	void Enter(Player* player, MiiEngine::CameraCommon* camera) override;

	/// <summary>
	/// ステート時の更新処理
	/// </summary>
	/// <param name="player">更新対象の Player インスタンスへのポインタ。</param>
	/// <param name="camera">更新処理に使用する MiiEngine::CameraCommon インスタンスへのポインタ。</param>
	void Update(Player* player, MiiEngine::CameraCommon* camera) override;

	/// <summary>
	/// ステートの終了処理
	/// </summary>
	void Finalize() override;	

private:
	/// ===状態=== ///
	struct State {
		Vector3 direction;	  // 移動方向
		Vector3 velocity;	  // 速度
		Quaternion rotation;  // 回転
	};
	State state_{}; // 現在の状態

	// 移動パーティクル
	MiiEngine::ParticleGroup* moveParticle_ = nullptr;

private:

	/// <summary>
	/// パーティクルの停止
	/// </summary>
	void StopMoveParticle();

	/// <summary>
	/// スティックの動きを適用します。
	/// </summary>
	/// <param name="stick">スティックの入力を表す2Dベクトル。</param>
	void ApplyStickMovement(const Vector2& stick);

	/// <summary>
	/// 減速処理
	/// </summary>
	void ApplyBraking();
};

