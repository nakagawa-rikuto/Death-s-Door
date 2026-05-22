#include "AttackEditor.h"
// PlayerWeapon
#include "application/Game/Entity/Player/Weapon/PlayerWeapon.h"
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

            if (ImGui::BeginTabItem("軌道")) {
                RenderTrajectoryChannelList(currentAttack);
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

    // 全チャンネルを順番に描画
    for (const auto& channel : currentAttack.trajectories) {
        if (!channel.enabled || channel.points.size() < 2) {
            continue;
        }

        // 描画用にコピー
        std::vector<MiiEngine::BezierControlPointData> drawPoints = channel.points;

        // ライン色
        Vector4 lineColor = { channel.color.x, channel.color.y, channel.color.z, 1.0f };
        line_->CreateSmoothCurve(drawPoints, lineColor, currentAttack.curveSegments);

        // 制御点の可視化
        Vector4 pointColor = {
            (std::min)(channel.color.x + 0.5f, 1.0f),
            (std::min)(channel.color.y + 0.5f, 1.0f),
            (std::min)(channel.color.z + 0.5f, 1.0f),
            1.0f
        };
        Vector4 pointColorAlpha = { channel.color.x, channel.color.y, channel.color.z, 0.5f };

        // ベジェ曲線の描画
        Service::Locator::GetLineObject3D()->DrawBezierControlPoints(
            drawPoints,
            pointColor,
            pointColorAlpha,
            AttackData::kControlPointSize
        );
    }

    // プレビュー再生中のみオブジェクトを描画
    if (isPlaying_) {
        if (previewWeapon_)    previewWeapon_->Draw(MiiEngine::BlendMode::KBlendModeNormal);
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
    if (selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) return;

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

	// 新しい攻撃の回転データをオイラー角に変換して編集用バッファに追加
    RebuildEulerAnglesBuffer();
}

///-------------------------------------------/// 
/// 全ての攻撃を保存
///-------------------------------------------///
void AttackEditor::SaveAllAttacks() {
    for (const auto& attack : attacks_) {
        if (attack.attackName.empty()) continue;
        std::string savePath = std::string(kDefaultSavePath) + attack.attackName + ".json";
        serializer_->SaveToJson(attack, savePath);
    }
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

	// 読み込んだ攻撃があれば最初の攻撃を選択
    if (!attacks_.empty()) {
        selectedAttackIndex_ = 0;
        RebuildEulerAnglesBuffer();
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
            RebuildEulerAnglesBuffer();
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

    // Push前の状態で判定し、Push/Popが必ず対応するようにフラグで管理する
    const bool isEmpty = data.attackName.empty();
    if (isEmpty) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.6f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    }

    if (ImGui::InputText("攻撃名", nameBuffer, sizeof(nameBuffer))) {
        data.attackName = nameBuffer;
    }

    // Pushした分は必ずPopする
    if (isEmpty) {
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
/// 軌道チャンネル一覧UIの描画
///-------------------------------------------///
void AttackEditor::RenderTrajectoryChannelList(AttackData& data) {
    data;
#ifdef USE_IMGUI
    ImGui::SeparatorText("軌道チャンネル");

    // 共通設定
    ImGui::DragFloat("武器の距離", &data.weaponLength, 0.1f, 1.0f, 50.0f);
    ImGui::DragInt("曲線の分割数", &data.curveSegments, 1, 5, 100);
    ImGui::Checkbox("軌道を表示", &data.showTrajectory);

    ImGui::Spacing();

    // チャンネル追加ボタン
    if (ImGui::Button("+ 軌道チャンネルを追加")) {
        TrajectoryChannel newChannel;
        newChannel.name = "Channel_" + std::to_string(static_cast<int>(data.trajectories.size()));
        newChannel.color = Vector3{ 1.0f, 1.0f, 0.0f }; // デフォルト黄色
        newChannel.enabled = true;
        // デフォルト3点
        newChannel.points.push_back({ Vector3{-5.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.0f });
        newChannel.points.push_back({ Vector3{ 0.0f, 0.0f, 5.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.5f });
        newChannel.points.push_back({ Vector3{ 5.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f });
        data.trajectories.push_back(newChannel);

        // オイラーバッファも追加
        eulerAnglesPerChannel_.push_back(std::vector<Vector3>(newChannel.points.size(), Vector3{ 0,0,0 }));
    }

    ImGui::Separator();

    // 各チャンネルをCollapsibleHeaderで表示
    for (int i = 0; i < static_cast<int>(data.trajectories.size()); ++i) {
        TrajectoryChannel& channel = data.trajectories[i];

        ImGui::PushID(i);

        // ヘッダー: "[0] Weapon "
        std::string headerLabel = "[" + std::to_string(i) + "] " + channel.name;
        bool headerOpen = ImGui::CollapsingHeader(headerLabel.c_str());

        // ヘッダー右側にカラーとチェックボックス
        ImGui::SameLine();
        ImGui::ColorEdit3("##color", &channel.color.x,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Checkbox("##enabled", &channel.enabled);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("有効/無効");

        // 削除ボタン
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("削除") && data.trajectories.size() > 1) {
            data.trajectories.erase(data.trajectories.begin() + i);
            if (i < static_cast<int>(eulerAnglesPerChannel_.size())) {
                eulerAnglesPerChannel_.erase(eulerAnglesPerChannel_.begin() + i);
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
            break; // イテレータ無効化を避けて即break
        }
        ImGui::PopStyleColor();

        if (headerOpen) {
            // チャンネル名編集
            char nameBuf[128];
            strncpy_s(nameBuf, channel.name.c_str(), sizeof(nameBuf));
            if (ImGui::InputText("チャンネル名", nameBuf, sizeof(nameBuf))) {
                channel.name = nameBuf;
            }

            ImGui::TextDisabled("使用側でのインデックス: %d", i);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("PlayerWeaponやPlayerHandでこの番号を指定して参照してください");
            }

            ImGui::Separator();

            // 制御点リスト
            RenderChannelControlPoints(channel, i);
        }

        ImGui::PopID();
        ImGui::Spacing();
    }
#endif
}

///-------------------------------------------/// 
/// 1チャンネル分の制御点リストUIの描画
///-------------------------------------------///
void AttackEditor::RenderChannelControlPoints(TrajectoryChannel& channel, int channelIndex) {
    channel;
    channelIndex;
#ifdef USE_IMGUI
    auto& points = channel.points;

    // オイラーバッファをチャンネルインデックスで参照
    // バッファが足りない場合は拡張
    SyncEulerBuffer(channelIndex, points);
    auto& eulerList = eulerAnglesPerChannel_[channelIndex];

    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        ImGui::PushID(i);

        ImGui::Text("制御点 %d", i);

        ImGui::DragFloat3("位置", &points[i].position.x, 0.1f);

        // 回転はオイラー角で編集→Quaternionに変換して保存
        if (i < static_cast<int>(eulerList.size())) {
            if (ImGui::DragFloat3("回転 (deg)", &eulerList[i].x, 0.5f)) {
                points[i].rotation = Math::QuaternionFromVector(eulerList[i]);
            }
        }

        // 時間スライダー（隣接制御点の時間を超えないようにクランプ）
        float timeMin = (i > 0) ? points[i - 1].time + 0.01f : 0.0f;
        float timeMax = (i < static_cast<int>(points.size()) - 1) ? points[i + 1].time - 0.01f : 1.0f;
        ImGui::SliderFloat("時間", &points[i].time, timeMin, timeMax, "%.2f");

        // 削除（最低2点は残す）
        ImGui::SameLine();
        if (ImGui::Button("削除") && points.size() > 2) {
            points.erase(points.begin() + i);
            eulerList.erase(eulerList.begin() + i);
            ImGui::PopID();
            break;
        }

        ImGui::Separator();
        ImGui::PopID();
    }

    // 末尾に制御点を追加
    if (ImGui::Button("+ 制御点を追加")) {
        MiiEngine::BezierControlPointData newPoint = {};
        newPoint.position = Vector3{ 0.0f, 0.0f, 0.0f };
        newPoint.rotation = Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };

        // 時間は最後の制御点と1.0の中間に挿入
        if (!points.empty()) {
            float lastTime = points.back().time;
            newPoint.time = lastTime + (1.0f - lastTime) * 0.5f;
            newPoint.time = (std::min)(newPoint.time, 1.0f);
        } else {
            newPoint.time = 0.5f;
        }

        points.push_back(newPoint);
        eulerList.push_back(Vector3{ 0.0f, 0.0f, 0.0f });
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

			// チャンネル0を武器の軌道として使用
            const TrajectoryChannel* weaponChannel = currentAttack.GetChannel(0);
            
            /// ===武器=== ///
			// 武器の軌道ポイントが2つ以上あることを確認
            if (weaponChannel && weaponChannel->points.size() >= 2) {

                // 全ての制御点にオフセットを適用
                std::vector<MiiEngine::BezierControlPointData> weaponPoints;
                for (const auto& point : weaponChannel->points) {
                    MiiEngine::BezierControlPointData weaponPoint = {};
                    weaponPoint.position = previewPlayerPosition_ + point.position;
					weaponPoint.rotation = point.rotation;
                    weaponPoint.time = point.time;
                    weaponPoints.push_back(weaponPoint);
                }

                // 武器に攻撃を実行させる（ベジェ曲線版）
                previewWeapon_->StartAttack(weaponPoints, currentAttack.activeDuration);
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
/// 選択中の攻撃を削除
///-------------------------------------------///
void AttackEditor::DeleteSelectedAttack() {
    if (selectedAttackIndex_ >= 0 && selectedAttackIndex_ < static_cast<int>(attacks_.size())) {
        attacks_.erase(attacks_.begin() + selectedAttackIndex_);
        selectedAttackIndex_ = -1;
		eulerAnglesPerChannel_.clear(); // バッファもクリア
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
    if (previewWeapon_) {previewWeapon_->Update();}

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
/// 全チャンネルのオイラー角バッファを再構築
/// 攻撃選択時・チャンネル数変化時に呼ぶ
///-------------------------------------------///
void AttackEditor::RebuildEulerAnglesBuffer() {
    if (selectedAttackIndex_ < 0 || selectedAttackIndex_ >= static_cast<int>(attacks_.size())) {
        eulerAnglesPerChannel_.clear();
        return;
    }

    AttackData& currentAttack = attacks_[selectedAttackIndex_];
    eulerAnglesPerChannel_.resize(currentAttack.trajectories.size());

    for (int ci = 0; ci < static_cast<int>(currentAttack.trajectories.size()); ++ci) {
        auto& points = currentAttack.trajectories[ci].points;
        auto& eulerList = eulerAnglesPerChannel_[ci];
        eulerList.resize(points.size());
        for (int pi = 0; pi < static_cast<int>(points.size()); ++pi) {
            eulerList[pi] = Math::QuaternionToEuler(points[pi].rotation);
        }
    }
}

///-------------------------------------------/// 
/// 指定チャンネルのオイラー角バッファを制御点数に合わせて同期
///-------------------------------------------///
void AttackEditor::SyncEulerBuffer(int channelIndex, std::vector<MiiEngine::BezierControlPointData>& points) {
    // バッファ自体が足りない場合は拡張
    if (channelIndex >= static_cast<int>(eulerAnglesPerChannel_.size())) {
        eulerAnglesPerChannel_.resize(channelIndex + 1);
    }

    auto& eulerList = eulerAnglesPerChannel_[channelIndex];

    // 制御点数に合わせて同期
    if (eulerList.size() < points.size()) {
        size_t oldSize = eulerList.size();
        eulerList.resize(points.size());
        for (size_t i = oldSize; i < points.size(); ++i) {
            eulerList[i] = Math::QuaternionToEuler(points[i].rotation);
        }
    } else if (eulerList.size() > points.size()) {
        eulerList.resize(points.size());
    }
}
