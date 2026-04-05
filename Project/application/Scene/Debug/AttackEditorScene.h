#pragma once
/// ===Include=== ///
#include "Engine/Scene/IScene.h"
// Editor
#include <Engine/System/Editor/Attack/AttackEditor.h>
// Line 
#include "application/Drawing/3d/Line.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// c++
#include <memory>

///=====================================================/// 
/// 攻撃エディターシーン
///=====================================================///
class AttackEditorScene : public MiiEngine::IScene {
public:

    AttackEditorScene() = default;
    ~AttackEditorScene();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;

private:
    /// ===Camera=== ///
    std::unique_ptr<MiiEngine::FollowCamera> camera_;
    /// ===Line=== ///
    std::unique_ptr<Line> line_;
    /// ===エディター=== ///
    std::unique_ptr<AttackEditor> attackEditor_;
    /// ===プレビュー用プレイヤー=== ///
    std::unique_ptr<Player> previewPlayer_;

    /// ===エディター設定=== ///
    bool showHelp_ = false;              // ヘルプ表示フラグ
    bool showStats_ = true;              // 統計情報表示フラグ
    bool showGrid_ = true;               // グリッド表示フラグ   

    /// ===カメラ=== ///
	Vector3 cameraPosition_{ 0.0f, 10.0f, -50.0f };   // カメラ位置
	Quaternion cameraRotation_{ 0.0f, 0.0f, 0.0f, 1.0f }; // カメラ回転

private:

    /// <summary>
    /// メニューバーの描画
    /// </summary>
    void RenderMenuBar();

    /// <summary>
    /// ヘルプウィンドウの描画
    /// </summary>
    void RenderHelpWindow();

    /// <summary>
    /// 統計情報ウィンドウの描画
    /// </summary>
    void RenderStatsWindow();
};