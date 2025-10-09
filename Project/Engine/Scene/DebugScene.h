#pragma once
/// ===Includ=== ///
#include "Engine/Scene/IScene.h"
// Game
#include "application/Drawing/2d/Sprite.h"
#include "application/Drawing/3d/Object3d.h"
// DebugModel
#include "Engine/Debug/DebugModel.h"
#include "Engine/Debug/DebugAnimationModel.h"
// SkyBox
#include "Engine/Graphics/3d/SkyBox/SkyBox.h"

// Line
#include "application/Drawing/3d/Line.h"
// Math
#include "Math/sMath.h"

#include <numbers>

#include "application/Game/Entity/Player/Player.h"

///=====================================================/// 
/// デバッグシーン
///=====================================================///
class DebugScene : public IScene {
public:/// ===メンバ関数=== ///

	DebugScene() = default;
	~DebugScene();

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;

private:/// ===メンバ変数=== ///
	/// <summary>
	/// シーン用
	/// </summary>

	/// ===ImGui用フラグ=== ///
#pragma region ImGui
	struct Setting {
		bool Sprite;
		bool Model;
		bool Particle1;
		bool Particle2;
		bool Particle3;
	};
	struct Display {
		bool Sprite;
		bool Model;
	};
	struct Imgui {
		bool Sprite;
		bool Model;
		bool Particle1;
		bool Particle2;
		bool Particle3;
	};
	struct LightTypeInfo {
		bool Lambert;
		bool HalfLambert;
		bool PointLight;
		bool SpotLight;
		bool None;
	};
	Setting isSetting_ = { false };
	Display isDisplay_ = { false };
	Imgui isImgui_ = { false };
	LightTypeInfo lightType_ = { false };
#pragma endregion

	/// ===クラス=== ///
#pragma region クラス
	// スプライト
	std::unique_ptr<Sprite> sprite_;
	//Camera
	std::shared_ptr<GameCamera> camera_;
	std::shared_ptr<GameCamera> camera2_;
	// モデル
	std::unique_ptr<Object3d> model2_;
	// モデルライト
	std::unique_ptr<Object3d> modelLight_;
	// Line
	std::unique_ptr<Line> line_;
	// DebugModel
	std::unique_ptr<DebugModel> debugModel_;
	// DebugAnimationModel
	std::unique_ptr<DebugAnimationModel> debugAnimationModel_;
	// SkyBox
	std::unique_ptr<SkyBox> skyBox_;

#pragma endregion

	/// ===変数=== ///
#pragma region 変数
	// モデル
	struct ModelInfo {
		Vector3 Translate = { 0.0f, -1.3f, 0.0f };
		Quaternion Rotate = { 0.0f, 0.0f, 0.0f, 1.0f };
		Vector3 Scale = { 1.0f, 1.0f, 1.0f };
		Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool isRotate = false;
	};
	ModelInfo modelInfo_;

	// sprite
	struct SpriteInfo {
		Vector2 Translate = { 0.0f, 0.0f };
		float Rotate = 0.0f;
		Vector2 Size = { 100.0f, 100.0f };
		Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
	SpriteInfo spriteInfo_;

	// ライト
	LightInfo light_ = {
		{ 48.5f } ,
		{ { 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, 1.0f, 0.0f } , 1.0f },
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, 2.0f, 0.0f } , 1.0f, 10.0f, 1.0f},
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 2.0f, 1.25f, 0.0f } , 4.0f, Normalize(Vector3{ -1.0f, -1.0f, 0.0f }), 7.0f, 2.0f, std::cos(std::numbers::pi_v<float> / 3.0f)}
	};


	// カメラ
	struct CameraInfo {
		Vector3 Translate = { 0.0f, 0.0f, -13.0f };
		Quaternion Rotate = { 0.0f, 0.0f, 0.0f, 1.0f };
		bool Set = false;
	};
	CameraInfo cameraInfo_;

	// Audio
	struct AudioInfo {
		bool play = false;
		float volume = 1.0f;
		float pitch = 1.0f;
	};
	AudioInfo audioInfo_;

	// マウス
	struct MouseInfo {
		bool PushLeft_ = false;
		bool TriggerRight_ = false;
		Vector2 Position_ = { 0.0f, 0.0f };
	};
	MouseInfo mouseInfo_;
	
	// Particle
	struct ParticleInfo {
		bool start_ = false;
		Vector3 Translate = { 0.0f, 0.0f, 0.0f };
	};
	ParticleInfo particleInofo_;
	
	// OffScreen
	struct OffScreenInfo {
		bool isGrayscale = false;
	};
	OffScreenInfo offScreenInfo_;
	
	// Line
	struct LineInfo {
		Vector3 startPos = { 0.0f, 0.0f, 0.0f };
		Vector3 endPos = { 10.0f, 0.0f, 0.0f };
		Vector4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
	};
	LineInfo lineInfo_;
	
#pragma endregion

	// Player
	std::unique_ptr<Player> player_;
};