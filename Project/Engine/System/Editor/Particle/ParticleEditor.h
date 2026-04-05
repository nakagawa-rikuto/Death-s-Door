#pragma once
/// ===Include=== ///
#include "Engine/Graphics/Particle/ParticleDefinition.h"
#include "Engine/DataInfo/BlendModeData.h"
// c++
#include <memory>
#include <string>
#include <vector>

namespace MiiEngine {
    /// ===前方宣言=== ///
    class ParticleGroup;

    ///=====================================================/// 
    /// パーティクルエディター
    /// ImGuiを使用したパーティクル編集UI
    ///=====================================================///
    class ParticleEditor {
    public:

        ParticleEditor();
        ~ParticleEditor();

        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize();

        /// <summary>
        /// 更新処理
        /// </summary>
        void Update();

        /// <summary>
        /// ImGUI描画処理
        /// </summary>
        void Render();

        /// <summary>
        /// プレビュー描画処理（ゲーム画面に表示）
        /// </summary>
        void DrawPreview();

        /// <summary>
        /// エディターの表示/非表示切り替え
        /// </summary>
        void ToggleVisibility();

        /// <summary>
        /// エディターの表示状態を設定
        /// </summary>
        void SetVisible(bool visible);

        /// <summary>
        /// エディターが表示されているか
        /// </summary>
        bool IsVisible() const;

    private:

        /// ===ファイル操作=== ///

        /// <summary>
        /// JSONファイルに保存
        /// </summary>
        void SaveToJson();

        /// <summary>
        /// JSONファイルから読み込み
        /// </summary>
        void LoadFromJson();

        /// <summary>
        /// 新規作成
        /// </summary>
        void CreateNew();

        /// ===プレビュー制御=== ///

        /// <summary>
        /// プレビュー再生開始
        /// </summary>
        void PlayPreview();

        /// <summary>
        /// プレビュー停止
        /// </summary>
        void StopPreview();

        /// <summary>
        /// プレビューリセット
        /// </summary>
        void ResetPreview();

        /// <summary>
        /// プレビュー用パーティクルの生成
        /// </summary>
        void CreatePreviewParticle();

        /// ===ImGui描画関数群=== ///

        /// <summary>
        /// メニューバーの描画
        /// </summary>
        void RenderMenuBar();

        /// <summary>
        /// 基本設定UIの描画
        /// </summary>
        void RenderBasicSettings();

        /// <summary>
        /// 物理設定UIの描画
        /// </summary>
        void RenderPhysicsSettings();

        /// <summary>
        /// 見た目設定UIの描画
        /// </summary>
        void RenderAppearanceSettings();

        /// <summary>
        /// 回転設定UIの描画
        /// </summary>
        void RenderRotationSettings();

        /// <summary>
        /// 発生設定UIの描画
        /// </summary>
        void RenderEmissionSettings();

        /// <summary>
        /// 項で設定UIの描画
        /// </summary>
        void RenderAdvancedSettings();

        /// <summary>
        /// プレビューコントロールUIの描画
        /// </summary>
        void RenderPreviewControls();

        /// <summary>
        /// ファイル操作UIの描画
        /// </summary>
        void RenderFileOperations();

        /// ===ヘルパー関数=== ///

        /// <summary>
        /// 利用可能なモデルファイルのリストを取得
        /// </summary>
        void UpdateAvailableModels();

        /// <summary>
        /// 利用可能なテクスチャファイルのリストを取得
        /// </summary>
        void UpdateAvailableTextures();

        /// <summary>
        /// プリセット一覧の取得
        /// </summary>
        void UpdateAvailablePresets();

        /// <summary>
        /// 軌跡プレビューの更新
        /// </summary>
        void UpdateTrajectoryPreview();

        /// <summary>
        ///  プリセット
        /// </summary>
        void ApplyTrajectoryPreset();
        void ApplyExplosionPreset();
        void ApplyMagicCirclePreset();
        void ApplySmokePreset();

    private:

        /// ===エディター状態=== ///
        bool isVisible_ = false;                              // エディターの表示状態
        ParticleDefinition currentDefinition_;                // 編集中のパーティクル定義

        /// ===プレビュー関連=== ///
        std::unique_ptr<ParticleGroup> previewParticle_;     // プレビュー用パーティクル
        bool isPlaying_ = false;                              // 再生中フラグ
        bool autoReplay_ = true;                              // 自動リプレイ
        Vector3 previewPosition_ = { 0.0f, 0.0f, 0.0f };     // プレビュー位置
        BlendMode previewBlendMode_ = BlendMode::kBlendModeAdd; // プレビューブレンドモード

        /// ===ファイル管理=== ///
        char filePathBuffer_[256] = "Assets/Particles/";     // ファイルパス入力バッファ
        std::string currentFilePath_;                         // 現在のファイルパス
        std::vector<std::string> availableModels_;           // 利用可能なモデルリスト
        std::vector<std::string> availableTextures_;         // 利用可能なテクスチャリスト
        std::vector<std::string> availablePresets_;          // 利用可能なプリセットリスト

        /// ===UI状態=== ///
        int selectedModelIndex_ = 0;                         // 選択中のモデルインデックス
        int selectedTextureIndex_ = 0;                       // 選択中のテクスチャインデックス
        int selectedPresetIndex_ = 0;                        // 選択中のプリセットインデックス
        bool showSaveDialog_ = false;                        // 保存ダイアログ表示フラグ
        bool showLoadDialog_ = false;                        // 読み込みダイアログ表示フラグ

        /// ===軌跡プレビュー=== ///
        bool trajectoryPreviewMode_ = false;        // 軌跡プレビューモード
        Vector3 trajectoryStartPos_ = { -3, 0, 0 };   // 軌跡開始位置
        Vector3 trajectoryEndPos_ = { 3, 0, 0 };      // 軌跡終了位置
        float trajectoryProgress_ = 0.0f;           // 軌跡の進行度(0~1)
        float trajectorySpeed_ = 1.0f;              // 軌跡の速度
        Vector3 trajectoryRotation_ = { 0, 0, 0 };    // 軌跡の回転

        /// ===定数=== ///
        static constexpr const char* kDefaultSavePath = "Resource/Json/Particles/";
        static constexpr const char* kFileExtension = ".json";
    };
}