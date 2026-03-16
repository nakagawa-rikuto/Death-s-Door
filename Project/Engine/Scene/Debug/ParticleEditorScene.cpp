#include "ParticleEditorScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Service/Particle.h"
#include "Service/DeltaTime.h"
#ifdef USE_IMGUI
// ImGui
#include <imgui.h>
#endif // USE_IMGUI

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	ParticleEditorScene::~ParticleEditorScene() {
		particleEditor_.reset();
		// ISceneのデストラクタ
		IScene::~IScene();
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void ParticleEditorScene::Initialize() {
		// ISceneの初期化(デフォルトカメラとカメラマネージャ)
		IScene::Initialize();

		/// ===エディターの初期化=== ///
		particleEditor_ = std::make_unique<ParticleEditor>();
		particleEditor_->Initialize();
		particleEditor_->SetVisible(true);

		/// ===Line=== ///
		line_ = std::make_unique<Line>();

		/// ===カメラの初期設定=== ///
		// デフォルトカメラの位置を設定
		// camera_->SetTranslate(cameraPosition_);
		// camera_->SetRotate(cameraRotation_);
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void ParticleEditorScene::Update() {
		/// ===カメラの更新=== ///
		//UpdateCamera();

		/// ===エディターの更新=== ///
		if (particleEditor_) {
			particleEditor_->Update();
		}

		/// ===ImGui描画=== ///
#ifdef USE_IMGUI
	//RenderMenuBar();
		defaultCamera_->ImGuiUpdate();

		// エディターメインウィンドウ
		particleEditor_->Render();

		// サブウィンドウ
		/*if (showHelp_) {
			RenderHelpWindow();
		}
		if (showStats_) {
			RenderStatsWindow();
		}*/

		//RenderCameraControl();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 描画
	///-------------------------------------------///
	void ParticleEditorScene::Draw() {
		/// ===グリッドの描画=== ///
		line_->DrawGrid({ 0.0f, 0.0f, 0.0f }, { 500.0f, 0.0f, 500.0f }, 100, { 1.0f, 1.0f, 1.0f, 1.0f });

		/// ===エディタープレビューの描画=== ///
		if (particleEditor_) {
			particleEditor_->DrawPreview();
		}
	}

	///-------------------------------------------/// 
	/// メニューバーの描画
	///-------------------------------------------///
	void ParticleEditorScene::RenderMenuBar() {
#ifdef USE_IMGUI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("ファイル")) {
				if (ImGui::MenuItem("新規作成", "Ctrl+N")) {
					// エディターの新規作成機能を呼び出し
				}
				if (ImGui::MenuItem("開く", "Ctrl+O")) {
					// 開く処理
				}
				if (ImGui::MenuItem("保存", "Ctrl+S")) {
					// 保存処理
				}
				ImGui::Separator();
				if (ImGui::MenuItem("終了", "Alt+F4")) {
					// シーン遷移などの終了処理
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("表示")) {
				bool editorVisible = particleEditor_ ? particleEditor_->IsVisible() : false;
				if (ImGui::MenuItem("エディター", nullptr, &editorVisible)) {
					if (particleEditor_) {
						particleEditor_->SetVisible(editorVisible);
					}
				}
				ImGui::MenuItem("統計情報", nullptr, &showStats_);
				ImGui::MenuItem("カメラコントロール", nullptr, nullptr);
				ImGui::MenuItem("グリッド", nullptr, &showGrid_);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("ヘルプ")) {
				if (ImGui::MenuItem("使い方")) {
					showHelp_ = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("バージョン情報")) {
					// バージョン情報表示
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// ヘルプウィンドウの描画
	///-------------------------------------------///
	void ParticleEditorScene::RenderHelpWindow() {
#ifdef USE_IMGUI
		ImGui::Begin("使い方", &showHelp_);

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "パーティクルエディターの使い方");
		ImGui::Separator();

		ImGui::Text("■ 基本操作");
		ImGui::BulletText("左側のタブで各種設定を調整できます");
		ImGui::BulletText("プレビューボタンで効果を確認できます");
		ImGui::BulletText("保存ボタンでJSONファイルに保存されます");

		ImGui::Spacing();
		ImGui::Text("■ タブの説明");
		ImGui::BulletText("基本設定: 名前、モデル、形状などの基本パラメータ");
		ImGui::BulletText("物理設定: 重力、速度、爆発半径などの物理挙動");
		ImGui::BulletText("見た目設定: 色、スケール、テクスチャなどの視覚効果");
		ImGui::BulletText("回転設定: パーティクルの回転に関する設定");
		ImGui::BulletText("発生設定: 寿命、発生レート、バーストなどの発生パターン");

		ImGui::Spacing();
		ImGui::Text("■ カメラ操作");
		ImGui::BulletText("WASDキー: カメラの移動");
		ImGui::BulletText("矢印キー: カメラの回転");
		ImGui::BulletText("カメラコントロールウィンドウで詳細設定可能");

		ImGui::Spacing();
		ImGui::Text("■ ショートカット");
		ImGui::BulletText("Ctrl+N: 新規作成");
		ImGui::BulletText("Ctrl+S: 保存");
		ImGui::BulletText("Ctrl+O: 開く");
		ImGui::BulletText("Space: プレビュー再生/停止");

		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::Button("閉じる", ImVec2(120, 30))) {
			showHelp_ = false;
		}

		ImGui::End();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 統計情報ウィンドウの描画
	///-------------------------------------------///
	void ParticleEditorScene::RenderStatsWindow() {
#ifdef USE_IMGUI
		ImGui::Begin("統計情報", &showStats_);

		// FPS表示
		float deltaTime = Service::DeltaTime::GetDeltaTime();
		float fps = 1.0f / deltaTime;
		ImGui::Text("FPS: %.1f", fps);
		ImGui::Text("デルタタイム: %.3f ms", deltaTime * 1000.0f);

		ImGui::Separator();

		// 登録されているパーティクル定義一覧
		ImGui::Text("登録済みパーティクル:");
		std::vector<std::string> names = Service::Particle::GetDefinitionNames();
		if (names.empty()) {
			ImGui::TextDisabled("（なし）");
		} else {
			for (const auto& name : names) {
				ImGui::BulletText("%s", name.c_str());
				ImGui::Indent();

				// アクティブなグループ数とパーティクル数
				size_t groupCount = Service::Particle::GetActiveGroupCount(name);
				uint32_t particleCount = Service::Particle::GetActiveParticleCount(name);

				ImGui::Text("グループ数: %zu", groupCount);
				ImGui::Text("パーティクル数: %u", particleCount);

				ImGui::Unindent();
			}
		}

		ImGui::Separator();

		// カメラ情報
		ImGui::Text("カメラ位置:");
		ImGui::Text("  X: %.2f, Y: %.2f, Z: %.2f",
			cameraPosition_.x, cameraPosition_.y, cameraPosition_.z);
		ImGui::Text("カメラ回転:");
		ImGui::Text("  X: %.2f, Y: %.2f, Z: %.2f",
			cameraRotation_.x, cameraRotation_.y, cameraRotation_.z);

		ImGui::End();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// カメラコントロールウィンドウの描画
	///-------------------------------------------///
	void ParticleEditorScene::RenderCameraControl() {
#ifdef USE_IMGUI
		ImGui::Begin("カメラコントロール");

		ImGui::Text("カメラ設定");
		ImGui::Separator();

		// 位置
		if (ImGui::DragFloat3("位置", &cameraPosition_.x, 0.1f)) {
			// カメラ位置を更新
			// camera_->SetTranslate(cameraPosition_);
		}

		// 回転
		if (ImGui::DragFloat3("回転", &cameraRotation_.x, 0.01f)) {
			// カメラ回転を更新
			// camera_->SetRotate(cameraRotation_);
		}

		ImGui::Spacing();

		// 速度設定
		ImGui::DragFloat("移動速度", &cameraSpeed_, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("回転速度", &cameraRotationSpeed_, 0.001f, 0.001f, 0.1f);

		ImGui::Spacing();
		ImGui::Separator();

		// リセットボタン
		if (ImGui::Button("カメラをリセット", ImVec2(150, 30))) {
			cameraPosition_ = { 0.0f, 5.0f, -10.0f };
			cameraRotation_ = { 0.3f, 0.0f, 0.0f };
			// camera_->SetTranslate(cameraPosition_);
			// camera_->SetRotate(cameraRotation_);
		}

		ImGui::Spacing();
		ImGui::TextDisabled("WASDキー: 移動");
		ImGui::TextDisabled("矢印キー: 回転");
		ImGui::TextDisabled("QE: 上下移動");

		ImGui::End();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// カメラの更新
	///-------------------------------------------///
	void ParticleEditorScene::UpdateCamera() {
		// TODO: 入力処理の実装
		// Input::IsPress()などを使用してカメラを操作

		/*
		// WASD移動
		if (Input::IsPress(Key::W)) {
			cameraPosition_.z += cameraSpeed_;
		}
		if (Input::IsPress(Key::S)) {
			cameraPosition_.z -= cameraSpeed_;
		}
		if (Input::IsPress(Key::A)) {
			cameraPosition_.x -= cameraSpeed_;
		}
		if (Input::IsPress(Key::D)) {
			cameraPosition_.x += cameraSpeed_;
		}

		// QE上下移動
		if (Input::IsPress(Key::Q)) {
			cameraPosition_.y -= cameraSpeed_;
		}
		if (Input::IsPress(Key::E)) {
			cameraPosition_.y += cameraSpeed_;
		}

		// 矢印キーで回転
		if (Input::IsPress(Key::Up)) {
			cameraRotation_.x += cameraRotationSpeed_;
		}
		if (Input::IsPress(Key::Down)) {
			cameraRotation_.x -= cameraRotationSpeed_;
		}
		if (Input::IsPress(Key::Left)) {
			cameraRotation_.y -= cameraRotationSpeed_;
		}
		if (Input::IsPress(Key::Right)) {
			cameraRotation_.y += cameraRotationSpeed_;
		}

		// カメラに反映
		camera_->SetTranslate(cameraPosition_);
		camera_->SetRotate(cameraRotation_);
		*/
	}
}