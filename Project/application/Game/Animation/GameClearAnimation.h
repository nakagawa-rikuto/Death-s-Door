#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/CData.h"

/// ===前方宣言=== ///
class Player;
namespace MiiEngine {
	class FollowCamera;
}

/// ===アニメーションフェーズ=== ///
enum class ClearAnimationPhase {
	CameraRotation,  // カメラ回転フェーズ
	PlayerAdvance,   // プレイヤー前進フェーズ
	Completed        // 完了
};

///=====================================================/// 
/// ゲームクリアアニメーションクラス
///=====================================================///
class GameClearAnimation {
public:
	GameClearAnimation() = default;
	~GameClearAnimation() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="camera">カメラのポインタ</param>
	void Initialize(Player* player, MiiEngine::FollowCamera* camera);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// アニメーション完了フラグを取得
	/// </summary>
	/// <returns>完了していればtrue</returns>
	bool IsCompleted() const;

private:
	/// ===メンバ変数=== ///
	Player* player_ = nullptr;                   // プレイヤーへの参照
	MiiEngine::FollowCamera* camera_ = nullptr;  // カメラへの参照

	// 現在のフェーズ
	ClearAnimationPhase currentPhase_ = ClearAnimationPhase::CameraRotation;

	/// ===カメラ回転用=== ///
	struct CameraRotationInfo {
		float timer = 0.0f;			  // カメラ回転タイマー
		float duration = 3.0f;		  // カメラ回転時間
		float startAngle = 0.0f;      // 初期角度（ラジアン）
		float targetAngle = 0.0f;     // 目標角度（ラジアン）
		float height = 0.120f;		  // カメラの高さ（補間用）
		float distance = 94.0f;		  // カメラとプレイヤーの距離（補間用）
		Quaternion startRotation;     // 初期カメラ回転
		Quaternion targetRotation;    // 目標カメラ回転
		Vector3 playerForward;		  // プレイヤーの前方向ベクトル
	};
	CameraRotationInfo cameraInfo_;

	/// ===プレイヤー前進用=== ///
	struct PlayerAdvanceInfo {
		float timer = 0.0f;                          // 前進タイマー
		float duration = 1.5f;                       // 前進時間
		float speed = 5.0f;                          // 前進速度
		Vector3 direction;                           // 前進方向（プレイヤーの向き＋固定オフセット）
		static constexpr float kDirectionOffsetRad = 0.0f; // プレイヤー回転に加算する固定オフセット角度（ラジアン）
	};
	PlayerAdvanceInfo advanceInfo_;

	// カメラターゲット用の固定位置（初期化時に登録し、以降変更しない）
	Vector3 fixedCameraTargetPosition_;

	float deltaTime_ = 0.0f; // deltaTime

private:
	/// <summary>
	/// カメラ回転フェーズの更新
	/// </summary>
	void UpdateCameraRotation();

	/// <summary>
	/// プレイヤー前進フェーズの更新
	/// </summary>
	void UpdatePlayerAdvance();
};
