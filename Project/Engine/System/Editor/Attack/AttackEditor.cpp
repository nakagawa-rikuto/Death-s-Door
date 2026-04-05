#include "AttackEditor.h"
// PlayerWeapon
#include "application/Game/Entity/Player/Weapon/PlayerWeapon.h"
// PlayerHand
#include "application/Game/Entity/Player/Hand/PlayerHand.h"
// Service
#include "Service/Locator.h"
// Line
#include "Engine/Graphics/3d/Line/LineObject3D.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include <filesystem>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
AttackEditor::~AttackEditor() {
    line_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void AttackEditor::Initialize() {
    // Lineの初期化
    line_ = std::make_unique<Line>();

    // デフォルトのファイルパスを設定
    strcpy_s(filePathBuffer_, kDefaultSavePath);

    // 利用可能なプリセットを更新
    UpdateAvailablePresets();

    // デフォルトの攻撃データを作成
    CreateNew();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void AttackEditor::Update(const float deltaTime) {
	// 表示されていなければ処理しない
    if (!isVisible_) return;

    // プレビューの更新
    if (isPlaying_) {
        UpdateTrajectoryPreview(deltaTime);
    }
}

///-------------------------------------------/// 
/// ImGui描画
///-------------------------------------------///
void AttackEditor::Render() {
#ifdef USE_IMGUI
    if (!isVisible_) return;

    ImGui::Begin("プレイヤー攻撃エディター", &isVisible_, ImGuiWindowFlags_MenuBar);

    // メニューバー
    RenderMenuBar();

    // 左側：攻撃リスト
    ImGui::BeginChild("AttackList", ImVec2(250, 0), true);
    RenderAttackList();
    ImGui::EndChild();

    ImGui::SameLine();

    // 右側：編集パネル
    ImGui::BeginChild("EditPanel", ImVec2(0, 0), true);
    if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
        AttackData& currentAttack = attacks_[selectedAttackIndex_];

        // タブで分割
        if (ImGui::BeginTabBar("AttackEditorTabs")) {

            if (ImGui::BeginTabItem("基本設定")) {
                RenderBasicSettings(currentAttack);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("タイミング")) {
                RenderTimingSettings(currentAttack);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("武器の軌道")) {
                RenderTrajectorySettings(currentAttack);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("手の軌道")) {
                RenderDualHandTrajectorySettings(currentAttack);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("エフェクト")) {
                RenderEffectSettings(currentAttack);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("コンボ")) {
                RenderComboSettings(currentAttack);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Separator();

        // 保存ボタン群
        if (ImGui::Button("上書き保存 (Ctrl+S)", ImVec2(-1, 0))) {
            SaveCurrent();
        }

        if (ImGui::Button("名前を付けて保存", ImVec2(-1, 0))) {
            SaveCurrentAs();
        }

        // ファイルパス表示
        auto it = attackFilePaths_.find(currentAttack.attackID);
        if (it != attackFilePaths_.end()) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "保存先:");
            ImGui::TextWrapped("%s", it->second.c_str());
        } else {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "未保存");
        }

    } else {
        ImGui::Text("攻撃を選択してください");
    }
    ImGui::EndChild();

    ImGui::End();

    // ===ポップアップ群=== ///

    // 保存エラー
    if (ImGui::BeginPopupModal("保存エラー", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("エラー: 攻撃名が入力されていません。");
        ImGui::Separator();
        ImGui::Text("攻撃を保存するには、「基本設定」タブで");
        ImGui::Text("攻撃名を入力してください。");

        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // 保存成功
    if (ImGui::BeginPopupModal("保存成功", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
            AttackData& currentAttack = attacks_[selectedAttackIndex_];

            ImGui::Text("保存が完了しました。");

            auto it = attackFilePaths_.find(currentAttack.attackID);
            if (it != attackFilePaths_.end()) {
                ImGui::Separator();
                ImGui::TextWrapped("ファイル:  %s", it->second.c_str());
            }
        }

        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // 保存失敗
    if (ImGui::BeginPopupModal("保存失敗", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("エラー: ファイルの保存に失敗しました。");
        ImGui::Separator();
        ImGui::Text("ファイルパスを確認してください。");

        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // 上書き確認
    if (ImGui::BeginPopupModal("上書き確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("ファイルが既に存在します。");
        ImGui::Text("上書きしますか？");

        ImGui::Spacing();
        if (ImGui::Button("はい", ImVec2(120, 0))) {
            if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
                AttackData& currentAttack = attacks_[selectedAttackIndex_];
                std::string savePath = std::string(kDefaultSavePath) + currentAttack.attackName + ".json";

                if (serializer_->SaveToJson(currentAttack, savePath)) {
                    attackFilePaths_[currentAttack.attackID] = savePath;
                    ImGui::OpenPopup("保存成功");
                } else {
                    ImGui::OpenPopup("保存失敗");
                }
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("いいえ", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // プレビューコントロールウィンドウ
    RenderPreviewControl();
#endif
}

///-------------------------------------------/// 
/// プレビュー描画
///-------------------------------------------///
void AttackEditor::DrawPreview() {
	// 表示状態と選択中の攻撃を確認
    if (!isVisible_ || selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) {
        return;
    }

	// 現在の攻撃データを取得
    AttackData& currentAttack = attacks_[selectedAttackIndex_];

	// 軌道表示が無効なら終了
    if (!currentAttack.showTrajectory) {
        return;
    }

    /// ===右手=== ///
    if (currentAttack.isRightHandAttack) {
        // Dataの取得
        std::vector<MiiEngine::BezierControlPointData> HandWorld;
        for (const auto& point : currentAttack.rightHandTrajectoryPoints) {
            MiiEngine::BezierControlPointData worldPoint = {};
            worldPoint.position = point.position;
            worldPoint.rotation = point.rotation;
            worldPoint.time = point.time;
            HandWorld.push_back(worldPoint);
        }
        // 右手軌道（緑）
        Vector4 color = { currentAttack.rightHandColor.x, currentAttack.rightHandColor.y, currentAttack.rightHandColor.z, 1.0f };
        line_->CreateSmoothCurve(HandWorld, color, currentAttack.curveSegments);
        // 制御点を可視化（右手）
        Service::Locator::GetLineObject3D()->DrawBezierControlPoints(
            HandWorld,
            Vector4{ 0.5f, 1.0f, 0.5f, 1.0f },  // 明るい緑
            Vector4{ 0.2f, 0.8f, 0.2f, 0.5f },  // 半透明緑
            AttackData::kControlPointSize
        );
    }

    /// ===左手=== ///
    if (currentAttack.isLeftHandAttack) {
        // Dataの取得
        std::vector<MiiEngine::BezierControlPointData> HandWorld;
        for (const auto& point : currentAttack.leftHandTrajectoryPoints) {
            MiiEngine::BezierControlPointData worldPoint = {};
            worldPoint.position = point.position;
			worldPoint.rotation = point.rotation;
            worldPoint.time = point.time;
            HandWorld.push_back(worldPoint);
        }
        // 左手軌道（青）
        Vector4 color = { currentAttack.leftHandColor.x, currentAttack.leftHandColor.y, currentAttack.leftHandColor.z, 1.0f };
        line_->CreateSmoothCurve(HandWorld, color, currentAttack.curveSegments);
        // 制御点を可視化（左手）
        Service::Locator::GetLineObject3D()->DrawBezierControlPoints(
            HandWorld,
            Vector4{ 0.5f, 0.5f, 1.0f, 1.0f },  // 明るい青
            Vector4{ 0.2f, 0.2f, 0.8f, 0.5f },  // 半透明青
            AttackData::kControlPointSize
        );
    }

    /// ===武器=== ///
    // Dataの取得
    std::vector<MiiEngine::BezierControlPointData> WeaponWorld;
    for (const auto& point : currentAttack.weaponTrajectoryPoints) {
        MiiEngine::BezierControlPointData worldPoint = {};
        worldPoint.position = point.position;
        worldPoint.rotation = point.rotation;
        worldPoint.time = point.time;
        WeaponWorld.push_back(worldPoint);
    }
    // 武器軌道（赤）
    Vector4 weaponColor = { currentAttack.weaponColor.x, currentAttack.weaponColor.y, currentAttack.weaponColor.z, 1.0f };
    line_->CreateSmoothCurve(WeaponWorld, weaponColor, currentAttack.curveSegments);
    // 制御点を可視化（武器）
    Service::Locator::GetLineObject3D()->DrawBezierControlPoints(
        WeaponWorld,
        Vector4{ 1.0f, 0.5f, 0.5f, 1.0f },  // 明るい赤
        Vector4{ 0.8f, 0.2f, 0.2f, 0.5f },  // 半透明赤
        AttackData::kControlPointSize
    );

    // プレビュー用武器の描画（オプション）
    if (previewWeapon_ && previewRightHand_ && previewLeftHand_&& isPlaying_) {
        previewWeapon_->Draw(MiiEngine::BlendMode::KBlendModeNormal);
		previewLeftHand_->Draw(MiiEngine::BlendMode::KBlendModeNormal);
		previewRightHand_->Draw(MiiEngine::BlendMode::KBlendModeNormal);
    }
}

///-------------------------------------------/// 
/// 表示/非表示切り替え
///-------------------------------------------///
void AttackEditor::ToggleVisibility() {
    isVisible_ = !isVisible_;
}

///-------------------------------------------/// 
/// 表示状態を設定
///-------------------------------------------///
void AttackEditor::SetVisible(bool visible) {
    isVisible_ = visible;
}

///-------------------------------------------/// 
/// 選択中の攻撃を上書き保存
///-------------------------------------------///
void AttackEditor::SaveCurrent() {
#ifdef USE_IMGUI
    if (selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) {
        return;
    }

    AttackData& currentAttack = attacks_[selectedAttackIndex_];

    // 名前チェック
    if (currentAttack.attackName.empty()) {
        ImGui::OpenPopup("保存エラー");
        return;
    }

    // ファイルパスを取得または生成
    std::string savePath;

    auto it = attackFilePaths_.find(currentAttack.attackID);
    if (it != attackFilePaths_.end()) {
        // 既存のファイルパスがあれば上書き
        savePath = it->second;
    } else {
        // 新規の場合は新しいパスを生成
        savePath = std::string(kDefaultSavePath) + currentAttack.attackName + ".json";
        attackFilePaths_[currentAttack.attackID] = savePath;
    }

    // 保存実行
    if (serializer_->SaveToJson(currentAttack, savePath)) {
        ImGui::OpenPopup("保存成功");
    } else {
        ImGui::OpenPopup("保存失敗");
    }
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 選択中の攻撃を名前を付けて保存
///-------------------------------------------///
void AttackEditor::SaveCurrentAs() {
#ifdef USE_IMGUI
    if (selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) {
        return;
    }

    AttackData& currentAttack = attacks_[selectedAttackIndex_];

    // 名前チェック
    if (currentAttack.attackName.empty()) {
        ImGui::OpenPopup("保存エラー");
        return;
    }

    // 常に新しいファイル名で保存
    std::string savePath = std::string(kDefaultSavePath) + currentAttack.attackName + ".json";

    // ファイルが既に存在する場合は確認
    if (std::filesystem::exists(savePath)) {
        ImGui::OpenPopup("上書き確認");
        return;
    }

    // 保存実行
    if (serializer_->SaveToJson(currentAttack, savePath)) {
        // 新しいパスを記録
        attackFilePaths_[currentAttack.attackID] = savePath;
        ImGui::OpenPopup("保存成功");
    } else {
        ImGui::OpenPopup("保存失敗");
    }
#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// JSONファイルに保存
///-------------------------------------------///
void AttackEditor::SaveToJson() {
    SaveCurrent();
}

///-------------------------------------------/// 
/// JSONファイルから読み込み
///-------------------------------------------///
void AttackEditor::LoadFromJson() {

}

///-------------------------------------------/// 
/// 新規作成
///-------------------------------------------///
void AttackEditor::CreateNew() {
    AttackData newAttack;
    newAttack.attackName = "NewAttack_" + std::to_string(static_cast<int>(attacks_.size()));
    newAttack.attackID = static_cast<int>(attacks_.size());
    attacks_.push_back(newAttack);
    selectedAttackIndex_ = static_cast<int>(attacks_.size()) - 1;

	// 回転データを初期化
	UpdateRotationEditData();
}

///-------------------------------------------/// 
/// 全ての攻撃を保存
///-------------------------------------------///
void AttackEditor::SaveAllAttacks() {
    int successCount = 0;
    int failCount = 0;

    for (const auto& attack : attacks_) {
        // 名前が空の攻撃はスキップ
        if (attack.attackName.empty()) {
            failCount++;
            continue;
        }

        std::string savePath = std::string(kDefaultSavePath) + attack.attackName + ".json";
        if (serializer_->SaveToJson(attack, savePath)) {
            successCount++;
        } else {
            failCount++;
        }
    }

#ifdef USE_IMGUI
    // 結果を表示
    if (failCount > 0) {
        ImGui::OpenPopup("一括保存結果");
    }
#endif
}

///-------------------------------------------/// 
/// プリセットフォルダから全JSONを読み込み
///-------------------------------------------///
void AttackEditor::LoadAllAttacks() {
    attacks_.clear();
    attackFilePaths_.clear();  // クリア

    if (!std::filesystem::exists(kDefaultSavePath)) {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(kDefaultSavePath)) {
        if (entry.path().extension() == ".json") {
            AttackData data;
            std::string filepath = entry.path().string();

            if (serializer_->LoadFromJson(data, filepath)) {
                attacks_.push_back(data);

                // ファイルパスを記録
                attackFilePaths_[data.attackID] = filepath;
            }
        }
    }

    // 最初の攻撃が選択されている場合、回転データを更新
    if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
        UpdateRotationEditData();
    }
}

///-------------------------------------------/// 
/// プレビュー再生開始
///-------------------------------------------///
void AttackEditor::PlayPreview() {
    isPlaying_ = true;
    previewTimer_ = 0.0f;
}

///-------------------------------------------/// 
/// プレビュー停止
///-------------------------------------------///
void AttackEditor::StopPreview() {
    isPlaying_ = false;
}

///-------------------------------------------/// 
/// プレビューリセット
///-------------------------------------------///
void AttackEditor::ResetPreview() {
    previewTimer_ = 0.0f;
}

///-------------------------------------------/// 
/// メニューバーの描画
///-------------------------------------------///
void AttackEditor::RenderMenuBar() {
#ifdef USE_IMGUI
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("ファイル")) {
            if (ImGui::MenuItem("新規作成", "Ctrl+N")) {
                CreateNew();
            }

            ImGui::Separator();

            // 上書き保存
            bool canSave = (selectedAttackIndex_ >= 0 &&
                selectedAttackIndex_ < static_cast<int>(attacks_.size()) &&
                !attacks_[selectedAttackIndex_].attackName.empty());

            if (ImGui::MenuItem("上書き保存", "Ctrl+S", false, canSave)) {
                SaveCurrent();
            }

            if (!canSave && ImGui::IsItemHovered()) {
                ImGui::SetTooltip("保存するには攻撃名を入力してください");
            }

            // 名前を付けて保存
            if (ImGui::MenuItem("名前を付けて保存", "Ctrl+Shift+S", false, canSave)) {
                SaveCurrentAs();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("全て保存")) {
                SaveAllAttacks();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("開く", "Ctrl+O")) {
                LoadFromJson();
            }
            if (ImGui::MenuItem("全て読み込み")) {
                LoadAllAttacks();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("編集")) {
            if (ImGui::MenuItem("削除", nullptr, false, selectedAttackIndex_ >= 0)) {
                DeleteSelectedAttack();
            }
            if (ImGui::MenuItem("複製", nullptr, false, selectedAttackIndex_ >= 0)) {
                DuplicateSelectedAttack();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
#endif
}

///-------------------------------------------/// 
/// 攻撃リストの描画
///-------------------------------------------///
void AttackEditor::RenderAttackList() {
#ifdef USE_IMGUI
    ImGui::Text("攻撃リスト");
    ImGui::Separator();

    for (int i = 0; i < static_cast<int>(attacks_.size()); ++i) {
        bool isSelected = (selectedAttackIndex_ == i);
        if (ImGui::Selectable(attacks_[i].attackName.c_str(), isSelected)) {
            selectedAttackIndex_ = i;
            // 選択された攻撃の回転データをオイラー角に変換
            UpdateRotationEditData();
        }
    }

    ImGui::Separator();
    if (ImGui::Button("+ 新規追加", ImVec2(-1, 0))) {
        CreateNew();
    }
#endif
}

///-------------------------------------------/// 
/// 基本設定UIの描画
///-------------------------------------------///
void AttackEditor::RenderBasicSettings(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("基本情報");

    char nameBuffer[256];
    strncpy_s(nameBuffer, data.attackName.c_str(), sizeof(nameBuffer));

    // 名前が空の場合は警告色で表示
    if (data.attackName.empty()) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.6f, 0.2f, 0.2f, 0.5f)); // 赤っぽい背景
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));    // 黄色いテキスト
    }

    if (ImGui::InputText("攻撃名", nameBuffer, sizeof(nameBuffer))) {
        data.attackName = nameBuffer;
    }

    if (data.attackName.empty()) {
        ImGui::PopStyleColor(2);

        // ヘルプテキスト
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(*)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("保存するには攻撃名が必要です");
        }
    }

    char descBuffer[512];
    strncpy_s(descBuffer, data.description.c_str(), sizeof(descBuffer));
    if (ImGui::InputTextMultiline("説明", descBuffer, sizeof(descBuffer), ImVec2(-1, 60))) {
        data.description = descBuffer;
    }

    ImGui::InputInt("攻撃ID", &data.attackID);

    ImGui::SeparatorText("プレイヤーの動き");
    ImGui::SliderFloat("移動速度倍率", &data.moveSpeedMultiplier, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat3("ルートモーション", &data.rootMotion.x, 0.1f);
#endif
}

///-------------------------------------------/// 
/// タイミング設定UIの描画
///-------------------------------------------///
void AttackEditor::RenderTimingSettings(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("タイミング設定");
    ImGui::DragFloat("攻撃時間 (秒)", &data.activeDuration, 0.01f, 0.01f, 10.0f, "%.2f");
    ImGui::DragFloat("コンボ受付時間 (秒)", &data.comboWindowTime, 0.01f, 0.0f, 5.0f, "%.2f");
    ImGui::DragFloat("クールダウン (秒)", &data.cooldownTime, 0.01f, 0.0f, 5.0f, "%.2f");
#endif
}

///-------------------------------------------/// 
/// 軌道設定UIの描画
///-------------------------------------------///
void AttackEditor::RenderTrajectorySettings(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("武器の軌道設定");

    ImGui::DragFloat("武器の距離", &data.weaponLength, 0.1f, 1.0f, 50.0f);
    ImGui::DragInt("曲線の分割数", &data.curveSegments, 1, 5, 100);

    ImGui::Spacing();
    ImGui::Separator();

    // ベジェ曲線の制御点リスト
    ImGui::SeparatorText("ベジェ曲線の制御点");
    RenderBezierControlPointList(data.weaponTrajectoryPoints, "Weapon", TrajectoryType::WeaponTrajectory);

    ImGui::Spacing();
    ImGui::Separator();

    // プレビュー設定
    ImGui::SeparatorText("プレビュー");
    ImGui::Checkbox("軌道を表示", &data.showTrajectory);
    ImGui::ColorEdit3("軌道の色", &data.weaponColor.x);
#endif
}

///-------------------------------------------/// 
/// 両手軌道設定UIの描画
///-------------------------------------------///
void AttackEditor::RenderDualHandTrajectorySettings(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("手の攻撃設定");

	// 攻撃モード選択
    ImGui::Checkbox("左手攻撃", &data.isLeftHandAttack);
    ImGui::Checkbox("右手攻撃", &data.isRightHandAttack);

    ImGui::Spacing();
    ImGui::Separator();

    // 共通設定
    ImGui::DragInt("曲線の分割数", &data.curveSegments, 1, 5, 100);
    // 色設定
    ImGui::ColorEdit3("左手軌道の色", &data.leftHandColor.x);
    ImGui::ColorEdit3("右手軌道の色", &data.rightHandColor.x);
    
    ImGui::Spacing();
    ImGui::Separator();

    // 左手のベジェ曲線の表示
    ImGui::BeginChild("LeftHandPanel", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0), true);
    ImGui::TextColored(ImVec4(0.3f, 0.3f, 1.0f, 1.0f), "左手 (Left Hand)");
    ImGui::Separator();
    RenderBezierControlPointList(data.leftHandTrajectoryPoints, "LeftHand", TrajectoryType::LeftHandTrajectory);
    ImGui::EndChild();

    ImGui::SameLine();

    // 右手のベジェ曲線の表示
    ImGui::BeginChild("RightHandPanel", ImVec2(0, 0), true);
    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "右手 (Right Hand)");
    ImGui::Separator();
    RenderBezierControlPointList(data.rightHandTrajectoryPoints, "RightHand", TrajectoryType::RightHandTrajectory);
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();

#endif
}

///-------------------------------------------/// 
/// エフェクト設定UIの描画
///-------------------------------------------///
void AttackEditor::RenderEffectSettings(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("パーティクルエフェクト");

    char particleBuffer[256];
    strncpy_s(particleBuffer, data.particleEffectName.c_str(), sizeof(particleBuffer));
    if (ImGui::InputText("エフェクト名", particleBuffer, sizeof(particleBuffer))) {
        data.particleEffectName = particleBuffer;
    }

    ImGui::DragFloat3("エフェクトオフセット", &data.particleOffset.x, 0.1f);

    ImGui::SeparatorText("カメラシェイク");
    ImGui::DragFloat("シェイク強度", &data.cameraShakeIntensity, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("シェイク時間", &data.cameraShakeDuration, 0.01f, 0.0f, 5.0f);

    ImGui::SeparatorText("サウンド");

    char swingBuffer[256];
    strncpy_s(swingBuffer, data.swingSoundName.c_str(), sizeof(swingBuffer));
    if (ImGui::InputText("振りサウンド", swingBuffer, sizeof(swingBuffer))) {
        data.swingSoundName = swingBuffer;
    }

    char hitBuffer[256];
    strncpy_s(hitBuffer, data.hitSoundName.c_str(), sizeof(hitBuffer));
    if (ImGui::InputText("ヒットサウンド", hitBuffer, sizeof(hitBuffer))) {
        data.hitSoundName = hitBuffer;
    }
#endif
}

///-------------------------------------------/// 
/// コンボ設定UIの描画
///-------------------------------------------///
void AttackEditor::RenderComboSettings(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("コンボ連携");

    ImGui::Checkbox("次のコンボにつなげる", &data.canComboToNext);

    if (data.canComboToNext) {
        ImGui::InputInt("次のコンボID", &data.nextComboID);

        ImGui::SeparatorText("分岐コンボ");
        ImGui::Text("分岐可能なコンボID:");

        for (int i = 0; i < static_cast<int>(data.branchComboIDs.size()); ++i) {
            ImGui::PushID(i);
            ImGui::InputInt("branchID", &data.branchComboIDs[i]);
            ImGui::SameLine();
            if (ImGui::Button("削除")) {
                data.branchComboIDs.erase(data.branchComboIDs.begin() + i);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
        }

        if (ImGui::Button("+ 分岐を追加")) {
            data.branchComboIDs.push_back(-1);
        }
    }
#endif
}

///-------------------------------------------/// 
/// プレビューコントロールの描画
///-------------------------------------------///
void AttackEditor::RenderPreviewControl() {
#ifdef USE_IMGUI
    ImGui::Begin("プレビューコントロール");

    if (ImGui::Button("再生")) {
        PlayPreview();
    }
    ImGui::SameLine();
    if (ImGui::Button("停止")) {
        StopPreview();
    }
    ImGui::SameLine();
    if (ImGui::Button("リセット")) {
        ResetPreview();
    }

    ImGui::Checkbox("自動リプレイ", &autoReplay_);

    ImGui::SeparatorText("プレビュー設定");
	ImGui::DragFloat3("武器位置", &previewPlayerPosition_.x, 0.1f); // 後々Player位置の変更にする予定

    // 実際の攻撃をプレビュー
    if (ImGui::Button("攻撃プレビュー実行", ImVec2(-1, 0)) && previewWeapon_) {
		// 選択中の攻撃を確認
        if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
			// 現在の攻撃データを取得
            AttackData& currentAttack = attacks_[selectedAttackIndex_];
            
            /// ===武器=== ///
			// 武器の軌道ポイントが2つ以上あることを確認
            if (currentAttack.weaponTrajectoryPoints.size() >= 2) {

                // 全ての制御点にオフセットを適用
                std::vector<MiiEngine::BezierControlPointData> weaponPoints;
                for (const auto& point : currentAttack.weaponTrajectoryPoints) {
                    MiiEngine::BezierControlPointData weaponPoint = {};
                    weaponPoint.position = previewPlayerPosition_ + point.position;
					weaponPoint.rotation = point.rotation;
                    weaponPoint.time = point.time;
                    weaponPoints.push_back(weaponPoint);
                }

                // 武器に攻撃を実行させる（ベジェ曲線版）
                previewWeapon_->StartAttack(weaponPoints, currentAttack.activeDuration);
            }

            /// ===右手攻撃=== ///
			// 右手攻撃が有効で、プレビューオブジェクトが存在する場合
            if (currentAttack.isRightHandAttack && previewRightHand_) {
				// 右手の軌道ポイントが2つ以上あることを確認
                if (currentAttack.rightHandTrajectoryPoints.size() >= 2) {
					// 全ての制御点にオフセットを適用
                    std::vector<MiiEngine::BezierControlPointData> rightHandPoints;
                    for (const auto& point : currentAttack.rightHandTrajectoryPoints) {
                        MiiEngine::BezierControlPointData handPoint = {};
                        handPoint.position = previewPlayerPosition_ + point.position;
						handPoint.rotation = point.rotation;
                        handPoint.time = point.time;
                        rightHandPoints.push_back(handPoint);
                    }
					// 右手に攻撃を実行させる（ベジェ曲線版）
                    previewRightHand_->StartAttack(rightHandPoints, currentAttack.activeDuration);
                }
			}

            /// ===左手攻撃=== ///
            // 左手攻撃が有効で、プレビューオブジェクトが存在する場合
            if (currentAttack.isLeftHandAttack && previewLeftHand_) {
                // 左手の軌道ポイントが2つ以上あることを確認
                if (currentAttack.leftHandTrajectoryPoints.size() >= 2) {
                    // 全ての制御点にオフセットを適用
                    std::vector<MiiEngine::BezierControlPointData> leftHandPoints;
                    for (const auto& point : currentAttack.leftHandTrajectoryPoints) {
                        MiiEngine::BezierControlPointData handPoint = {};
                        handPoint.position = previewPlayerPosition_ + point.position;
                        handPoint.rotation = point.rotation;
                        handPoint.time = point.time;
                        leftHandPoints.push_back(handPoint);
                    }
					// 左手に攻撃を実行させる（ベジェ曲線版）
                    previewLeftHand_->StartAttack(leftHandPoints, currentAttack.activeDuration);
                }
            }

            /// ===プレビュー再生開始=== ///
            PlayPreview();
        }
    }
    ImGui::Separator();
    ImGui::End();
#endif
}

///-------------------------------------------/// 
/// ベジェ曲線制御点リストのUI描画
///-------------------------------------------///
void AttackEditor::RenderBezierControlPointList(std::vector<MiiEngine::BezierControlPointData>& points, const char* label, TrajectoryType type) {
	points;
	label;
    type;
#ifdef USE_IMGUI
    // rotationEditDataのベクターサイズを調整
    std::vector<Vector3>* eulerList = nullptr;
    switch (type) {
    case TrajectoryType::WeaponTrajectory:
        eulerList = &rotationEditData_.weaponEuler;
        break;
    case TrajectoryType::RightHandTrajectory:
        eulerList = &rotationEditData_.rightHandEuler;
        break;
    case TrajectoryType::LeftHandTrajectory:
        eulerList = &rotationEditData_.leftHandEuler;
        break;
    }

    // ベクターサイズが足りない場合は追加
    if (eulerList && eulerList->size() < points.size()) {
        size_t oldSize = eulerList->size();
        eulerList->resize(points.size());
        // 新しく追加された要素をQuaternionから変換して初期化
        for (size_t i = oldSize; i < points.size(); ++i) {
            (*eulerList)[i] = Math::QuaternionToEuler(points[i].rotation);
        }
    }
    // ベクターサイズが多すぎる場合は削減
    else if (eulerList && eulerList->size() > points.size()) {
        eulerList->resize(points.size());
    }

    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        ImGui::PushID((std::string(label) + std::to_string(i)).c_str());

        // 制御点データ
        ImGui::Text("制御点 %d", i);
        ImGui::DragFloat3("位置", &points[i].position.x, 0.1f);

        if (eulerList && i < static_cast<int>(eulerList->size())) {
            ImGui::DragFloat3("回転", &(*eulerList)[i].x, 0.1f);
            // 回転をQuaternionに変換して保存
            points[i].rotation = Math::QuaternionFromVector((*eulerList)[i]);
        }

        ImGui::SliderFloat("時間", &points[i].time, 0.0f, 1.0f, "%.2f");
        ImGui::SameLine();

        // 削除
        if (ImGui::Button("削除") && points.size() > 2) {
            points.erase(points.begin() + i);
            if (eulerList && i < static_cast<int>(eulerList->size())) {
                eulerList->erase(eulerList->begin() + i);
            }
            ImGui::PopID();
            break;
        }
        ImGui::Separator();
        ImGui::PopID();
    }

    // 制御点追加ボタン
    if (ImGui::Button((std::string("+ 制御点を追加_") + label).c_str())) {
        MiiEngine::BezierControlPointData newPoint = {};
        newPoint.position = Vector3{ 0.0f, 0.0f, 0.0f };
        newPoint.rotation = Math::QuaternionFromVector(Vector3{ 0.0f, 0.0f, 0.0f });
        newPoint.time = 0.5f;
        points.push_back(newPoint);

        // オイラー角リストにも追加
        if (eulerList) {
            eulerList->push_back(Vector3{ 0.0f, 0.0f, 0.0f });
        }
    }

#endif
}

///-------------------------------------------/// 
/// 選択中の攻撃を削除
///-------------------------------------------///
void AttackEditor::DeleteSelectedAttack() {
    if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
        attacks_.erase(attacks_.begin() + selectedAttackIndex_);
        selectedAttackIndex_ = -1;
    }
}

///-------------------------------------------/// 
/// 選択中の攻撃を複製
///-------------------------------------------///
void AttackEditor::DuplicateSelectedAttack() {
    if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
        AttackData copy = attacks_[selectedAttackIndex_];
        copy.attackName += "_Copy";
        copy.attackID = static_cast<int>(attacks_.size());
        attacks_.push_back(copy);
    }
}

///-------------------------------------------/// 
/// 利用可能なプリセットファイルを更新
///-------------------------------------------///
void AttackEditor::UpdateAvailablePresets() {
    availablePresets_.clear();

    if (!std::filesystem::exists(kDefaultSavePath)) {
        std::filesystem::create_directories(kDefaultSavePath);
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(kDefaultSavePath)) {
        if (entry.path().extension() == ".json") {
            availablePresets_.push_back(entry.path().filename().string());
        }
    }
}

///-------------------------------------------/// 
/// 軌道のプレビュー更新
///-------------------------------------------///
void AttackEditor::UpdateTrajectoryPreview(const float deltaTime) {
    if (selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) {
        return;
    }

    AttackData& currentAttack = attacks_[selectedAttackIndex_];

    // タイマーを進める
    previewTimer_ += deltaTime;

    // プレビュー用武器の更新
    if (previewLeftHand_) {
        previewLeftHand_->Update();
    }
    if (previewRightHand_) {
        previewRightHand_->Update();
    }
    if (previewWeapon_) {
        previewWeapon_->Update();
    }

    // 攻撃時間を超えたらリセット
    if (previewTimer_ >= currentAttack.activeDuration) {
        if (autoReplay_) {
            previewTimer_ = 0.0f;
        } else {
            isPlaying_ = false;
        }
    }
}

///-------------------------------------------/// 
/// 選択中の攻撃の回転データをオイラー角に変換して保存
///-------------------------------------------///
void AttackEditor::UpdateRotationEditData() {
    if (selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) {
        return;
    }

    AttackData& currentAttack = attacks_[selectedAttackIndex_];

    // 武器の回転データを変換
    rotationEditData_.weaponEuler.clear();
    for (const auto& point : currentAttack.weaponTrajectoryPoints) {
        rotationEditData_.weaponEuler.push_back(Math::QuaternionToEuler(point.rotation));
    }

    // 右手の回転データを変換
    rotationEditData_.rightHandEuler.clear();
    for (const auto& point : currentAttack.rightHandTrajectoryPoints) {
        rotationEditData_.rightHandEuler.push_back(Math::QuaternionToEuler(point.rotation));
    }

    // 左手の回転データを変換
    rotationEditData_.leftHandEuler.clear();
    for (const auto& point : currentAttack.leftHandTrajectoryPoints) {
        rotationEditData_.leftHandEuler.push_back(Math::QuaternionToEuler(point.rotation));
    }
}
