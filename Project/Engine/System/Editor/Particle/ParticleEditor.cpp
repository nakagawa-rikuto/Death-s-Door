#include "ParticleEditor.h"
// Particle
#include "Engine/Graphics/Particle/ParticleFactory.h"
#include "Engine/Graphics/Particle/ParticleGroup.h"
// Service
#include "Service/DeltaTime.h"

#ifdef USE_IMGUI
// ImGui
#include <imgui.h>
#endif // USE_IMGUI
// c++
#include <fstream>
#include <filesystem>

namespace MiiEngine {
	///-------------------------------------------/// 
	/// コンストラクタ・デストラクタ
	///-------------------------------------------///
	ParticleEditor::ParticleEditor() {
		// デフォルトのファイルパスを設定
		strcpy_s(filePathBuffer_, kDefaultSavePath);
	}

	ParticleEditor::~ParticleEditor() {
		previewParticle_.reset();
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void ParticleEditor::Initialize() {
		// 利用可能なリソースを更新
		UpdateAvailableModels();
		UpdateAvailableTextures();
		UpdateAvailablePresets();

		// デフォルトの定義を設定
		currentDefinition_ = ParticleDefinition();
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void ParticleEditor::Update() {
		if (!isVisible_) return;

		// プレビューパーティクルの更新
		if (isPlaying_ && previewParticle_) {
			// 軌跡プレビューモードの場合
			if (trajectoryPreviewMode_ && currentDefinition_.advanced.isTrajectoryParticle) {
				UpdateTrajectoryPreview();
			}

			previewParticle_->Update();

			// 終了したら自動リプレイ
			if (autoReplay_ && previewParticle_->IsFinish()) {
				ResetPreview();
				PlayPreview();
			}
		}
	}

	///-------------------------------------------/// 
	/// ImGui描画
	///-------------------------------------------///
	void ParticleEditor::Render() {
#ifdef USE_IMGUI
		if (!isVisible_) return;

		ImGui::Begin("パーティクルエディター", &isVisible_, ImGuiWindowFlags_MenuBar);

		// メニューバー
		RenderMenuBar();

		// タブで分割
		if (ImGui::BeginTabBar("ParticleEditorTabs")) {

			// 基本設定タブ
			if (ImGui::BeginTabItem("基本設定")) {
				RenderBasicSettings();
				ImGui::EndTabItem();
			}

			// 物理設定タブ
			if (ImGui::BeginTabItem("物理設定")) {
				RenderPhysicsSettings();
				ImGui::EndTabItem();
			}

			// 見た目設定タブ
			if (ImGui::BeginTabItem("見た目設定")) {
				RenderAppearanceSettings();
				ImGui::EndTabItem();
			}

			// 回転設定タブ
			if (ImGui::BeginTabItem("回転設定")) {
				RenderRotationSettings();
				ImGui::EndTabItem();
			}

			// 発生設定タブ
			if (ImGui::BeginTabItem("発生設定")) {
				RenderEmissionSettings();
				ImGui::EndTabItem();
			}

			// 高度設定タブ
			if (ImGui::BeginTabItem("動作設定")) {
				RenderAdvancedSettings();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::Separator();

		// プレビューコントロール
		RenderPreviewControls();

		// ファイル操作
		RenderFileOperations();

		ImGui::End();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プレビュー描画
	///-------------------------------------------///
	void ParticleEditor::DrawPreview() {
#ifdef USE_IMGUI
		if (isPlaying_ && previewParticle_) {
			previewParticle_->Draw(previewBlendMode_);
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 表示切り替え
	///-------------------------------------------///
	void ParticleEditor::ToggleVisibility() {
		isVisible_ = !isVisible_;
	}

	void ParticleEditor::SetVisible(bool visible) {
		isVisible_ = visible;
	}

	bool ParticleEditor::IsVisible() const {
		return isVisible_;
	}

	///-------------------------------------------/// 
	/// JSON保存
	///-------------------------------------------///
	void ParticleEditor::SaveToJson() {
#ifdef USE_IMGUI
		try {
			// 編集中の定義を保存
			nlohmann::json j = currentDefinition_.ToJson();

			std::string filepath = filePathBuffer_;
			if (filepath.find(kFileExtension) == std::string::npos) {
				filepath += kFileExtension;
			}

			std::ofstream file(filepath);
			file << j.dump(4); // インデント4でフォーマット
			file.close();

			currentFilePath_ = filepath;

			// プリセット一覧を更新
			UpdateAvailablePresets();

		} catch (const std::exception& e) {
			// エラーハンドリング（コンソールに出力など）
			e;
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// JSON読み込み
	///-------------------------------------------///
	void ParticleEditor::LoadFromJson() {
#ifdef USE_IMGUI
		try {
			std::string filepath = filePathBuffer_;
			if (filepath.find(kFileExtension) == std::string::npos) {
				filepath += kFileExtension;
			}

			std::ifstream file(filepath);
			nlohmann::json j;
			file >> j;
			file.close();

			currentDefinition_ = ParticleDefinition::FromJson(j);
			currentFilePath_ = filepath;

			// プレビューを更新
			if (isPlaying_) {
				ResetPreview();
				CreatePreviewParticle();
				PlayPreview();
			}

		} catch (const std::exception& e) {
			// エラーハンドリング
			e;
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 新規作成
	///-------------------------------------------///
	void ParticleEditor::CreateNew() {
#ifdef USE_IMGUI
		currentDefinition_ = ParticleDefinition();
		currentFilePath_.clear();
		strcpy_s(filePathBuffer_, kDefaultSavePath);

		if (isPlaying_) {
			StopPreview();
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プレビュー再生
	///-------------------------------------------///
	void ParticleEditor::PlayPreview() {
#ifdef USE_IMGUI
		if (!previewParticle_) {
			CreatePreviewParticle();
		}
		isPlaying_ = true;
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プレビュー停止
	///-------------------------------------------///
	void ParticleEditor::StopPreview() {
#ifdef USE_IMGUI
		isPlaying_ = false;
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プレビューリセット
	///-------------------------------------------///
	void ParticleEditor::ResetPreview() {
#ifdef USE_IMGUI
		previewParticle_.reset();
		isPlaying_ = false;
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プレビューパーティクル生成
	///-------------------------------------------///
	void ParticleEditor::CreatePreviewParticle() {
#ifdef USE_IMGUI
		// ParticleGroupを生成してDefinitionを適用
		previewParticle_ = std::make_unique<ParticleGroup>();
		previewParticle_->Initialize(previewPosition_, currentDefinition_);

		// ★テクスチャが指定されていれば明示的に設定（二重適用になるが安全のため）
		if (!currentDefinition_.appearance.texturePath.empty()) {
			previewParticle_->SetTexture(currentDefinition_.appearance.texturePath);
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// メニューバー描画
	///-------------------------------------------///
	void ParticleEditor::RenderMenuBar() {
#ifdef USE_IMGUI
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("ファイル")) {
				if (ImGui::MenuItem("新規作成", "Ctrl+N")) {
					CreateNew();
				}
				if (ImGui::MenuItem("開く", "Ctrl+O")) {
					showLoadDialog_ = true;
				}
				if (ImGui::MenuItem("保存", "Ctrl+S")) {
					SaveToJson();
				}
				if (ImGui::MenuItem("名前を付けて保存...")) {
					showSaveDialog_ = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("閉じる")) {
					isVisible_ = false;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("ヘルプ")) {
				if (ImGui::MenuItem("使い方")) {
					// TODO: ヘルプダイアログ表示
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 基本設定UI
	///-------------------------------------------///
	void ParticleEditor::RenderBasicSettings() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("基本設定");

		/// ===名前=== ///
		char nameBuffer[256];
		strcpy_s(nameBuffer, currentDefinition_.name.c_str());
		if (ImGui::InputText("パーティクル", nameBuffer, sizeof(nameBuffer))) {
			currentDefinition_.name = nameBuffer;
		}
		ImGui::TextDisabled("エミッター登録時に使用する識別名");
		ImGui::Spacing();

		/// ===モデル選択=== ///
		if (!availableModels_.empty()) {
			if (ImGui::Combo("モデル", &selectedModelIndex_,
				[](void* data, int idx, const char** out_text) {
					auto& models = *static_cast<std::vector<std::string>*>(data);
					*out_text = models[idx].c_str();
					return true;
				}, &availableModels_, static_cast<int>(availableModels_.size()))) {
				currentDefinition_.modelName = availableModels_[selectedModelIndex_];

				// モデル変更時はプレビューを再生成
				if (isPlaying_) {
					ResetPreview();
					CreatePreviewParticle();
					PlayPreview();
				}
			}
		} else {
			char modelBuffer[256];
			strcpy_s(modelBuffer, currentDefinition_.modelName.c_str());
			if (ImGui::InputText("モデル名", modelBuffer, sizeof(modelBuffer))) {
				currentDefinition_.modelName = modelBuffer;
			}
		}

		/// ===形状選択=== ///
		const char* shapes[] = { "円", "円柱", "なし" };
		int currentShape = static_cast<int>(currentDefinition_.shape);
		if (ImGui::Combo("形状タイプ", &currentShape, shapes, IM_ARRAYSIZE(shapes))) {
			currentDefinition_.shape = static_cast<shapeType>(currentShape);
		}
		ImGui::TextDisabled("カスタム頂点バッファを使用する形状");
		ImGui::Spacing();

		/// ===最大インスタンス数=== ///
		int maxInstance = static_cast<int>(currentDefinition_.maxInstance);
		if (ImGui::DragInt("最大パーティクル数", &maxInstance, 1, 1, 10000)) {
			currentDefinition_.maxInstance = static_cast<uint32_t>(maxInstance);

			// 最大数変更時もプレビューを再生成
			if (isPlaying_) {
				ResetPreview();
				CreatePreviewParticle();
				PlayPreview();
			}
		}
		ImGui::TextDisabled("同時に表示できる最大数");
		ImGui::Spacing();
		ImGui::Separator();

		/// ===プレビュー位置設定=== ///
		ImGui::SeparatorText("プレビュー設定");

		bool positionChanged = false;

		/// ===XYZ個別スライダー=== ///
		positionChanged |= ImGui::DragFloat("発生位置 X", &previewPosition_.x, 0.1f, -100.0f, 100.0f);
		positionChanged |= ImGui::DragFloat("発生位置 Y", &previewPosition_.y, 0.1f, -100.0f, 100.0f);
		positionChanged |= ImGui::DragFloat("発生位置 Z", &previewPosition_.z, 0.1f, -100.0f, 100.0f);

		/// ===まとめてリセット=== ///
		if (ImGui::Button("位置をリセット", ImVec2(150, 25))) {
			previewPosition_ = { 0.0f, 0.0f, 0.0f };
			positionChanged = true;
		}
		ImGui::SameLine();

		// 位置変更時にプレビューパーティクルの位置も更新
		if (positionChanged && previewParticle_) {
			previewParticle_->SetEmitterPosition(previewPosition_);
		}

		ImGui::Spacing();
		ImGui::TextDisabled("Tips: プレビュー再生中でも位置を変更できます");
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 物理設定UI
	///-------------------------------------------///
	void ParticleEditor::RenderPhysicsSettings() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("物理設定");

		/// ===速度設定=== ///
		if (ImGui::Checkbox("ランダム速度を使用", &currentDefinition_.physics.useRandomVelocity)) {
			// チェックボックス変更時にプレビューパーティクルに適用
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}
		ImGui::TextDisabled("オフの場合はランダム速度になります");
		ImGui::Spacing();

		bool velocityChanged = false;

		/// ===速度の設定=== ///
		if (currentDefinition_.physics.useRandomVelocity) {
			ImGui::Text("速度範囲");
			velocityChanged |= ImGui::DragFloat3("最小速度", &currentDefinition_.physics.velocityMin.x, 0.1f, -100.0f, 100.0f);
			velocityChanged |= ImGui::DragFloat3("最大速度", &currentDefinition_.physics.velocityMax.x, 0.1f, -100.0f, 100.0f);
		} else {
			velocityChanged |= ImGui::DragFloat3("初期速度", &currentDefinition_.physics.velocityMin.x, 0.1f, -100.0f, 100.0f);
		}

		// 速度変更時にプレビューに適用
		if (velocityChanged && previewParticle_) {
			previewParticle_->SetParameter(ParticleParameter::VelocityMinX, currentDefinition_.physics.velocityMin.x);
			previewParticle_->SetParameter(ParticleParameter::VelocityMinY, currentDefinition_.physics.velocityMin.y);
			previewParticle_->SetParameter(ParticleParameter::VelocityMinZ, currentDefinition_.physics.velocityMin.z);
			previewParticle_->SetParameter(ParticleParameter::VelocityMaxX, currentDefinition_.physics.velocityMax.x);
			previewParticle_->SetParameter(ParticleParameter::VelocityMaxY, currentDefinition_.physics.velocityMax.y);
			previewParticle_->SetParameter(ParticleParameter::VelocityMaxZ, currentDefinition_.physics.velocityMax.z);
		}
		ImGui::Spacing();
		ImGui::Separator();

		/// ===加速度の設定=== ///
		ImGui::Text("加速度");
		if (ImGui::DragFloat3("加速度ベクトル", &currentDefinition_.physics.acceleration.x, 0.1f, -50.0f, 50.0f)) {
			if (previewParticle_) {
				previewParticle_->SetParameter(ParticleParameter::AccelerationX, currentDefinition_.physics.acceleration.x);
				previewParticle_->SetParameter(ParticleParameter::AccelerationY, currentDefinition_.physics.acceleration.y);
				previewParticle_->SetParameter(ParticleParameter::AccelerationZ, currentDefinition_.physics.acceleration.z);
			}
		}
		ImGui::TextDisabled("毎フレーム速度に加算される量");
		ImGui::Spacing();

		/// ===重力の設定=== ///
		if (ImGui::DragFloat("重力", &currentDefinition_.physics.gravity, 0.1f, -50.0f, 50.0f)) {
			if (previewParticle_) {
				previewParticle_->SetParameter(ParticleParameter::Gravity, currentDefinition_.physics.gravity);
			}
		}
		ImGui::TextDisabled("Y軸方向の加速度");

		if (ImGui::DragFloat("上方向の初期加速", &currentDefinition_.physics.upwardForce, 0.1f, 0.0f, 50.0f)) {
			if (previewParticle_) {
				previewParticle_->SetParameter(ParticleParameter::UpwardForce, currentDefinition_.physics.upwardForce);
			}
		}
		ImGui::TextDisabled("発生時にY軸速度に加算される値");
		ImGui::Spacing();
		ImGui::Separator();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 見た目設定UI
	///-------------------------------------------///
	void ParticleEditor::RenderAppearanceSettings() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("見た目設定");

		/// ===色設定=== ///
		ImGui::Text("色設定");
		if (ImGui::Checkbox("色のグラデーション", &currentDefinition_.appearance.useColorGradient)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}
		if (ImGui::ColorEdit4("開始色", &currentDefinition_.appearance.startColor.x)) {
			if (previewParticle_) {
				previewParticle_->SetParameter(ParticleParameter::StartColorR, currentDefinition_.appearance.startColor.x);
				previewParticle_->SetParameter(ParticleParameter::StartColorG, currentDefinition_.appearance.startColor.y);
				previewParticle_->SetParameter(ParticleParameter::StartColorB, currentDefinition_.appearance.startColor.z);
				previewParticle_->SetParameter(ParticleParameter::StartColorA, currentDefinition_.appearance.startColor.w);
			}
		}
		if (currentDefinition_.appearance.useColorGradient) {
			if (ImGui::ColorEdit4("終了色", &currentDefinition_.appearance.endColor.x)) {
				if (previewParticle_) {
					previewParticle_->SetParameter(ParticleParameter::EndColorR, currentDefinition_.appearance.endColor.x);
					previewParticle_->SetParameter(ParticleParameter::EndColorG, currentDefinition_.appearance.endColor.y);
					previewParticle_->SetParameter(ParticleParameter::EndColorB, currentDefinition_.appearance.endColor.z);
					previewParticle_->SetParameter(ParticleParameter::EndColorA, currentDefinition_.appearance.endColor.w);
				}
			}
			ImGui::TextDisabled("寿命に応じて開始色→終了色へ変化");
		}
		ImGui::Spacing();
		ImGui::Separator();

		/// ===スケール設定=== ///
		ImGui::Text("スケール設定");
		if (ImGui::Checkbox("スケールアニメーション", &currentDefinition_.appearance.useScaleAnimation)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}

		ImGui::Text("開始スケール範囲");
		bool scaleChanged = false;
		scaleChanged |= ImGui::DragFloat3("最小スケール", &currentDefinition_.appearance.startScaleMin.x, 0.01f, 0.0f, 10.0f);
		scaleChanged |= ImGui::DragFloat3("最大スケール", &currentDefinition_.appearance.startScaleMax.x, 0.01f, 0.0f, 10.0f);
		if (currentDefinition_.appearance.useScaleAnimation) {
			scaleChanged |= ImGui::DragFloat3("終了スケール", &currentDefinition_.appearance.endScale.x, 0.01f, 0.0f, 10.0f);
			ImGui::TextDisabled("寿命に応じて縮小・拡大します");
		}
		if (scaleChanged && previewParticle_) {
			previewParticle_->SetDefinition(currentDefinition_);
		}
		ImGui::Spacing();
		ImGui::Separator();

		/// ===テクスチャ設定=== ///
		ImGui::Text("テクスチャ");
		if (!availableTextures_.empty()) {
			if (ImGui::Combo("テクスチャファイル", &selectedTextureIndex_,
				[](void* data, int idx, const char** out_text) {
					auto& textures = *static_cast<std::vector<std::string>*>(data);
					*out_text = textures[idx].c_str();
					return true;
				}, &availableTextures_, static_cast<int>(availableTextures_.size()))) {
				currentDefinition_.appearance.texturePath = availableTextures_[selectedTextureIndex_];

				if (previewParticle_) {
					previewParticle_->SetTexture(currentDefinition_.appearance.texturePath);
				}
			}
		} else {
			char textureBuffer[256];
			strcpy_s(textureBuffer, currentDefinition_.appearance.texturePath.c_str());
			if (ImGui::InputText("テクスチャパス", textureBuffer, sizeof(textureBuffer))) {
				currentDefinition_.appearance.texturePath = textureBuffer;

				if (previewParticle_) {
					previewParticle_->SetTexture(currentDefinition_.appearance.texturePath);
				}
			}
		}

		if (!currentDefinition_.appearance.texturePath.empty()) {
			ImGui::TextDisabled("現在: %s", currentDefinition_.appearance.texturePath.c_str());
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 回転設定UI
	///-------------------------------------------///
	void ParticleEditor::RenderRotationSettings() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("回転設定");

		/// ===回転の設定=== ///
		if (ImGui::Checkbox("回転を有効化", &currentDefinition_.rotation.enableRotation)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}

		if (currentDefinition_.rotation.enableRotation) {
			ImGui::Spacing();

			// ===初期回転設定=== ///
			ImGui::SeparatorText("初期回転");

			if (ImGui::Checkbox("初期回転をランダム化", &currentDefinition_.rotation.randomInitialRotation)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}

			bool initialRotationChanged = false;
			if (currentDefinition_.rotation.randomInitialRotation) {
				ImGui::Text("初期回転範囲（ラジアン）");
				initialRotationChanged |= ImGui::DragFloat3("最小初期回転", &currentDefinition_.rotation.initialRotationMin.x, 0.01f, -6.28f, 6.28f);
				initialRotationChanged |= ImGui::DragFloat3("最大初期回転", &currentDefinition_.rotation.initialRotationMax.x, 0.01f, -6.28f, 6.28f);
			} else {
				ImGui::Text("固定初期回転（ラジアン）");
				initialRotationChanged |= ImGui::DragFloat3("初期回転", &currentDefinition_.rotation.initialRotationMin.x, 0.01f, -6.28f, 6.28f);
			}

			if (initialRotationChanged && previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}

			ImGui::Spacing();
			ImGui::Separator();

			/// ===回転速度設定=== ///
			ImGui::SeparatorText("回転速度");

			if (ImGui::Checkbox("ランダム回転速度", &currentDefinition_.rotation.randomRotation)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			ImGui::TextDisabled("オフの場合は固定速度で回転");

			ImGui::Spacing();

			bool rotationChanged = false;

			if (currentDefinition_.rotation.randomRotation) {
				ImGui::Text("回転速度範囲（ラジアン/秒)");
				rotationChanged |= ImGui::DragFloat3("最小回転速度", &currentDefinition_.rotation.rotationSpeedMin.x, 0.1f, -10.0f, 10.0f);
				rotationChanged |= ImGui::DragFloat3("最大回転速度", &currentDefinition_.rotation.rotationSpeedMax.x, 0.1f, -10.0f, 10.0f);
			} else {
				ImGui::Text("固定回転速度（ラジアン/秒)");
				rotationChanged |= ImGui::DragFloat3("回転速度", &currentDefinition_.rotation.rotationSpeedMin.x, 0.1f, -10.0f, 10.0f);
			}

			if (rotationChanged && previewParticle_) {
				previewParticle_->SetParameter(ParticleParameter::RotationSpeedX, currentDefinition_.rotation.rotationSpeedMin.x);
				previewParticle_->SetParameter(ParticleParameter::RotationSpeedY, currentDefinition_.rotation.rotationSpeedMin.y);
				previewParticle_->SetParameter(ParticleParameter::RotationSpeedZ, currentDefinition_.rotation.rotationSpeedMin.z);
			}

			ImGui::Spacing();
		} else {
			ImGui::Spacing();
			ImGui::TextDisabled("回転が無効化されています");
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 発生設定UI
	///-------------------------------------------///
	void ParticleEditor::RenderEmissionSettings() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("発生設定");

		/// ===寿命の設定=== ///
		ImGui::Text("パーティクル寿命 (秒)");
		bool lifetimeChanged = false;
		lifetimeChanged |= ImGui::DragFloat("最小寿命", &currentDefinition_.emission.lifetimeMin, 0.1f, 0.1f, 100.0f);
		lifetimeChanged |= ImGui::DragFloat("最大寿命", &currentDefinition_.emission.lifetimeMax, 0.1f, 0.1f, 100.0f);
		ImGui::TextDisabled("各パーティクルの寿命はこの範囲でランダム");

		if (lifetimeChanged && previewParticle_) {
			previewParticle_->SetParameter(ParticleParameter::LifetimeMin, currentDefinition_.emission.lifetimeMin);
			previewParticle_->SetParameter(ParticleParameter::LifetimeMax, currentDefinition_.emission.lifetimeMax);
		}

		ImGui::Spacing();
		ImGui::Separator();

		/// ===発生範囲の設定=== ///
		ImGui::Text("発生範囲");
		bool rangeChanged = false;
		rangeChanged |= ImGui::DragFloat("X軸範囲", &currentDefinition_.physics.explosionRange.x, 0.1f, 0.0f, 20.0f);
		rangeChanged |= ImGui::DragFloat("Y軸範囲", &currentDefinition_.physics.explosionRange.y, 0.1f, 0.0f, 20.0f);
		rangeChanged |= ImGui::DragFloat("Z軸範囲", &currentDefinition_.physics.explosionRange.z, 0.1f, 0.0f, 20.0f);
		ImGui::TextDisabled("各軸方向の発生範囲（±指定値の範囲内）");

		if (rangeChanged && previewParticle_) {
			previewParticle_->SetDefinition(currentDefinition_);
		}

		ImGui::Spacing();
		ImGui::Separator();

		/// ===発生モード=== ///
		ImGui::Text("発生モード");
		if (ImGui::Checkbox("一度に大量", &currentDefinition_.emission.isBurst)) {
			if (previewParticle_) {
				// バーストモード切り替え時は再生成
				ResetPreview();
				CreatePreviewParticle();
				if (isPlaying_) {
					PlayPreview();
				}
			}
		}
		ImGui::TextDisabled("一度に大量発生するか、継続的に発生するか");
		ImGui::Spacing();

		if (currentDefinition_.emission.isBurst) {
			// バーストモード設定
			int burstCount = static_cast<int>(currentDefinition_.emission.burstCount);
			if (ImGui::DragInt("パーティクル数", &burstCount, 1, 1, 10000)) {
				currentDefinition_.emission.burstCount = static_cast<uint32_t>(burstCount);

				if (previewParticle_) {
					previewParticle_->SetParameter(ParticleParameter::BurstCount, static_cast<float>(currentDefinition_.emission.burstCount));
				}
			}
			ImGui::TextDisabled("一度に発生するパーティクルの数");
		} else {
			// 連続発生モード設定
			bool emissionChanged = false;
			emissionChanged |= ImGui::DragFloat("発生レート（個/秒）", &currentDefinition_.emission.emissionRate, 0.1f, 0.1f, 1000.0f);
			ImGui::TextDisabled("1秒あたりに発生するパーティクル数");

			emissionChanged |= ImGui::DragFloat("発生頻度（秒）", &currentDefinition_.emission.frequency, 0.01f, 0.01f, 10.0f);
			ImGui::TextDisabled("パーティクルを発生させる間隔");

			if (emissionChanged && previewParticle_) {
				previewParticle_->SetParameter(ParticleParameter::EmissionRate, currentDefinition_.emission.emissionRate);
				previewParticle_->SetParameter(ParticleParameter::Frequency, currentDefinition_.emission.frequency);
			}

			ImGui::Spacing();
			ImGui::Text("実際の発生数");
			uint32_t actualCount = static_cast<uint32_t>(currentDefinition_.emission.emissionRate * currentDefinition_.emission.frequency);
			ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%u 個ずつ発生", actualCount);
		}

		ImGui::TextDisabled("1回の発生で生成される粒子数");
		ImGui::Spacing();
		ImGui::Separator();
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 高度設定UI
	///-------------------------------------------///
	void ParticleEditor::RenderAdvancedSettings() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("動作設定");

		// ===軌跡パーティクル設定=== ///
		ImGui::SeparatorText("軌跡パーティクル");
		if (ImGui::Checkbox("軌跡パーティクルモード", &currentDefinition_.advanced.isTrajectoryParticle)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
			// 軌跡モードをオフにした場合、プレビューモードも自動でオフ
			if (!currentDefinition_.advanced.isTrajectoryParticle) {
				trajectoryPreviewMode_ = false;
			}
		}
		ImGui::TextDisabled("攻撃の軌道などに使用する連続発生モード");

		if (currentDefinition_.advanced.isTrajectoryParticle) {
			ImGui::Indent();

			// 軌跡間隔設定
			if (ImGui::DragFloat("軌跡間隔 (秒)", &currentDefinition_.advanced.trailSpacing, 0.001f, 0.001f, 0.1f, "%.3f")) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			ImGui::TextDisabled("パーティクルを発生させる間隔");

			ImGui::Checkbox("停止時にクリア", &currentDefinition_.advanced.clearOnStop);
			ImGui::TextDisabled("発生停止時に既存のパーティクルをクリア");

			ImGui::Spacing();
			ImGui::Separator();

			// 軌跡プレビュー設定
			ImGui::Text("プレビュー設定");
			ImGui::Checkbox("軌跡プレビューモード", &trajectoryPreviewMode_);
			ImGui::TextDisabled("エミッタを自動で移動させて軌跡を確認");

			if (trajectoryPreviewMode_) {
				ImGui::Spacing();
				ImGui::Text("軌跡経路設定");
				ImGui::DragFloat3("開始位置", &trajectoryStartPos_.x, 0.1f, -20.0f, 20.0f);
				ImGui::DragFloat3("終了位置", &trajectoryEndPos_.x, 0.1f, -20.0f, 20.0f);
				ImGui::DragFloat("移動速度", &trajectorySpeed_, 0.1f, 0.1f, 5.0f);
				ImGui::DragFloat3("基本回転", &trajectoryRotation_.x, 0.1f, -6.28f, 6.28f);

				ImGui::Spacing();

				// 進行状況表示
				ImGui::ProgressBar(trajectoryProgress_, ImVec2(-1, 0));
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
					"進行度: %.1f%% | 位置: (%.1f, %.1f, %.1f)",
					trajectoryProgress_ * 100.0f,
					previewPosition_.x,
					previewPosition_.y,
					previewPosition_.z
				);
			}

			ImGui::Unindent();
		}
		ImGui::Spacing();
		ImGui::Separator();

		// ===渦巻=== ///
		ImGui::SeparatorText("渦巻動作");
		auto& motion = currentDefinition_.advanced.motion;

		/// ===渦巻き運動=== ///
		if (ImGui::Checkbox("渦巻き運動", &motion.enableSwirling)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}
		// 渦巻運動パラメータの設定
		if (motion.enableSwirling) {
			ImGui::Indent();
			// 渦巻き速度
			if (ImGui::DragFloat("渦巻き速度", &motion.swirlingSpeed, 0.1f, 0.0f, 20.0f)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			// 拡散速度
			if (ImGui::DragFloat("拡散速度", &motion.expansionRate, 0.1f, 0.0f, 5.0f)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			ImGui::Unindent();
		}
		ImGui::Spacing();

		// エミッタ追従
		if (ImGui::Checkbox("エミッタに追従", &motion.followEmitter)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}
		// 追従パラメータの設定
		ImGui::TextDisabled("エミッタの移動に追従する");
		if (motion.followEmitter) {
			ImGui::Indent();
			// 追従強度
			if (ImGui::SliderFloat("追従強度", &motion.followStrength, 0.0f, 1.0f)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			ImGui::TextDisabled("0.0 = 追従しない、1.0 = 完全追従");
			ImGui::Unindent();
		}
		ImGui::Spacing();

		// 回転影響
		if (ImGui::Checkbox("回転影響を受ける", &motion.useRotationInfluence)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}
		ImGui::TextDisabled("エミッタの回転がパーティクルに影響");
		// 回転影響パラメータの設定
		if (motion.useRotationInfluence) {
			ImGui::Indent();
			// 回転影響係数
			if (ImGui::DragFloat("回転影響係数", &motion.rotationInfluence, 0.1f, 0.0f, 5.0f)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			ImGui::Unindent();
		}
		ImGui::Spacing();

		// 速度減衰
		if (ImGui::SliderFloat("速度減衰率", &motion.velocityDamping, 0.0f, 1.0f)) {
			if (previewParticle_) {
				previewParticle_->SetDefinition(currentDefinition_);
			}
		}
		ImGui::TextDisabled("毎フレーム速度に掛ける係数 (1.0 = 減衰なし)");
		ImGui::Spacing();

		// ビルボード回転
		ImGui::Checkbox("ビルボード回転", &motion.enableBillboardRotation);
		// ビルボード回転パラメータの設定
		if (motion.enableBillboardRotation) {
			ImGui::Indent();
			// 回転速度
			if (ImGui::DragFloat("回転速度", &motion.billboardRotationSpeed, 0.1f, 0.0f, 10.0f)) {
				if (previewParticle_) {
					previewParticle_->SetDefinition(currentDefinition_);
				}
			}
			ImGui::Unindent();
		}
		ImGui::Spacing();
		ImGui::Separator();

		// プリセットボタン
		ImGui::SeparatorText("プリセット");
		if (ImGui::Button("攻撃軌跡プリセット", ImVec2(200, 30))) {
			ApplyTrajectoryPreset();
		}
		ImGui::SameLine();
		if (ImGui::Button("爆発プリセット", ImVec2(200, 30))) {
			ApplyExplosionPreset();
		}
		ImGui::SameLine();
		if (ImGui::Button("煙プリセット", ImVec2(200, 30))) {
			ApplySmokePreset();
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プレビューコントロールUI
	///-------------------------------------------///
	void ParticleEditor::RenderPreviewControls() {
#ifdef USE_IMGUI
		ImGui::SeparatorText("プレビュー");

		// 再生/停止ボタン
		if (isPlaying_) {
			if (ImGui::Button("停止", ImVec2(100, 30))) {
				StopPreview();
			}
		} else {
			if (ImGui::Button("再生", ImVec2(100, 30))) {
				PlayPreview();
			}
		}

		ImGui::SameLine();

		// リセットボタン
		if (ImGui::Button("リセット", ImVec2(100, 30))) {
			ResetPreview();
		}

		ImGui::SameLine();

		// 自動リプレイチェックボックス
		ImGui::Checkbox("自動リプレイ", &autoReplay_);
		ImGui::Spacing();
		ImGui::Separator();

		// ブレンドモード選択
		ImGui::Text("ブレンドモード");
		const char* blendModes[] = {
			"なし",
			"通常",
			"加算",
			"減算",
			"乗算",
			"スクリーン"
		};
		int currentBlendMode = static_cast<int>(previewBlendMode_);
		if (ImGui::Combo("##BlendMode", &currentBlendMode, blendModes, IM_ARRAYSIZE(blendModes))) {
			previewBlendMode_ = static_cast<BlendMode>(currentBlendMode);
		}
		ImGui::TextDisabled("プレビュー描画時の合成モード");

		// プレビュー状態表示
		if (isPlaying_ && previewParticle_) {
			ImGui::Spacing();
			ImGui::Text("アクティブパーティクル数: %u", previewParticle_->GetActiveParticleCount());
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// ファイル操作UI
	///-------------------------------------------///
	void ParticleEditor::RenderFileOperations() {
#ifdef USE_IMGUI
		ImGui::Separator();
		ImGui::SeparatorText("ファイル操作");

		// ファイルパス入力
		ImGui::InputText("保存先パス", filePathBuffer_, sizeof(filePathBuffer_));
		ImGui::TextDisabled(".json拡張子は自動で付加されます");

		ImGui::Spacing();

		// 保存・読み込みボタン
		if (ImGui::Button("保存", ImVec2(120, 30))) {
			SaveToJson();
		}

		ImGui::SameLine();

		if (ImGui::Button("読み込み", ImVec2(120, 30))) {
			LoadFromJson();
		}

		// 現在のファイルパスを表示
		if (!currentFilePath_.empty()) {
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "現在のファイル:");
			ImGui::TextWrapped("%s", currentFilePath_.c_str());
		}

		// プリセット選択
		if (!availablePresets_.empty()) {
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("プリセット読み込み:");
			if (ImGui::Combo("##Presets", &selectedPresetIndex_,
				[](void* data, int idx, const char** out_text) {
					auto& presets = *static_cast<std::vector<std::string>*>(data);
					*out_text = presets[idx].c_str();
					return true;
				}, &availablePresets_, static_cast<int>(availablePresets_.size()))) {
				strcpy_s(filePathBuffer_, availablePresets_[selectedPresetIndex_].c_str());
				LoadFromJson();
			}
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 利用可能なモデル更新
	///-------------------------------------------///
	void ParticleEditor::UpdateAvailableModels() {
#ifdef USE_IMGUI
		availableModels_.clear();
		// 実際の実装では、Assets/Modelsディレクトリをスキャンする
		availableModels_.push_back("plane");
		availableModels_.push_back("sphere");
		availableModels_.push_back("cube");
		availableModels_.push_back("triangle");
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 利用可能なテクスチャ更新
	///-------------------------------------------///
	void ParticleEditor::UpdateAvailableTextures() {
#ifdef USE_IMGUI
		availableTextures_.clear();

		// テクスチャの種類
		availableTextures_.push_back("circle");
		availableTextures_.push_back("circle2");
		availableTextures_.push_back("gradationLine");
		availableTextures_.push_back("Spark");
		availableTextures_.push_back("FlameEye");
		availableTextures_.push_back("Fire");

#endif // USE_IMGUI   
	}

	///-------------------------------------------/// 
	/// 利用可能なプリセット更新
	///-------------------------------------------///
	void ParticleEditor::UpdateAvailablePresets() {
#ifdef USE_IMGUI
		availablePresets_.clear();

		// Assets/Particlesディレクトリをスキャン
		std::string path = kDefaultSavePath;
		if (std::filesystem::exists(path)) {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				if (entry.path().extension() == kFileExtension) {
					availablePresets_.push_back(entry.path().string());
				}
			}
		}
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// 軌跡プレビューの更新
	///-------------------------------------------///
	void ParticleEditor::UpdateTrajectoryPreview() {
#ifdef USE_IMGUI
		float deltaTime = Service::DeltaTime::GetDeltaTime();

		// 進行度を更新
		trajectoryProgress_ += deltaTime * trajectorySpeed_;

		// ループ
		if (trajectoryProgress_ >= 1.0f) {
			trajectoryProgress_ = 0.0f;
		}

		// 現在位置を補間（線形補間）
		Vector3 currentPos;
		currentPos.x = trajectoryStartPos_.x + (trajectoryEndPos_.x - trajectoryStartPos_.x) * trajectoryProgress_;
		currentPos.y = trajectoryStartPos_.y + (trajectoryEndPos_.y - trajectoryStartPos_.y) * trajectoryProgress_;
		currentPos.z = trajectoryStartPos_.z + (trajectoryEndPos_.z - trajectoryStartPos_.z) * trajectoryProgress_;

		// 回転も補間（オプション：螺旋運動など）
		Vector3 currentRot = trajectoryRotation_;
		currentRot.y += trajectoryProgress_ * 3.14159f * 2.0f; // 1周回転

		// エミッタ位置と回転を更新
		previewParticle_->SetEmitterPosition(currentPos);
		previewParticle_->SetEmitterRotate(currentRot);
#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// プリセット適用関数
	///-------------------------------------------///
	void ParticleEditor::ApplyTrajectoryPreset() {
		// 攻撃軌跡用のプリセット
		currentDefinition_.advanced.isTrajectoryParticle = true;
		currentDefinition_.advanced.trailSpacing = 0.008f;
		currentDefinition_.advanced.clearOnStop = true;

		currentDefinition_.advanced.emissionPattern.pattern = ParticleEmissionPattern::Pattern::Sphere;
		currentDefinition_.advanced.emissionPattern.patternRadius = 0.25f;
		currentDefinition_.advanced.emissionPattern.particlesPerEmit = 6;

		currentDefinition_.advanced.motion.enableSwirling = true;
		currentDefinition_.advanced.motion.swirlingSpeed = 3.0f;
		currentDefinition_.advanced.motion.expansionRate = 0.8f;
		currentDefinition_.advanced.motion.useRotationInfluence = true;
		currentDefinition_.advanced.motion.rotationInfluence = 1.2f;
		currentDefinition_.advanced.motion.velocityDamping = 0.95f;
		currentDefinition_.advanced.motion.enableBillboardRotation = true;
		currentDefinition_.advanced.motion.billboardRotationSpeed = 3.0f;

		currentDefinition_.advanced.colorGradient.useGradient = true;
		currentDefinition_.advanced.colorGradient.primaryColor = { 0.2f, 0.8f, 1.0f, 1.0f };
		currentDefinition_.advanced.colorGradient.secondaryColor = { 1.0f, 0.9f, 0.3f, 1.0f };
		currentDefinition_.advanced.colorGradient.oscillateColor = true;

		currentDefinition_.advanced.scaleAnimation.type = ParticleScaleAnimation::AnimationType::Bounce;
		currentDefinition_.advanced.scaleAnimation.startScale = 0.3f;
		currentDefinition_.advanced.scaleAnimation.maxScale = 1.5f;
		currentDefinition_.advanced.scaleAnimation.endScale = 0.1f;

		currentDefinition_.emission.lifetimeMin = 0.4f;
		currentDefinition_.emission.lifetimeMax = 0.8f;

		if (previewParticle_) {
			ResetPreview();
			CreatePreviewParticle();
			PlayPreview();
		}
	}
	void ParticleEditor::ApplyExplosionPreset() {
		// 爆発用のプリセット
		currentDefinition_.advanced.isTrajectoryParticle = false;
		currentDefinition_.advanced.emissionPattern.pattern = ParticleEmissionPattern::Pattern::Burst;
		currentDefinition_.advanced.emissionPattern.patternRadius = 2.0f;
		currentDefinition_.advanced.emissionPattern.particlesPerEmit = 30;

		currentDefinition_.advanced.motion.enableSwirling = false;
		currentDefinition_.advanced.motion.velocityDamping = 0.92f;

		currentDefinition_.advanced.colorGradient.useGradient = true;
		currentDefinition_.advanced.colorGradient.primaryColor = { 1.0f, 0.5f, 0.0f, 1.0f };
		currentDefinition_.advanced.colorGradient.secondaryColor = { 1.0f, 0.0f, 0.0f, 1.0f };

		currentDefinition_.advanced.scaleAnimation.type = ParticleScaleAnimation::AnimationType::EaseOut;
		currentDefinition_.emission.isBurst = true;
		currentDefinition_.emission.burstCount = 50;

		if (previewParticle_) {
			ResetPreview();
			CreatePreviewParticle();
			PlayPreview();
		}
	}
	void ParticleEditor::ApplyMagicCirclePreset() {
		// 魔法陣用のプリセット
		currentDefinition_.advanced.emissionPattern.pattern = ParticleEmissionPattern::Pattern::Ring;
		currentDefinition_.advanced.emissionPattern.patternRadius = 3.0f;
		currentDefinition_.advanced.emissionPattern.particlesPerEmit = 12;

		currentDefinition_.advanced.motion.enableSwirling = true;
		currentDefinition_.advanced.motion.swirlingSpeed = 2.0f;
		currentDefinition_.advanced.motion.followEmitter = true;
		currentDefinition_.advanced.motion.followStrength = 0.8f;

		currentDefinition_.advanced.colorGradient.useGradient = true;
		currentDefinition_.advanced.colorGradient.primaryColor = { 0.5f, 0.2f, 1.0f, 1.0f };
		currentDefinition_.advanced.colorGradient.secondaryColor = { 0.8f, 0.8f, 1.0f, 1.0f };
		currentDefinition_.advanced.colorGradient.oscillateColor = true;

		currentDefinition_.advanced.scaleAnimation.type = ParticleScaleAnimation::AnimationType::Pulse;
		currentDefinition_.emission.isBurst = false;
		currentDefinition_.emission.emissionRate = 20.0f;
		currentDefinition_.emission.frequency = 0.05f;

		if (previewParticle_) {
			ResetPreview();
			CreatePreviewParticle();
			PlayPreview();
		}
	}
	void ParticleEditor::ApplySmokePreset() {
		// 煙用のプリセット
		currentDefinition_.advanced.emissionPattern.pattern = ParticleEmissionPattern::Pattern::Cone;
		currentDefinition_.advanced.emissionPattern.patternRadius = 0.5f;
		currentDefinition_.advanced.emissionPattern.patternAngle = 30.0f;
		currentDefinition_.advanced.emissionPattern.particlesPerEmit = 3;

		currentDefinition_.advanced.motion.enableSwirling = true;
		currentDefinition_.advanced.motion.swirlingSpeed = 0.5f;
		currentDefinition_.advanced.motion.expansionRate = 1.5f;
		currentDefinition_.advanced.motion.velocityDamping = 0.98f;

		currentDefinition_.advanced.colorGradient.useGradient = true;
		currentDefinition_.advanced.colorGradient.primaryColor = { 0.3f, 0.3f, 0.3f, 0.8f };
		currentDefinition_.advanced.colorGradient.secondaryColor = { 0.6f, 0.6f, 0.6f, 0.2f };

		currentDefinition_.advanced.scaleAnimation.type = ParticleScaleAnimation::AnimationType::Linear;
		currentDefinition_.advanced.scaleAnimation.startScale = 0.5f;
		currentDefinition_.advanced.scaleAnimation.endScale = 3.0f;

		currentDefinition_.emission.lifetimeMin = 2.0f;
		currentDefinition_.emission.lifetimeMax = 3.0f;
		currentDefinition_.physics.upwardForce = 2.0f;

		if (previewParticle_) {
			ResetPreview();
			CreatePreviewParticle();
			PlayPreview();
		}
	}
}