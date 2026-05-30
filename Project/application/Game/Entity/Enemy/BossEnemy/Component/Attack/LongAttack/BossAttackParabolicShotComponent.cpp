#include "BossAttackParabolicShotComponent.h"
// C++
#include <cassert>
#include <cmath>
#include <algorithm>
#include <cstdlib>
// Math
#include <Math/sMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void BossAttackParabolicShotComponent::Initialize(const ParabolicConfig& config) {
	config_ = config;
	state_ = ParabolicState{};
	phase_ = ParabolicPhase::Idle;
	initialSpeed_ = 0.0f;
	hitPosition_ = Vector3{};
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
BossAttackParabolicShotComponent::UpdateResult BossAttackParabolicShotComponent::Update(const UpdateContext & context) {
    UpdateResult result;
    result.isFlying = (phase_ == ParabolicPhase::Flying);
    result.isFinished = (phase_ == ParabolicPhase::Finished);


    // 非アクティブ時は何もしない
    if (phase_ == ParabolicPhase::Idle || phase_ == ParabolicPhase::Finished) {
        return result;
    }

    // ロックオンフェーズ
    if (phase_ == ParabolicPhase::LockOn) {
        state_.trembleTimer -= context.deltaTime;
        
		// 狙いを定める終了判定
        if (state_.trembleTimer <= 0.0f) {
            // 狙いを定める終了、飛行開始
            state_.velocity = CalcInitialVelocity(context.bossPosition, context.targetPosition);
            initialSpeed_ = Length(state_.velocity);
            state_.lifeTimer = config_.lifetime;
            
            phase_ = ParabolicPhase::Flying;
            result.isFlying = true;
        } else {
			// プレイヤーの方を常に向くようにする
            Vector3 direction = Normalize(context.targetPosition - context.bossPosition);
            direction.y = 0.0f; // 水平な方向のみを考慮
            result.faceDirection= Normalize(direction);
            return result;
        }
    }

    // -----------------------------------------------
    // 重力を速度に積算（Y成分のみ）
    // -----------------------------------------------
    state_.velocity.y -= config_.gravity * context.deltaTime;

    // -----------------------------------------------
    // 速度の大きさを現フレームの speed として返す
    // -----------------------------------------------
    result.velocity = state_.velocity;

    // -----------------------------------------------
    // 生存タイマー更新
    // -----------------------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
    state_.lifeTimer -= context.deltaTime;

    // -----------------------------------------------
    // 着弾判定（地面 Y 以下）
    // -----------------------------------------------
    const bool hitGround = config_.enableGroundHit &&
        (context.bulletPosition.y <= state_.groundY) &&
        (state_.velocity.y < 0.0f); // 下降中のみ

    if (hitGround) {
        hitPosition_ = context.bulletPosition;
        hitPosition_.y = state_.groundY;
    }

    // -----------------------------------------------
    // 終了条件チェック
    // -----------------------------------------------
    isHitGround_ = hitGround;
    if (state_.lifeTimer <= 0.0f || hitGround) {
        phase_ = ParabolicPhase::Finished;
        result.isFlying = false;
        result.isFinished = true;
    }

    return result;
}

///-------------------------------------------/// 
/// リセット
///-------------------------------------------///
void BossAttackParabolicShotComponent::Reset() {
    state_ = ParabolicState{};
    phase_ = ParabolicPhase::Idle;
    initialSpeed_ = 0.0f;
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackParabolicShotComponent::Information() {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("放物線弾攻撃情報")) {
        const char* phaseNames[] = { "Idle", "Trembling", "Flying", "Finished" };
        ImGui::Text("フェーズ: %s", phaseNames[static_cast<int>(phase_)]);
        ImGui::Text("残り生存時間: %.3f", state_.lifeTimer);
        ImGui::Text("残り震え時間: %.3f", state_.trembleTimer);
        ImGui::Text("現在速度ベクトル: (%.2f, %.2f, %.2f)",
            state_.velocity.x, state_.velocity.y, state_.velocity.z);
        ImGui::Text("初速の大きさ: %.3f", initialSpeed_);

        ImGui::Separator();

        ImGui::DragFloat("仰角 (度)", &config_.launchAngleDeg, 0.5f, 5.0f, 85.0f);
        ImGui::DragFloat("重力加速度", &config_.gravity, 0.1f, 0.0f, 50.0f);
        ImGui::DragFloat("生存時間 (秒)", &config_.lifetime, 0.1f, 0.5f, 30.0f);
        ImGui::DragFloat("LockONの時間 (秒)", &config_.trembleDuration, 0.05f, 0.0f, 5.0f);
        ImGui::DragFloat("水平速度上限", &config_.maxHorizontalSpeed, 0.5f, 1.0f, 100.0f);
        ImGui::Checkbox("地面着弾判定を有効化", &config_.enableGroundHit);
        if (config_.enableGroundHit) {
            ImGui::DragFloat("地面 Y 座標", &state_.groundY, 0.1f, -50.0f, 50.0f);
        }

        ImGui::TreePop();
    }
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 攻撃の開始
///-------------------------------------------///
void BossAttackParabolicShotComponent::StartAttack(float groundPosY) {
    if (IsActive()) {
        return;
    }

    state_.groundY = groundPosY;
    state_.trembleTimer = config_.trembleDuration;

    phase_ = ParabolicPhase::LockOn;
    isHitGround_ = false;
}

///-------------------------------------------///  
/// 発射初速ベクトルの計算
///-------------------------------------------///
Vector3 BossAttackParabolicShotComponent::CalcInitialVelocity(const Vector3 & from, const Vector3 & to) const {
    const float kDegToRad = Math::Pi() / 180.0f;

    // 水平方向のベクトルと距離
    const Vector3 horizontal = { to.x - from.x, 0.0f, to.z - from.z };
    const float   dx = Length(horizontal);
    const float   dy = to.y - from.y;

    const float cosTheta = std::cos(config_.launchAngleDeg * kDegToRad);
    const float sinTheta = std::sin(config_.launchAngleDeg * kDegToRad);
    const float tanTheta = (cosTheta > 1e-6f) ? (sinTheta / cosTheta) : 1e6f;

    // 分母: dx*tanθ - dy
    const float denom = dx * tanTheta - dy;

    float v0 = 0.0f;
    if (dx > 1e-3f && denom > 1e-3f) {
        // 物理式から v0 を求める
        const float cos2 = cosTheta * cosTheta;
        const float numerator = config_.gravity * dx * dx;
        const float denomFull = 2.0f * cos2 * denom;
        v0 = std::sqrt(numerator / denomFull);
    } else {
        // フォールバック: 水平速度上限を使用
        v0 = (dx > 1e-3f)
            ? (config_.maxHorizontalSpeed / std::max(cosTheta, 1e-3f))
            : config_.maxHorizontalSpeed;
    }

    // 水平速度上限を適用（v0 * cosθ <= maxHorizontalSpeed）
    const float hSpeed = v0 * cosTheta;
    if (hSpeed > config_.maxHorizontalSpeed) {
        v0 = config_.maxHorizontalSpeed / std::max(cosTheta, 1e-3f);
    }

    // 水平方向の単位ベクトル
    const Vector3 hDir = (dx > 1e-6f)
        ? Vector3{ horizontal.x / dx, 0.0f, horizontal.z / dx }
    : Vector3{ 0.0f, 0.0f, 1.0f };

    return Vector3{
        hDir.x * v0 * cosTheta,
        v0 * sinTheta,           // 上向き初速
        hDir.z * v0 * cosTheta
    };
}
