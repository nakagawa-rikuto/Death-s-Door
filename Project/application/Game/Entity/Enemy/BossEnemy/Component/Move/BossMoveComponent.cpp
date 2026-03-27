#include "BossMoveComponent.h"
// C++
#include <cassert>
#include <algorithm>
// Math
#include <Math/sMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

#ifdef USE_IMGUI
///-------------------------------------------///  
///	設定の適用
///-------------------------------------------///
void BossMoveComponent::ApplyConfig(const MoveConfig& newconfig) {
	if (newconfig.speed < 0.0f) {
		assert(false && "Invalid MoveConfig");
	}

	// configの設定
	config_ = newconfig;
}
#endif // USE_IMGUI

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void BossMoveComponent::Initialize(const MoveConfig& config) {
	// 設定の初期化
	config_ = config;
	state_ = MoveState{};
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
BossMoveComponent::UpdateResult BossMoveComponent::Update(const UpdateContext & context) {
	UpdateResult result;

	// プレイヤーへのベクトルを計算
	Vector3 toPlayer = context.playerPosition - context.currentPosition;

	// 高低差を無視し、水平面（XZ平面）のみの移動とY軸周りの回転にする
	toPlayer.y = 0.0f;

	// プレイヤーに向かってゆっくり移動する方向を計算
	if (Length(toPlayer) > 0.001f) {
		state_.direction = Normalize(toPlayer);
	}

	result.velocity = state_.direction * config_.speed;

	// プレイヤーの方を徐々に向くように回転を計算
	result.rotate = CalcRotation(context.currentRotation, state_.direction, context.deltaTime);

	return result;
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void BossMoveComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("移動情報")) {
		// 速度・回転
		ImGui::DragFloat("移動速度", &config_.speed, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("回転速度", &config_.rotationSpeed, 0.01f, 0.01f, 1.0f);
		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------///  
/// 現在の回転から目標方向への回転を計算します。
///-------------------------------------------///
Quaternion BossMoveComponent::CalcRotation(const Quaternion& current, const Vector3& targetDir, float deltaTime) const {
	// targetDir がほぼゼロベクトルなら回転させない
	if (Length(targetDir) < 0.001f) {
		return current;
	}
	// forwardを基準に targetDir 方向への回転を計算
	const Vector3    forward = { 0.0f, 0.0f, 1.0f };
	const Quaternion targetRotation = DirectionToQuaternion(forward, Normalize(targetDir));
	const float      t = std::min(config_.rotationSpeed * deltaTime, 1.0f);
	return Math::SLerp(current, targetRotation, t);

}

///-------------------------------------------/// 
/// 2つの方向ベクトル間の回転を表すクォータニオンを計算します。
///-------------------------------------------///
Quaternion BossMoveComponent::DirectionToQuaternion(const Vector3& from, const Vector3& to) const {
	// 2ベクトルが同じ方向の場合は恒等Quaternionを返す
	const float dot = Dot(from, to);
	if (dot >= 1.0f - 0.001f) {
		return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	// 2ベクトルが逆方向の場合はY軸周りで180度回転させる
	if (dot <= -1.0f + 0.001f) {
		// キャラクターが上下反転しないように、強制的にY軸を回転軸にする
		return Quaternion{
			0.0f,
			1.0f,
			0.0f,
			0.0f
		};
	}

	// 通常ケース：外積を回転軸、内積から角度を求める
	const Vector3 axis = Normalize(Math::Cross(from, to));
	const float   angle = std::acos(std::clamp(dot, -1.0f, 1.0f));
	const float   s = std::sin(angle * 0.5f);

	return Quaternion{
		axis.x * s,
		axis.y * s,
		axis.z * s,
		std::cos(angle * 0.5f)
	};

}
