#include "BossAttackOrbitingOrbsComponent.h"
// C++
#include <cassert>
#include <cmath>
#include <algorithm>
// Math
#include <Math/sMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

namespace {
    const float kDegToRad = Math::Pi() / 180.0f;
    const float kTwoPi = Math::Pi() * 2.0f;
} // namespace

///-------------------------------------------///
/// 初期化
///-------------------------------------------///
void BossAttackOrbitingOrbsComponent::Initialize(const OrbitConfig& config) {
    config_ = config;
    state_ = OrbitState{};
    phase_ = OrbitPhase::Idle;
}

///-------------------------------------------///
/// 更新
///-------------------------------------------///
BossAttackOrbitingOrbsComponent::UpdateResult
BossAttackOrbitingOrbsComponent::Update(const UpdateContext& context) {
    UpdateResult result;
    result.isOrbiting = (phase_ == OrbitPhase::Orbiting);
    result.isFinished = (phase_ == OrbitPhase::Finished);

    // 非アクティブ時は何もしない
    if (phase_ == OrbitPhase::Idle || phase_ == OrbitPhase::Finished) {
        return result;
    }

    // -----------------------------------------------
    // タイマー更新
    // -----------------------------------------------
    state_.orbitTimer += context.deltaTime;
    state_.lifeTimer -= context.deltaTime;

    // -----------------------------------------------
    // 各弾の位置・方向を計算
    // -----------------------------------------------
    for (int i = 0; i < kOrbCount; ++i) {
        const float angleRad = CalcOrbitAngleRad(i);

        // 現在位置
        const Vector3 currentPos = CalcOrbitPosition(context.bossPosition, angleRad);

        // 次フレームの向き（現在位置 → 次位置 の差分）
        const Vector3 dir = CalcOrbitDirection(context.bossPosition, angleRad, context.deltaTime);

        result.bullets[i].position = currentPos;
        result.bullets[i].direction = dir;
    }

    // -----------------------------------------------
    // 生存時間チェック → Finished 遷移
    // -----------------------------------------------
    if (state_.lifeTimer <= 0.0f) {
        phase_ = OrbitPhase::Finished;
        result.isOrbiting = false;
        result.isFinished = true;
    }

    return result;
}

///-------------------------------------------///
/// リセット
///-------------------------------------------///
void BossAttackOrbitingOrbsComponent::Reset() {
    state_ = OrbitState{};
    phase_ = OrbitPhase::Idle;
}

///-------------------------------------------///
/// 攻撃開始
///-------------------------------------------///
void BossAttackOrbitingOrbsComponent::StartAttack() {
    if (IsActive()) {
        return;
    }
    state_.orbitTimer = 0.0f;
    state_.lifeTimer = config_.lifetime;
    phase_ = OrbitPhase::Orbiting;
}

///-------------------------------------------///
/// ImGui 情報の表示
///-------------------------------------------///
void BossAttackOrbitingOrbsComponent::Information() {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("公転弾攻撃情報")) {
        const char* phaseNames[] = { "Idle", "Orbiting", "Finished" };
        ImGui::Text("フェーズ: %s", phaseNames[static_cast<int>(phase_)]);
        ImGui::Text("公転タイマー: %.3f", state_.orbitTimer);
        ImGui::Text("残り生存時間: %.3f", state_.lifeTimer);

        ImGui::Separator();

        ImGui::DragFloat("公転半径", &config_.orbitRadius, 0.05f, 0.5f, 20.0f);
        ImGui::DragFloat("公転速度 (度/秒)", &config_.orbitSpeedDeg, 1.0f, 10.0f, 720.0f);
        ImGui::DragFloat("高さオフセット", &config_.orbitHeight, 0.05f, -5.0f, 5.0f);
        ImGui::DragFloat("生存時間 (秒)", &config_.lifetime, 0.1f, 0.5f, 30.0f);
        ImGui::DragFloat("初期位相オフセット(度)", &config_.initialPhaseOffsetDeg, 1.0f, 0.0f, 180.0f);

        ImGui::TreePop();
    }
#endif // USE_IMGUI
}

#ifdef USE_IMGUI
///-------------------------------------------///
/// 設定の適用（ImGui 編集後に呼ぶ）
///-------------------------------------------///
void BossAttackOrbitingOrbsComponent::ApplyConfig(const OrbitConfig& newConfig) {
    assert(newConfig.orbitRadius > 0.0f && "orbitRadius must be > 0");
    assert(newConfig.lifetime > 0.0f && "lifetime must be > 0");
    config_ = newConfig;
}
#endif // USE_IMGUI

///-------------------------------------------///
/// 弾 index の現在の公転角度（ラジアン）を返す
///-------------------------------------------///
float BossAttackOrbitingOrbsComponent::CalcOrbitAngleRad(int index) const {
    // 基準角度（経過時間 × 角速度）
    const float baseAngle = state_.orbitTimer * config_.orbitSpeedDeg * kDegToRad;

    // 各弾の位相オフセット
    const float phaseOffset = static_cast<float>(index) * config_.initialPhaseOffsetDeg * kDegToRad;

    return baseAngle + phaseOffset;
}

///-------------------------------------------///
/// 公転角度 → ワールド座標
///-------------------------------------------///
Vector3 BossAttackOrbitingOrbsComponent::CalcOrbitPosition(
    const Vector3& center, float angleRad) const
{
    return Vector3{
        center.x + config_.orbitRadius * std::cos(angleRad),
        center.y + config_.orbitHeight,
        center.z + config_.orbitRadius * std::sin(angleRad)
    };
}

///-------------------------------------------///
/// 現在角度から次フレームの移動方向を計算
/// BossEnemyBullet は velocity = direction * speed で動くため、
/// 「1フレーム後の位置 - 現在位置」を正規化して向きだけを返す。
///-------------------------------------------///
Vector3 BossAttackOrbitingOrbsComponent::CalcOrbitDirection(
    const Vector3& center, float angleRad, float deltaTime) const
{
    // 次フレームの角度
    const float nextAngleRad = angleRad + config_.orbitSpeedDeg * kDegToRad * deltaTime;

    const Vector3 currentPos = CalcOrbitPosition(center, angleRad);
    const Vector3 nextPos = CalcOrbitPosition(center, nextAngleRad);

    Vector3 dir = {
        nextPos.x - currentPos.x,
        nextPos.y - currentPos.y,
        nextPos.z - currentPos.z
    };

    // 正規化（長さがほぼ0なら前向きにフォールバック）
    const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (len > 1e-6f) {
        dir.x /= len;
        dir.y /= len;
        dir.z /= len;
    } else {
        dir = { 0.0f, 0.0f, 1.0f };
    }

    return dir;
}
