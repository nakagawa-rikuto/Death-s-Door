#pragma once
#include "Math/Vector2.h"

/// ===前方宣言=== ///
class Player;
namespace MiiEngine {
	class CameraCommon;
}

///=====================================================/// 
/// プレイヤーのベースステート
///=====================================================///
class PlayerState {
public:
	virtual ~PlayerState() = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理、純粋仮想関数
	/// </summary>
	/// <param name="player">Enter 処理の対象となる Player オブジェクトへのポインタ。</param>
	/// <param name="camera">処理時に参照または操作する CameraCommon オブジェクトへのポインタ。</param>
	virtual void Enter(Player* player, MiiEngine::CameraCommon* camera) = 0;

	/// <summary>
	/// ステート時の更新処理、純粋仮想関数
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// ステートの終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 減速処理
	/// </summary>
	/// <param name="deceleration">減速率</param>
	void ApplyDeceleration(const float& deceleration);

protected:

	Player* player_ = nullptr; // Player
	MiiEngine::CameraCommon* camera_ = nullptr; // Camera

	
};
