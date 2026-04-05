#include "PlayerAttackComponent.h"
// PlayerHand
#include "application/Game/Entity/Player/Hand/PlayerHand.h"
// PlayerWeapon
#include "application/Game/Entity/Player/Weapon/PlayerWeapon.h"
// AttackDataSerializer
#include "Engine/System/Editor/Attack/AttackDataSerializer.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI


///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void PlayerAttackComponent::Initialize() {
    // コンボの読み込み
    LoadAttackData(0, "Resource/Json/Attacks/Player_Attack_0.json");
    LoadAttackData(1, "Resource/Json/Attacks/Player_Attack_1.json");
    LoadAttackData(2, "Resource/Json/Attacks/Player_Attack_2.json");
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void PlayerAttackComponent::Update(const float deltaTime) {
    // タイマーを更新
    UpdateTimers(deltaTime);

    // 攻撃中の処理
    if (state_.isActive) {
        // 攻撃時間が終了したかチェック
        const AttackData* currentData = GetAttackData(state_.currentAttackID);

        if (currentData && state_.timer >= currentData->activeDuration) {
            state_.isActive = false;
            state_.previousAttackID = state_.currentAttackID;  // ← 前の攻撃IDを保存
            state_.currentAttackID = -1;

            // コンボ受付開始
            if (currentData->canComboToNext) {
                state_.canCombo = true;
                state_.comboTimer = 0.0f;
            }
        }
    }

    // コンボ受付時間の管理
    if (state_.canCombo) {
        // ★ 前の攻撃IDからデータを取得
        const AttackData* previousData = GetAttackData(state_.previousAttackID);

        if (previousData && state_.comboTimer >= previousData->comboWindowTime) {
            state_.canCombo = false;
            state_.comboCount = 0;
            state_.previousAttackID = -1;  // ← リセット
        }
    }
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void PlayerAttackComponent::Information() {
#ifdef USE_IMGUI
    ImGui::SeparatorText("PlayerAttackComponent");

    ImGui::Text("攻撃中: %s", state_.isActive ? "true" : "false");
    ImGui::Text("現在の攻撃ID: %d", state_.currentAttackID);
    ImGui::Text("前の攻撃ID: %d", state_.previousAttackID);  // ← 追加
    ImGui::Text("コンボ可能: %s", state_.canCombo ? "true" : "false");
    ImGui::Text("コンボ数: %d", state_.comboCount);
    ImGui::Text("攻撃タイマー: %.3f", state_.timer);
    ImGui::Text("コンボタイマー: %.3f", state_.comboTimer);
    ImGui::Text("攻撃進行度: %. 1f%%", GetAttackProgress() * 100.0f);

    // デバッグ情報
    ImGui::Separator();

    if (state_.isActive) {
        const AttackData* currentData = GetAttackData(state_.currentAttackID);
        if (currentData) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "現在の攻撃:  %s", currentData->attackName.c_str());
            ImGui::Text("攻撃時間: %.2f秒", currentData->activeDuration);
            ImGui::Text("終了まで: %. 2f秒", currentData->activeDuration - state_.timer);
        }
    }

    if (state_.canCombo) {
        const AttackData* previousData = GetAttackData(state_.previousAttackID);
        if (previousData) {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "コンボ受付中！");
            ImGui::Text("前の攻撃:  %s", previousData->attackName.c_str());
            ImGui::Text("次の攻撃ID: %d", previousData->nextComboID);
            ImGui::Text("受付時間: %.2f秒", previousData->comboWindowTime);
            ImGui::Text("残り時間: %.2f秒", previousData->comboWindowTime - state_.comboTimer);
        }
    }

    ImGui::Separator();
    ImGui::Text("登録されている攻撃数: %zu", attackDataMap_.size());
#endif
}

///-------------------------------------------/// 
/// 攻撃データをJsonから読み込み
///-------------------------------------------///
bool PlayerAttackComponent::LoadAttackData(int attackID, const std::string& filepath) {
    AttackData data;
    if (AttackDataSerializer::LoadFromJson(data, filepath)) {
        data.attackID = attackID;  // IDを確実に設定
        attackDataMap_[attackID] = data;
        return true;
    }
    return false;
}

///-------------------------------------------/// 
/// 攻撃を開始
///-------------------------------------------///
bool PlayerAttackComponent::StartAttack(
    int attackID, 
    PlayerWeapon* weapon, 
    PlayerHand* rightHand, 
    PlayerHand* leftHand) {

    // 既に攻撃中の場合は失敗
    if (state_.isActive) {
        return false;
    }

	CancelAttack(); // 状態をリセット

    // 攻撃データを取得
    auto it = attackDataMap_.find(attackID);
    if (it == attackDataMap_.end()) {
        return false;
    }

    const AttackData& attackData = it->second;

    // 武器に攻撃を適用
    ApplyAttackToWeapon(attackData, weapon, rightHand, leftHand);

    // 状態を更新
    state_.isActive = true;
    state_.currentAttackID = attackID;
    state_.timer = 0.0f;
    state_.canCombo = false;
    state_.comboCount = 0;

    return true;
}

///-------------------------------------------/// 
/// コンボ攻撃を試行
///-------------------------------------------///
bool PlayerAttackComponent::TryCombo(
    PlayerWeapon* weapon, 
    PlayerHand* rightHand,
    PlayerHand* leftHand) {

    // コンボ可能状態でない場合は失敗
    if (!state_.canCombo) {
        return false;
    }

    // 前の攻撃データを取得
    const AttackData* previousData = GetAttackData(state_.previousAttackID);
    if (!previousData || !previousData->canComboToNext) {
        return false;
    }

    // 次のコンボIDを取得
    int nextAttackID = previousData->nextComboID;
    if (nextAttackID < 0) {
        return false;
    }

    // 次の攻撃を開始
    state_.canCombo = false;
    state_.comboCount++;

    return StartAttack(nextAttackID, weapon, rightHand, leftHand);
}

///-------------------------------------------/// 
/// 攻撃をキャンセル
///-------------------------------------------///
void PlayerAttackComponent::CancelAttack() {
    state_.isActive = false;
    state_.canCombo = false;
    state_.currentAttackID = -1;
    state_.previousAttackID = -1;  // ← リセット
    state_.comboCount = 0;
    state_.timer = 0.0f;
    state_.comboTimer = 0.0f;
}

///-------------------------------------------/// 
/// 現在の攻撃データを取得
///-------------------------------------------///
const AttackData* PlayerAttackComponent::GetCurrentAttackData() const {
    return GetAttackData(state_.currentAttackID);
}

///-------------------------------------------/// 
/// 攻撃データをIDで取得
///-------------------------------------------///
const AttackData* PlayerAttackComponent::GetAttackData(int attackID) const {
    auto it = attackDataMap_.find(attackID);
    if (it != attackDataMap_.end()) {
        return &it->second;
    }
    return nullptr;
}

///-------------------------------------------/// 
/// 現在の攻撃進行度を取得
///-------------------------------------------///
float PlayerAttackComponent::GetAttackProgress() const {
    const AttackData* data = GetCurrentAttackData();
    if (!data || !state_.isActive) {
        return 0.0f;
    }

    return (std::min)(state_.timer / data->activeDuration, 1.0f);
}

///-------------------------------------------/// 
/// 武器に攻撃軌道を設定
///-------------------------------------------///
void PlayerAttackComponent::ApplyAttackToWeapon(
    const AttackData& data, 
    PlayerWeapon* weapon, 
    PlayerHand* rightHand, 
    PlayerHand* leftHand) {

    if (!weapon) {
        return;
    }

    /// ===攻撃スタート=== ///
    if (data.isLeftHandAttack) {
		// 左手攻撃
        leftHand->StartAttack(data.leftHandTrajectoryPoints, data.activeDuration);
    } 
    
    if (data.isRightHandAttack) {
		// 右手攻撃
		rightHand->StartAttack(data.rightHandTrajectoryPoints, data.activeDuration);
    }

	// 武器攻撃
	weapon->StartAttack(data.weaponTrajectoryPoints, data.activeDuration);
}

///-------------------------------------------/// 
/// タイマーの更新
///-------------------------------------------///
void PlayerAttackComponent::UpdateTimers(float deltaTime) {
    if (state_.isActive) {
        state_.timer += deltaTime;
    }

    if (state_.canCombo) {
        state_.comboTimer += deltaTime;
    }
}