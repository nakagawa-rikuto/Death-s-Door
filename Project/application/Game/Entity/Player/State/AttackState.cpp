#include "AttackState.h"
// Service
#include "Service/Input.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"
// Math
#include "Math/sMath.h"
#include "Math/TransformationMath.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
AttackState::AttackState() {
	// 攻撃アニメーションの名前を設定
	attackAnimationNames_ = { "Attack1", "Attack2", "Attack3" };

	// コンボ受付開始のアニメーションの進捗
	comboAcceptNormalizeTime_ = 0.8f;

	// アニメーション終了後のコンボ受付時間
	comboAcceptExtraTime_ = 0.2f;

	// 攻撃開始時の前方移動の強さを初期化
	moveForwardStrength_ = 0.5f;
}

///-------------------------------------------/// 
/// 状態に入ったときに呼ばれる
///-------------------------------------------///
void AttackState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 攻撃の開始
	StartAttack(0);
	// 前方に移動
	moveForwardOnAttackStart();

	// 攻撃のアクティブフラグを設定
	player_->SetActiveAttackFlag(isAttacking_);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void AttackState::Update() {

	/// ===減速処理=== ///
	PlayerState::ApplyDeceleration(0.4f); // 攻撃中は減速率を高めに設定

	// その攻撃が最後が最後の攻撃か
	bool isLastAttack = (attackIndex_ >= static_cast<int>(attackAnimationNames_.size()) - 1);

	/// ===攻撃中の処理=== ///
	if (isAttacking_) {
		// 最後の攻撃かどうかを判別
		float normalizeTime = player_->GetAnimationNormalizeTime();

		// 最後の攻撃以外ならコンボの受付
		if (!canCombo_ && !isLastAttack && normalizeTime >= comboAcceptNormalizeTime_) {
			canCombo_ = true;
		}

		// アニメーションが終了したら
		if (player_->IsAnimationFinished()) {
			isAttacking_ = false;

			// 最後の攻撃なら強制終了
			if (isLastAttack) {
				canCombo_ = false;
			}

			// コンボ受付タイマーの計測開始
			comboAcceptTimer_ = 0.0f;
		}
	} else if (canCombo_) {

		/// ===時間の更新=== ///
		comboAcceptTimer_ += player_->GetDeltaTime();

		// コンボ受付が終了したら
		if (comboAcceptTimer_ >= comboAcceptExtraTime_) {
			canCombo_ = false;
		}
	}

	/// ===攻撃ボタンが押されたら=== ///
	if (canCombo_ && (Service::Input::TriggerButton(0, ControllerButtonType::X) || Service::Input::TriggerMouse(MouseButtonType::Left))) {
		moveForwardStrength_ = 2.5f;
		StartAttack(attackIndex_ + 1);
		moveForwardOnAttackStart();
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
void AttackState::StartAttack(int attackIndex) {

	// キーボード操作時はマウスの方向を向く
	FaceMouseDirection();

	attackIndex_ = attackIndex;
	player_->PlayAnimation(attackAnimationNames_[attackIndex_], false);

	isAttacking_ = true;
	canCombo_ = false;
	comboAcceptTimer_ = 0.0f;
}

///-------------------------------------------/// 
/// 攻撃のキャンセル
///-------------------------------------------///
void AttackState::CancelAttack() {
	isAttacking_ = false;
	canCombo_ = false;
	comboAcceptTimer_ = 0.0f;
	attackIndex_ = 0;
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
