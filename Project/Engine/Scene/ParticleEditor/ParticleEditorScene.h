#pragma once
/// ===Include=== ///
#include "Engine/Scene/IScene.h"

#ifdef USE_IMGUI
// Editor
#include "Engine/System/Editor/Particle/ParticleEditor.h"
// Line
#include "application/Drawing/3d/Line.h"
#endif // USE_IMGUI

// c++
#include <memory>

namespace MiiEngine {
	///=====================================================/// 
	/// パーティクルエディターシーン
	///=====================================================///
	class ParticleEditorScene : public IScene {
	public:

		ParticleEditorScene() = default;
		~ParticleEditorScene();

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
#ifdef USE_IMGUI
		/// ===Line=== ///
		std::unique_ptr<Line> line_;
		/// ===エディター=== ///
		std::unique_ptr<ParticleEditor> particleEditor_;

		/// ===エディター設定=== ///
		bool showHelp_ = false;              // ヘルプ表示フラグ
		bool showStats_ = true;              // 統計情報表示フラグ
		bool showGrid_ = true;               // グリッド表示フラグ

		/// ===カメラ制御=== ///
		Vector3 cameraPosition_ = { 0.0f, 5.0f, -10.0f };
		Quaternion cameraRotation_ = { 0.3f, 0.0f, 0.0f };
		float cameraSpeed_ = 0.1f;
		float cameraRotationSpeed_ = 0.02f;
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

		/// <summary>
		/// カメラコントロールウィンドウの描画
		/// </summary>
		void RenderCameraControl();
#endif // USE_IMGUI
	};
}