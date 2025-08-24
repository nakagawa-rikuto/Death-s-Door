#include "DebugScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/AudioService.h"
#include "Engine/System/Service/OffScreenService.h"
#include "Engine/System/Service/CameraService.h"
#include "Engine/System/Service/ParticleService.h"
#include "Engine/System/Service/ColliderService.h"
#include "Engine/System/Service/Loader.h"
// Particle
#include "Engine/Graphics/Particle/Derivative/ConfettiParticle.h"
#include "Engine/Graphics/Particle/Derivative/ExplosionParticle.h"
#include "Engine/Graphics/Particle/Derivative/WindParticle.h"
#include "Engine/Graphics/Particle/Derivative/HitEffectParticle.h"
#include "Engine/Graphics/Particle/Derivative/RingParticle.h"
#include "Engine/Graphics/Particle/Derivative/CylinderParticle.h"
// Math
#include "Math/sMath.h"


///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
DebugScene::~DebugScene() {
	// sprite
	sprite_.reset();
	// camera
	camera_.reset();
	camera2_.reset();
	// model
	model2_.reset();
	modelLight_.reset();
	// Colliderのリセット
	ColliderService::Reset();
	// ISceneのデストラクタ
	IScene::~IScene();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void DebugScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();

	/// ===読み込み処理=== ///
#pragma region Load
	// Jsonの読み込み
	Loader::LoadLevelJson("TL_12.json");
#pragma endregion

	/// ===カメラの初期化=== ///
#pragma region Cameraの初期化
	// カメラ1
	camera_ = std::make_shared<Camera>();
	camera_->Initialize();
	camera_->SetTranslate({ 0.0f, 0.0f, -4.0f });
	camera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	// カメラ2
	camera2_ = std::make_shared<Camera>();
	camera2_->Initialize();
	camera2_->SetTranslate({ 0.0f, 0.0f, -30.0f });
	camera2_->SetRotate({ 0.0f, 0.0f, 0.0f });
	// カメラマネージャにカメラを追加
	CameraService::Add("Debug", camera_);
	CameraService::Add("Debug2", camera2_);
#pragma endregion

	/// ===ParticleManager=== ///
#pragma region Particle
	// Particleの追加
	ParticleService::AddParticle("Confetti", std::make_unique<ConfettiParticle>());
	ParticleService::AddParticle("Explosion", std::make_unique<ExplosionParticle>());
	ParticleService::AddParticle("Wind", std::make_unique<WindParticle>());
	ParticleService::AddParticle("Ring", std::make_unique<RingParticle>());
	ParticleService::AddParticle("HitEffect", std::make_unique<HitEffectParticle>());
	ParticleService::AddParticle("Cylinder", std::make_unique<CylinderParticle>());

	/// ===Particle=== ///
	particleInofo_.Translate = { 0.0f, 0.0f, 0.0f };
#pragma endregion

	/// ===スプライトの初期化=== ///
#pragma region Spriteの初期化
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("uvChecker");                   // 初期化(const std::string& spriteNameが必須)
	/* // テクスチャの使い方
	sprite->SetPosition(Vector2(0.0f, 0.0f));           // 場所の設定(初期値は0,0)
	sprite->SetRotation(0.0f);                          // 回転の設定(初期値は0.0);
	sprite->SetSize(Vector2(100.0f, 100.f));            // サイズの設定(初期値は640, 360)
	sprite->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));  // カラーの設定(初期値は1,1,1,1)
	sprite->SetAnchorPoint(Vector2(0.0f, 0.0f));        // アンカーポイントの設定(初期値は0,0)
	sprite->SetTextureSize(Vector2(64.0f, 64.0f));      // テクスチャサイズの設定(初期値は100.0f, 100.0f)
	*/
	sprite_->Update();
#pragma endregion

	/// ===モデルの初期化=== ///
#pragma region Modelの初期化
	/* モデルの使い方
	model_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));              // 座標の設定(初期値は {0.0f, 0.0f, 0.0f} )
	model_->SetRotate(Vector3(0.0f, 0.0f, 0.0f));                // 回転の設定(初期値は {0.0f, 0.0f, 0.0f} )
	model_->SetScale(Vector3(0.0f, 0.0f, 0.0f));                 // スケールの設定(初期値は {1.0f, 1.0f, 1.0f} )
	model_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));           // カラーの設定(初期値は {1.0f, 1.0f, 1.0f, 1.0f} )
	model_->SetLightDirection(Vector3(0.0f, -1.0f, 0.0f));       // Lightの向き設定(初期値は {0.0f, -1.0f, 0.0f})
	model_->SetLightIntensity(1.0f);                             // Lightの明るさの設定(初期値は {1.0f})
	model_->SetLightColor(Vector4(1.0f, 1.0f, 1.0f, 1.0));       // Lightカラーの設定(初期値は {1.0f, 1.0f, 1.0f, 1.0f})
	model_->SetLightShininess(0.27f);                            // 光沢度の設定(初期値は0.27f)
	*/

	// DebugModelの初期化
	debugModel_ = std::make_unique<DebugModel>();
	debugModel_->Initialize();

	// モデルの初期化
	model2_ = std::make_unique<Object3d>();
	model2_->Init(ObjectType::Model, "terrain", LightType::PointLight);
	model2_->SetEnviromentMapData(false, 1.0f);
	model2_->SetTranslate({ 0.0f, -1.0f, 0.0f });

	modelLight_ = std::make_unique<Object3d>();
	modelLight_->Init(ObjectType::Model, "Particle");

	// ポイントライトの位置と半径の変更
	light_.point.position = { 0.0f, 0.0f, -30.0f };
	light_.point.radius = 500.0f;

	// 一回更新をかける
	model2_->Update();
	modelLight_->Update();

#pragma endregion

	/// ===アニメーションモデルの初期化=== ///
#pragma region AnimationModelの初期化
	debugAnimationModel_ = std::make_unique<DebugAnimationModel>();
	debugAnimationModel_->Initialize();
#pragma endregion

#pragma region SkyBoxの初期化
	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize("skyBox", LightType::None);
	//skyBox_->SetScale({ 100.0f, 100.0f, 100.0f });
#pragma endregion

	/// ===ColliderManagerの初期化=== ///
#pragma region ColliderManagerの初期化
	// コライダーの追加
	ColliderService::AddCollider(debugModel_.get());
	ColliderService::AddCollider(debugAnimationModel_.get());
#pragma endregion 

	/// ===ライト=== ///
#pragma region Lightの情報
#pragma endregion

	/// ===音=== ///
#pragma region Audio
	//AudioService::PlayeSound("clear", false);
#pragma endregion

	/// ===OffScreen=== ///
#pragma region OffScreen
	offScreenInfo_.isGrayscale = false;
#pragma endregion

	/// ===Line=== ///
#pragma region Line
	line_ = std::make_unique<Line>();
#pragma endregion

	// ===LevelDataからモデルの生成と配置=== ///
	GenerateModelsFromLevelData("TL_12.json");
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void DebugScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("DebugScene");
	if (ImGui::BeginCombo("Object", "Select")) {
		if (ImGui::Selectable("Sprite", isSetting_.Sprite)) {
			isSetting_.Sprite = true;
			isSetting_.Model = false;
			isSetting_.Particle1 = false;
			isSetting_.Particle2 = false;
			isSetting_.Particle3 = false;
		} else if (ImGui::Selectable("Model", isSetting_.Model)) {
			isSetting_.Sprite = false;
			isSetting_.Model = true;
			isSetting_.Particle1 = false;
			isSetting_.Particle2 = false;
			isSetting_.Particle3 = false;
		} else if (ImGui::Selectable("Particle1", isSetting_.Particle1)) {
			isSetting_.Sprite = false;
			isSetting_.Model = false;
			isSetting_.Particle1 = true;
			isSetting_.Particle2 = false;
			isSetting_.Particle3 = false;
		} else if (ImGui::Selectable("Particle2", isSetting_.Particle2)) {
			isSetting_.Sprite = false;
			isSetting_.Model = false;
			isSetting_.Particle1 = false;
			isSetting_.Particle2 = true;
			isSetting_.Particle3 = false;
		} else if (ImGui::Selectable("Particle3", isSetting_.Particle3)) {
			isSetting_.Sprite = false;
			isSetting_.Model = false;
			isSetting_.Particle1 = false;
			isSetting_.Particle2 = false;
			isSetting_.Particle3 = true;
		}
		ImGui::EndCombo();
	}
	/// ===Sprite=== ///
	if (isSetting_.Sprite) {
		if (!isDisplay_.Sprite && ImGui::Button("Draw")) {
			isDisplay_.Sprite = true;
		} else if (isDisplay_.Sprite && ImGui::Button("UnDraw")) {
			isDisplay_.Sprite = false;
		}
		if (!isImgui_.Sprite && ImGui::Button("Info")) {
			isImgui_.Sprite = true;
		} else if (isImgui_.Sprite && ImGui::Button("UnInfo")) {
			isImgui_.Sprite = false;
		}
		/// ===Info=== ///
		if (isImgui_.Sprite) {
			// Sprite
			ImGui::DragFloat2("Tranlate", &spriteInfo_.Translate.x, 0.1f);
			ImGui::DragFloat("Rotate", &spriteInfo_.Rotate, 0.1f);
			ImGui::DragFloat2("Size", &spriteInfo_.Size.x, 0.1f);
			ImGui::ColorEdit4("Color", &spriteInfo_.Color.x);
		}
	}
	/// ===Model=== ///
	if (isSetting_.Model) {
		if (!isDisplay_.Model && ImGui::Button("Draw")) {
			isDisplay_.Model = true;
		} else if (isDisplay_.Model && ImGui::Button("UnDraw")) {
			isDisplay_.Model = false;
		}
		if (!isImgui_.Model && ImGui::Button("Info")) {
			isImgui_.Model = true;
		} else if (isImgui_.Model && ImGui::Button("UnInfo")) {
			isImgui_.Model = false;
		}
		/// ===Info=== ///
		if (isImgui_.Model) {
			// Model
			ImGui::DragFloat3("Tranlate", &modelInfo_.Translate.x, 0.1f);
			ImGui::DragFloat4("Rotate", &modelInfo_.Rotate.x, 0.1f);
			ImGui::DragFloat3("Size", &modelInfo_.Scale.x, 0.1f);
			ImGui::ColorEdit4("Color", &modelInfo_.Color.x);
			if (!lightType_.Lambert && ImGui::Button("Lambert")) {
				debugAnimationModel_->SetLight(LightType::Lambert);
				debugModel_->SetLight(LightType::Lambert);
				model2_->SetLight(LightType::Lambert);
				ColliderService::SetLight(LightType::Lambert);
				lightType_.Lambert = true;
				lightType_.HalfLambert = false;
				lightType_.PointLight = false;
				lightType_.SpotLight = false;
				lightType_.None = false;
			}
			if (!lightType_.HalfLambert && ImGui::Button("HalfLambert")) {
				debugAnimationModel_->SetLight(LightType::HalfLambert);
				debugModel_->SetLight(LightType::HalfLambert);
				model2_->SetLight(LightType::HalfLambert);
				ColliderService::SetLight(LightType::HalfLambert);
				lightType_.Lambert = false;
				lightType_.HalfLambert = true;
				lightType_.PointLight = false;
				lightType_.SpotLight = false;
				lightType_.None = false;
			}
			if (!lightType_.PointLight && ImGui::Button("PointLight")) {
				debugAnimationModel_->SetLight(LightType::PointLight);
				debugModel_->SetLight(LightType::PointLight);
				model2_->SetLight(LightType::PointLight);
				ColliderService::SetLight(LightType::PointLight);
				lightType_.Lambert = false;
				lightType_.HalfLambert = false;
				lightType_.PointLight = true;
				lightType_.SpotLight = false;
				lightType_.None = false;
			}
			if (!lightType_.SpotLight && ImGui::Button("SpotLight")) {
				debugAnimationModel_->SetLight(LightType::SpotLight);
				debugModel_->SetLight(LightType::SpotLight);
				model2_->SetLight(LightType::SpotLight);
				ColliderService::SetLight(LightType::SpotLight);
				lightType_.Lambert = false;
				lightType_.HalfLambert = false;
				lightType_.PointLight = false;
				lightType_.SpotLight = true;
				lightType_.None = false;
			}
			if (!lightType_.None && ImGui::Button("None")) {
				debugAnimationModel_->SetLight(LightType::None);
				debugModel_->SetLight(LightType::None);
				model2_->SetLight(LightType::None);
				ColliderService::SetLight(LightType::None);
				lightType_.Lambert = false;
				lightType_.HalfLambert = false;
				lightType_.PointLight = false;
				lightType_.SpotLight = false;
				lightType_.None = true;
			}
			if (lightType_.Lambert) {
				// Shiniess
				ImGui::DragFloat("LightShininess", &light_.shininess, 0.01f);
				// DirectionalLight
				ImGui::Text("DirectionalLight");
				ImGui::ColorEdit4("LigthColor", &light_.directional.color.x);
				ImGui::DragFloat3("LightDirection", &light_.directional.direction.x, 0.01f);
				ImGui::DragFloat("lightIntensity", &light_.directional.intensity, 0.01f);
			} else if (lightType_.HalfLambert) {
				// Shiniess
				ImGui::DragFloat("LightShininess", &light_.shininess, 0.01f);
				// DirectionalLight
				ImGui::Text("DirectionalLight");
				ImGui::ColorEdit4("LigthColor", &light_.directional.color.x);
				ImGui::DragFloat3("LightDirection", &light_.directional.direction.x, 0.01f);
				ImGui::DragFloat("lightIntensity", &light_.directional.intensity, 0.01f);
			} else if (lightType_.PointLight) {
				// Shiniess
				ImGui::DragFloat("LightShininess", &light_.shininess, 0.01f);
				// PointLight
				ImGui::Text("PointLight");
				ImGui::ColorEdit4("pointLightColor", &light_.point.color.x);
				ImGui::DragFloat3("pointLightPosition", &light_.point.position.x, 0.01f);
				ImGui::DragFloat("pointLightIntensity", &light_.point.intensity, 0.01f);
				ImGui::DragFloat("pointLightRadius", &light_.point.radius, 0.01f);
				ImGui::DragFloat("pointLightDecay", &light_.point.decay, 0.01f);
			} else if (lightType_.SpotLight) {
				// Shiniess
				ImGui::DragFloat("LightShininess", &light_.shininess, 0.01f);
				// SpotLight
				ImGui::Text("SpotLight");
				ImGui::ColorEdit4("SpotLightColor", &light_.spot.color.x);
				ImGui::DragFloat3("spotLightPosition", &light_.spot.position.x, 0.01f);
				ImGui::DragFloat("SpotLightIntensity", &light_.spot.intensity, 0.01f);
				ImGui::DragFloat3("SpotLightDirection", &light_.spot.direction.x, 0.01f);
				ImGui::DragFloat("SpotLightDistance", &light_.spot.distance, 0.01f);
				ImGui::DragFloat("SpotLightDecay", &light_.spot.decay, 0.01f);
				ImGui::DragFloat("SpotLightCosAngle", &light_.spot.cosAngle, 0.01f);
			}
		}
	}
	/// ===Particle1=== ///
	if (isSetting_.Particle1) {
		if (ImGui::Button("Draw")) {
			ParticleService::Emit("Ring", particleInofo_.Translate);
			ParticleService::Emit("HitEffect", particleInofo_.Translate);
			ParticleService::SetTexture("Ring", "gradationLine");
			ParticleService::SetTexture("HitEffect", "circle2");
		}
		if (!isImgui_.Particle1 && ImGui::Button("Info")) {
			isImgui_.Particle1 = true;
		} else if (isImgui_.Particle1 && ImGui::Button("UnInfo")) {
			isImgui_.Particle1 = false;
		}
		/// ===Info=== ///
		if (isImgui_.Particle1) {
			// Particle
			ImGui::DragFloat3("Tranlate", &particleInofo_.Translate.x, 0.1f);
		}
	}
	/// ===Particle2=== ///
	if (isSetting_.Particle2) {
		if (ImGui::Button("Draw")) {
			ParticleService::Emit("Explosion", particleInofo_.Translate);
		}
		if (!isImgui_.Particle2 && ImGui::Button("Info")) {
			isImgui_.Particle2 = true;
		} else if (isImgui_.Particle2 && ImGui::Button("UnInfo")) {
			isImgui_.Particle2 = false;
		}
		/// ===Info=== ///
		if (isImgui_.Particle2) {
			// Particle
			ImGui::DragFloat3("Tranlate", &particleInofo_.Translate.x, 0.1f);
		}
	}
	/// ===Particle3=== ///
	if (isSetting_.Particle3) {
		if (ImGui::Button("Draw")) {
			ParticleService::Emit("Confetti", particleInofo_.Translate);
			ParticleService::SetTexture("Confetti", "monsterBall");
		}
		if (!isImgui_.Particle3 && ImGui::Button("Info")) {
			isImgui_.Particle3 = true;
		} else if (isImgui_.Particle3 && ImGui::Button("UnInfo")) {
			isImgui_.Particle3 = false;
		}
		/// ===Info=== ///
		if (isImgui_.Particle3) {
			// Particle
			ImGui::DragFloat3("Tranlate", &particleInofo_.Translate.x, 0.1f);
		}
	}
	ImGui::End();

	/// ===Camera=== ///
	ImGui::Begin("Camera");
	ImGui::Checkbox("Flag", &cameraInfo_.Set);
	ImGui::DragFloat3("Translate", &cameraInfo_.Translate.x, 0.1f);
	ImGui::DragFloat4("Rotate", &cameraInfo_.Rotate.x, 0.001f);
	ImGui::End();
	/// ===Audio=== ///
	ImGui::Begin("Audio");
	ImGui::Checkbox("play", &audioInfo_.play);
	ImGui::DragFloat("Volume", &audioInfo_.volume, 0.01f);
	ImGui::DragFloat("Ptich", &audioInfo_.pitch, 0.01f);
	ImGui::End();

	/// ===Line=== ///
	ImGui::Begin("Line");
	ImGui::DragFloat3("Start", &lineInfo_.startPos.x, 0.01f);
	ImGui::DragFloat3("End", &lineInfo_.endPos.x, 0.01f);
	ImGui::DragFloat4("Color", &lineInfo_.color.x, 0.01f);
	ImGui::End();

	debugAnimationModel_->ImGuiInfo();
	debugModel_->ImGuiInfo();

#endif // USE_IMGUI

#pragma region OffScreen
#pragma endregion

	/// ===カメラの変更=== ///
#pragma region カメラの変更
	if (cameraInfo_.Set) {
		CameraService::SetActiveCamera("Debug2");
	} else {
		CameraService::SetActiveCamera("Debug");
	}
#pragma endregion

	/// ===キーボード関連の処理=== ///
#pragma region キーボード関連の処理
	if (InputService::PushKey(DIK_D)) {
		cameraInfo_.Translate.x += 0.01f;
	} else if (InputService::PushKey(DIK_A)) {
		cameraInfo_.Translate.x -= 0.01f;
	}
	if (InputService::PushKey(DIK_W)) {
		cameraInfo_.Translate.y += 0.01f;
	} else if (InputService::PushKey(DIK_S)) {
		cameraInfo_.Translate.y -= 0.01f;
	}
	if (InputService::PushKey(DIK_UP)) {
		cameraInfo_.Translate.z += 0.01f;
	} else if (InputService::PushKey(DIK_DOWN)) {
		cameraInfo_.Translate.z -= 0.01f;
	}
	if (InputService::PushKey(DIK_LEFT)) {
		// Y軸回転のクォータニオンを加算
		Quaternion delta = Math::RotateY(-0.01f);
		cameraInfo_.Rotate = delta * cameraInfo_.Rotate;
	} else if (InputService::PushKey(DIK_RIGHT)) {
		Quaternion delta = Math::RotateY(+0.01f);
		cameraInfo_.Rotate = delta * cameraInfo_.Rotate;
	}
#pragma endregion

	/// ===マウス関連の処理=== ///
#pragma region マウス関連の処理
	if (InputService::PushMouse(MouseButtonType::Left)) {
		mouseInfo_.PushLeft_ = true;
	} else {
		mouseInfo_.PushLeft_ = false;
	}
	if (InputService::TriggerMouse(MouseButtonType::Right)) {
		if (mouseInfo_.TriggerRight_) {
			mouseInfo_.TriggerRight_ = false;
		} else {
			mouseInfo_.TriggerRight_ = true;
		}
	}
	mouseInfo_.Position_.x = static_cast<float>(InputService::GetMousePosition().x);
	mouseInfo_.Position_.y = static_cast<float>(InputService::GetMousePosition().y);
#pragma endregion

	/// ===コントローラーの処理=== ///
#pragma region コントローラの処理

	// スティックの入力処理を取得
	StickState leftStick = InputService::GetLeftStickState(0);
	StickState rightStick = InputService::GetRightStickState(0);

	// モデルの移動処理
	modelInfo_.Translate.x += leftStick.x * 0.01f;
	modelInfo_.Translate.y += leftStick.y * 0.01f;

	// カメラの移動処理
	cameraInfo_.Translate.x += rightStick.x * 0.01f;
	cameraInfo_.Translate.y += rightStick.y * 0.01f;

	if (InputService::FlickLeftStick(0, 0.5f)) {
		if (cameraInfo_.Set) {
			cameraInfo_.Set = false;
		} else {
			cameraInfo_.Set = true;
		}
	}

#pragma endregion

	/// ===Audioのセット=== ///
#pragma region Audioのセット
	if (audioInfo_.play) {
		AudioService::PlayeSound("fanfare", false);
		AudioService::VolumeSound("fanfare", audioInfo_.volume);
		AudioService::PitchSound("fanfare", audioInfo_.pitch);
	} else {
		AudioService::StopSound("fanfare");
		AudioService::StopSound("fanfare");
	}
#pragma endregion

	/// ===スプライトの更新=== ///
#pragma region スプライトの更新
	sprite_->SetPosition(spriteInfo_.Translate);
	sprite_->SetRotation(spriteInfo_.Rotate);
	sprite_->SetSize(spriteInfo_.Size);
	sprite_->SetColor(spriteInfo_.Color);
	sprite_->Update();
#pragma endregion

	/// ===モデルの更新=== ///
#pragma region モデルの更新
	//model2_->SetTranslate(modelInfo_.Translate);
	//model2_->SetRotate(modelInfo_.Rotate);
	model2_->SetLightData(light_);
	model2_->Update();

	modelLight_->SetTranslate(light_.point.position);
	modelLight_->Update();

	// デバッグモデルの更新(LightDataとCameraの設定はColliderManagerで行う)
	debugModel_->Update();
#pragma endregion

	/// ===AnimaitonModelの更新=== ///
#pragma region Animationモデルの更新
	debugAnimationModel_->Update();
#pragma endregion

#pragma region SkyBoxの更新
	skyBox_->Update();
#pragma endregion

	/// ===Particle=== ///
#pragma region Particle
	// パーティクルの生成
	if (InputService::TriggerKey(DIK_SPACE)) {
		ParticleService::Emit("Ring", particleInofo_.Translate);
		ParticleService::Emit("HitEffect", particleInofo_.Translate);
		ParticleService::SetTexture("Ring", "gradationLine");
		ParticleService::SetTexture("HitEffect", "circle2");
	}
#pragma endregion

	/// ===カメラの更新=== ///
#pragma region カメラの更新
	camera_->SetRotate(cameraInfo_.Rotate);
	camera_->SetTranslate(cameraInfo_.Translate);
#pragma endregion

	/// ===Lineの更新=== ///
#pragma region Lineの更新
#ifdef _DEBUG
	//line_->DrawLine(lineInfo_.startPos, lineInfo_.endPos, lineInfo_.color);
	//line_->DrawSphere({ modelInfo_.Translate, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
	line_->DrawGrid({ 0.0f,-2.0f, 0.0f }, { 100.0f, 1.0f, 100.0f }, 50, {1.0f, 1.0f, 1.0f, 1.0f});
#endif // _DEBUG
#pragma endregion

#pragma region ColliderManagerの更新
	ColliderService::SetLightData(light_);
#pragma endregion

	/// ===LevelEditorの更新=== ///
	UpdateLevelModels();

	/// ===ISceneのの更新=== ///
	IScene::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void DebugScene::Draw() {
#pragma region 背景スプライト描画

	/// ===Sprite=== ///
	if (isDisplay_.Sprite) {
		sprite_->Draw(GroundType::Back); // GroundTypeで背景か前景を選択か初期は前景描画。 BlendMode変更可　sprite->Draw(BlendMode::kBlendModeAdd);  
	}

#pragma endregion

#pragma region モデル描画

	// アニメーションの描画
	//debugAnimationModel_->Draw();
	/// ===Model=== ///
	//debugModel_->Draw(); // BlendMode変更可能 model_->Draw(BlendMode::kBlendModeAdd);s

	// Modelの描画
	if (isDisplay_.Model) {
		// SkyBoxの描画
		skyBox_->Draw();
		// アニメーションの描画
		debugAnimationModel_->Draw();
		/// ===Model=== ///
		debugModel_->Draw(); // BlendMode変更可能 model_->Draw(BlendMode::kBlendModeAdd);

		model2_->Draw();
		//modelLight_->Draw();
	}

	/// ===LevelEditorの更新=== ///
	DrawLevelModels();

	/// ===ISceneの描画=== ///
	IScene::Draw();

#pragma endregion

#pragma region 前景スプライト描画
#pragma endregion
}