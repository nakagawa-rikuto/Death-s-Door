#include "EnemyMoveComponent.h"
// C++
#include <cassert>
// Math
#include "Math/sMath.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

#ifdef USE_IMGUI
///-------------------------------------------/// 
/// 設定の適用
///-------------------------------------------///
void EnemyMoveComponent::ApplyConfig(const MoveConfig& newconfig) {
	if (newconfig.speed < 0.0f || newconfig.chaseRange <= 0.0f || newconfig.evadeRange <= 0.0f) {
		assert(false && "Invalid MoveConfig");
	}

	// configの設定
	config_ = newconfig;
}
#endif // USE_IMGUI

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyMoveComponent::Initialize(const MoveConfig& config) {
	std::seed_seq seed{
		static_cast<uint32_t>(std::time(nullptr)),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this)),
	};
	randomEngine_.seed(seed);

	// 設定の初期化
	config_ = config;
	state_ = MoveState{};

	// 最初の移動をセットアップ
	SetupRandomMove();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
EnemyMoveComponent::UpdateResult EnemyMoveComponent::Update(const UpdateContext& context) {

	UpdateResult result{};

	/// ===タイマーの更新=== ///
	TimerUpdate(context.deltaTime);

	/// ===プレイヤーとの距離を計算=== ///
	Vector3 toPlayer = context.playerPosition - context.currentPosition;
	const float distToPlayer = Length(toPlayer);

	/// ===回避の判定=== ///
	// 圏内に入ったら evadeTimer をカウントし、0 になったらテレポートを要求する
	if (distToPlayer <= config_.evadeRange) {
		if (!state_.isInEvadeRange) {
			// 圏内に入った瞬間にタイマーをセット
			state_.isInEvadeRange = true;
			state_.evadeTimer = config_.evadeTime;
		}

		if (state_.evadeTimer <= 0.0f) {
			// 時間が経過したらテレポート
			state_.evadeTimer = 0.0f;
			result.teleportTrigger = true;
			result.velocity = { 0.0f, 0.0f, 0.0f };
			return result;
		}
	} else {
		state_.isInEvadeRange = false;
		state_.evadeTimer = 0.0f;
	}

	/// ===追跡の判定=== ///
	// evadeRange より外側かつ chaseRange 以内に入った瞬間にテレポートを要求する
	if (distToPlayer <= config_.chaseRange && distToPlayer > config_.evadeRange) {
		if (!state_.isInChaseRange) {
			state_.isInChaseRange = true;
			result.teleportTrigger = true;
			result.velocity = { 0.0f, 0.0f, 0.0f };
			return result;
		}
	} else if (distToPlayer > config_.chaseRange) {
		// 圏外に出たらリセット（再突入で再びテレポート可能にする）
		state_.isInChaseRange = false;
	}

	/// ===通常移動（ランダム方向に moveTime 秒間移動してループ）=== ///
	if (state_.moveTimer <= 0.0f) {
		SetupRandomMove();
	} else {
		state_.direction = { 0.0f, 0.0f, 0.0f };
	}

	// 移動ベクトルを計算
	result.velocity = state_.direction * config_.speed;

	// 常にPlayerを向いて移動する
	result.rotateDirection = Normalize(toPlayer);
	result.rotateDirection.y = 0.0f;

	return result;
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void EnemyMoveComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("移動情報")) {
		// 速度・回転
		ImGui::DragFloat("移動速度", &config_.speed, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("回転速度", &config_.rotationSpeed, 0.01f, 0.01f, 1.0f);
		// 範囲
		ImGui::DragFloat("追跡範囲", &config_.chaseRange, 0.1f, 0.1f, 100.0f);
		ImGui::DragFloat("回避圏内距離", &config_.evadeRange, 0.1f, 0.1f, 100.0f);
		// 時間
		ImGui::DragFloat("1回の移動時間", &config_.moveTime, 0.05f, 0.1f, 10.0f);
		ImGui::DragFloat("回避猶予時間", &config_.evadeTime, 0.05f, 0.1f, 10.0f);
		// 状態
		ImGui::Separator();
		ImGui::Text("状態");
		ImGui::Text("moveTimer  : %.2f", state_.moveTimer);
		ImGui::Text("evadeTimer : %.2f", state_.evadeTimer);
		ImGui::Text("isInEvadeRange  : %s", state_.isInEvadeRange ? "Yes" : "No");
		ImGui::Text("isInChaseRange  : %s", state_.isInChaseRange ? "Yes" : "No");
		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 移動のセットアップ
///-------------------------------------------///
void EnemyMoveComponent::SetupRandomMove() {
	// ランダムな角度で行動範囲の円上に移動先を決める
	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * Math::Pi());
	float angle = angleDist(randomEngine_);

	// 方向ベクトルを設定（Y 成分は 0 で水平移動のみ）
	state_.direction = Normalize(Vector3{
		std::cos(angle),
		0.0f,
		std::sin(angle)
		});

	// moveTime 秒間この方向に動く
	state_.moveTimer = config_.moveTime;
}

///-------------------------------------------/// 
/// タイマーの更新処理
///-------------------------------------------///
void EnemyMoveComponent::TimerUpdate(const float deltaTime) {
	// タイマーが0より大きい場合は減らす
	if (state_.moveTimer > 0.0f) {
		state_.moveTimer -= deltaTime;
	}
	// 回避猶予タイマー（圏内にいる間だけカウント）
	if (state_.isInEvadeRange && state_.evadeTimer > 0.0f) {
		state_.evadeTimer -= deltaTime;
	}
}