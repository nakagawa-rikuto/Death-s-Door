#include "AttackState.h"
// Service
#include "Service/Input.h"
// Player
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Entity/Player/Weapon/PlayerWeapon.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"
// Math
#include "Math/sMath.h"
#include "Math/TransformationMath.h"

///-------------------------------------------/// 
/// 状態に入ったときに呼ばれる
///-------------------------------------------///
void AttackState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	if (StartAttack(0, player_->GetWeapon())) {
		moveForwardStrength_ = 0.5f; // 通常攻撃の前方移動の強さ
		moveForwardOnAttackStart(); // 攻撃開始時に前方に移動
	}

	// 攻撃のアクティブフラグを設定
	player_->SetActiveAttackFlag(isAttacking_);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void AttackState::Update() {

	/// ===減速処理=== ///
	PlayerState::ApplyDeceleration(0.4f); // 攻撃中は減速率を高めに設定

	/// ===タイマーの更新=== ///
	UpdateTimers(player_->GetDeltaTime());

	/// ===攻撃中の処理=== ///
	if (isAttacking_) {
		// 攻撃時間が終了していないかチェック
		const AttackData* currentData = player_->GetAttackData(state_.attackID);

		if (currentData && state_.timer >= currentData->activeDuration) {
			isAttacking_ = false;
			state_.previousAttackID = state_.attackID; // 前の攻撃IDを保存
			state_.attackID = -1; // 攻撃IDをリセット

			// コンボ受付の開始
			if (currentData->canComboToNext) {
				canCombo_ = true; // コンボ受付開始
				state_.comboTimer = 0.0f; // コンボタイマーリセット
			}
		}
	}

	/// ===コンボ受付時間の管理=== ///
	if (canCombo_) {
		// 前の攻撃IDからデータを取得
		const AttackData* previousData = player_->GetAttackData(state_.previousAttackID);

		// コンボ受付時間が終了していないかチェック
		if (previousData && state_.comboTimer >= previousData->comboWindowTime) {
			canCombo_ = false; // コンボ受付終了
			state_.comboTimer = 0.0f; // コンボタイマーリセット
			state_.previousAttackID = -1; // 前の攻撃IDリセット
		}
	}

	// 攻撃ボタンが押されたらコンボを試行
	if (Service::Input::TriggerButton(0, ControllerButtonType::X) || Service::Input::TriggerMouse(MouseButtonType::Left)) {
		if (canCombo_) {
			if (TryCombo(player_->GetWeapon())) {
				moveForwardStrength_ = 2.5f; // コンボ攻撃の前方移動の強さ
				moveForwardOnAttackStart(); // 攻撃開始時に前方に移動
			}
		}
	}

	/// ===Stateの移動=== ///
	if (!isAttacking_ && !canCombo_) {
		// 攻撃もコンボも終了している場合はRootStateに移行
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void AttackState::Finalize() {
	CancelAttack(); // 攻撃状態をリセット
	player_->SetActiveAttackFlag(isAttacking_); // 攻撃終了
	PlayerState::Finalize();
}

///-------------------------------------------/// 
/// 攻撃開始処理
///-------------------------------------------///
bool AttackState::StartAttack(int attackID, PlayerWeapon* weapon) {
	// 既存の攻撃をキャンセル
	CancelAttack(); 

	// キーボード操作時はマウスの方向を向く
	FaceMouseDirection();

	// 攻撃データを取得
	const AttackData* attackData = player_->GetAttackData(attackID);
	if (!attackData) {
		return false;
	}
	attackData_ = *attackData;

	// 攻撃データを武器に適用
	ApplyAttackToWeapon(attackData_, weapon);

	// 状態の更新
	isAttacking_ = true;
	state_.attackID = attackID;
	state_.timer = 0.0f;
	state_.comboCount = 0;
	canCombo_ = false;

	return true;
}

///-------------------------------------------/// 
/// コンボ攻撃の試行
///-------------------------------------------///
bool AttackState::TryCombo(PlayerWeapon* weapon) {
	// コンボ可能かどうかのチェック
	if (!canCombo_) return false;

	// 前の攻撃データを取得
	const AttackData* previousData = player_->GetAttackData(state_.previousAttackID);
	if (!previousData || !previousData->canComboToNext) return false;

	// 次の攻撃IDを取得
	int nextAttackID = previousData->nextComboID;
	if (nextAttackID < 0) return false;

	// 次の攻撃を開始
	canCombo_ = false;
	state_.comboCount++;
	return StartAttack(nextAttackID, weapon);
}

///-------------------------------------------/// 
/// 攻撃のキャンセル
///-------------------------------------------///
void AttackState::CancelAttack() {
	isAttacking_ = false;
	canCombo_ = false;
	state_.attackID = -1;
	state_.previousAttackID = -1;
	state_.comboCount = 0;
	state_.timer = 0.0f;
	state_.comboTimer = 0.0f;
}

///-------------------------------------------/// 
/// タイマーの更新
///-------------------------------------------///
void AttackState::UpdateTimers(const float deltaTime) {
	// 攻撃中なら攻撃タイマーを更新
	if (isAttacking_) {
		state_.timer += deltaTime;
	}
	
	// コンボ受付中ならコンボタイマーも更新
	if (canCombo_) {
		state_.comboTimer += deltaTime;
	}
}

///-------------------------------------------/// 
/// 攻撃データを武器に適用
///-------------------------------------------///
void AttackState::ApplyAttackToWeapon(const AttackData& data, PlayerWeapon* weapon) {
	// 早期リターン
	if (!weapon) return;

	// チャンネル0
	const TrajectoryChannel* weaponChannel = data.GetChannel(0);
	if (weapon && weaponChannel && weaponChannel->enabled && weaponChannel->points.size() >= 2) {
		weapon->StartAttack(weaponChannel->points, data.activeDuration);
	}
}

///-------------------------------------------/// 
/// 攻撃開始時に前方に移動する処理
///-------------------------------------------///
void AttackState::moveForwardOnAttackStart() {
	// 見栄え向上のために少し攻撃方向に進む
	Vector3 forward = Math::RotateVector({ 0.0f, 0.0f, 1.0f }, player_->GetTransform().rotate);
	forward.y = 0.0f;
	if (forward.x != 0.0f || forward.z != 0.0f) {
		forward = Normalize(forward);
		player_->SetVelocity(forward * moveForwardStrength_);
	}
	// 波紋を出す
	player_->GetGroundOcean()->AddRipple(player_->GetTransform().translate, 1.0f, 0.05f);
}

///-------------------------------------------/// 
/// マウスの方向に向く
///-------------------------------------------///
void AttackState::FaceMouseDirection() {
	// 現在のデバイスがキーボードでない場合は処理を中止
	if (Service::Input::GetActiveDevice() != DeviceType::Keyboard) return;

	// マウスの位置を取得
	POINT mousePos = Service::Input::GetMousePosition();
	Vector2 mousePosVec2 = { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };

	// プレイヤーのY座標の平面とマウスレイの交点を取得（Y軸は使用しないためプレイヤーの高さで固定）
	float playerY = player_->GetTransform().translate.y;
	std::optional<Vector3> hitPos = Math::ScreenToWorldOnPlane(mousePosVec2, playerY);
	if (!hitPos.has_value()) return;

	// プレイヤーから交点への方向（XZ平面のみ）
	Vector3 toMouse = hitPos.value() - player_->GetTransform().translate;
	toMouse.y = 0.0f;
	if (toMouse.x == 0.0f && toMouse.z == 0.0f) return;
	toMouse = Normalize(toMouse);

	// Yaw角を求める
	float yawRad = std::atan2(toMouse.x, toMouse.z);
	float yawDeg = yawRad * (180.0f / Math::Pi());
	Quaternion targetRotate = Math::EulerToQuaternion({ 0.0f, yawDeg, 0.0f });

	// 現在の回転から目標の回転へ滑らかに補間
	Quaternion currentRotate = player_->GetTransform().rotate;
	Quaternion newRotate = Math::SLerp(currentRotate, targetRotate, rotationLerpSpeed_ * player_->GetDeltaTime());
	player_->SetRotate(newRotate);
}
