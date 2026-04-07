#include "AttackEditorScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// ServiceLocator
#include "Service/DeltaTime.h"
#include "Service/Particle.h"
#include "Service/Camera.h"
#include "Service/Collision.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
AttackEditorScene::~AttackEditorScene() {
    // ISceneのデストラクタ
    IScene::~IScene();
    attackEditor_.reset();
	previewPlayer_.reset();
    line_.reset();
    // Colliderのリセット
    Service::Collision::Reset();
    // Camera
    Service::Camera::Remove("Editor");
    camera_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void AttackEditorScene::Initialize() {
    // ISceneの初期化(デフォルトカメラとカメラマネージャ)
    IScene::Initialize();

	// パーティクルの定義を読み込み
    Service::Particle::LoadParticleDefinition("PlayerAttack.json");

    /// ===カメラ=== ///
    camera_ = std::make_unique<MiiEngine::FollowCamera>();
    camera_->Initialize();
    camera_->SetTranslate(cameraPosition_);
    camera_->SetRotate(cameraRotation_);
    camera_->SetFollowCamera(MiiEngine::FollowCameraType::Orbiting);
    // カメラの設定
    Service::Camera::AddCamera("Editor", camera_.get());
    Service::Camera::SetActiveCamera("Editor");

    /// ===エディターの初期化=== ///
    attackEditor_ = std::make_unique<AttackEditor>();
    attackEditor_->Initialize();
    attackEditor_->SetVisible(true);

    /// ===Line=== ///
    line_ = std::make_unique<Line>();

    /// ===プレビュー用プレイヤーの初期化=== ///
    previewPlayer_ = std::make_unique<Player>();
    previewPlayer_->Initialize();
    // プレビュー用プレイヤーにカメラを設定
    previewPlayer_->SetCameraTargetPlayer();

    // エディターにプレビュー用プレイヤーとカメラを設定
	attackEditor_->SetPreviewLeftHand(previewPlayer_->GetLeftHand());
	attackEditor_->SetPreviewRightHand(previewPlayer_->GetRightHand());
    attackEditor_->SetPreviewPlayer(previewPlayer_->GetWeapon());
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void AttackEditorScene::Update() {
    /// ===エディターの更新=== ///
    if (attackEditor_) {
        attackEditor_->Update(Service::DeltaTime::GetDeltaTime());
    }

    /// ===ImGui描画=== ///
#ifdef USE_IMGUI
    RenderMenuBar();
    /// ===デバッグ時のカメラ=== ///
    camera_->ImGuiUpdate();
    camera_->DebugUpdate();

    // エディターメインウィンドウ
    attackEditor_->Render();

    // サブウィンドウ
    if (showHelp_) {
        RenderHelpWindow();
    }
    if (showStats_) {
        RenderStatsWindow();
    }
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void AttackEditorScene::Draw() {
    /// ===グリッドの描画=== ///
    if (showGrid_) {
        line_->DrawGrid({ 0.0f, 0.0f, 0.0f }, { 500.0f, 0.0f, 500.0f }, 25, { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    /// ===エディタープレビューの描画=== ///
    if (attackEditor_) {
        attackEditor_->DrawPreview();
    }

    previewPlayer_->Draw();
}

///-------------------------------------------/// 
/// メニューバーの描画
///-------------------------------------------///
void AttackEditorScene::RenderMenuBar() {
#ifdef USE_IMGUI
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("表示")) {
            ImGui::Checkbox("ヘルプ", &showHelp_);
            ImGui::Checkbox("統計情報", &showStats_);
            ImGui::Checkbox("グリッド", &showGrid_);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("エディター")) {
            if (ImGui::MenuItem("攻撃エディターを表示/非表示")) {
                attackEditor_->ToggleVisibility();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
#endif
}

///-------------------------------------------/// 
/// ヘルプウィンドウの描画
///-------------------------------------------///
void AttackEditorScene::RenderHelpWindow() {
#ifdef USE_IMGUI
    ImGui::Begin("ヘルプ", &showHelp_);

    ImGui::Text("攻撃エディターの使い方:");
    ImGui::Separator();
    ImGui::BulletText("左側のリストから攻撃を選択");
    ImGui::BulletText("右側のタブで各設定を編集");
    ImGui::BulletText("「保存」ボタンでJSONに保存");
    ImGui::BulletText("プレビューで軌道を確認");

    ImGui::Separator();
    ImGui::Text("ショートカットキー:");
    ImGui::BulletText("Ctrl+N:  新規作成");
    ImGui::BulletText("Ctrl+S: 保存");
    ImGui::BulletText("Ctrl+O: 読み込み");

    ImGui::End();
#endif
}

///-------------------------------------------/// 
/// 統計情報ウィンドウの描画
///-------------------------------------------///
void AttackEditorScene::RenderStatsWindow() {
#ifdef USE_IMGUI
    ImGui::Begin("統計情報", &showStats_);

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("フレーム時間: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("デルタタイム: %.3f", Service::DeltaTime::GetDeltaTime());

    ImGui::End();
#endif
}