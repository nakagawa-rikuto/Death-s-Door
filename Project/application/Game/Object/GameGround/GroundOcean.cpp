#include "GroundOcean.h"
// Service
#include "Service/Input.h"
#include "Service/Camera.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GroundOcean::~GroundOcean() {
	fftOcean_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void GroundOcean::Initialize() {

	// AABBコライダーの初期化と設定
	GameObject::Initialize();
	GameObject::SetAABB({ { -500.0f, -2.0f, -500.0f }, { 500.0f, 0.0f, 500.0f } });

	// FFTオーシャンの初期化
	fftOcean_ = std::make_unique<MiiEngine::FFTOceanGenerator>();
	fftOcean_->Initialize(256);
	fftOcean_->SetTranslate({ 0.0f, 0.0f, 0.0f });
	fftOcean_->SetScale({ 1000.0f, 1.0f, 1000.0f });

	// アクティブカメラを設定
	fftOcean_->SetCamera(Service::Camera::GetActiveCamera());

	// OceanParamsの初期設定
	MiiEngine::OceanParams oceanParams = fftOcean_->GetOceanParams();
	oceanParams.gridWidth = 2000.0f;
	oceanParams.windDirection = { 1.0f, 1.0f };
	oceanParams.windowSpeed = 9.5f;
	oceanParams.amplitude = 50.0f;
	oceanParams.lambda = 0.5f;
	fftOcean_->SetOceanParams(oceanParams);
	
	// Oceanの描画コールバックを設定
	MiiEngine::OceanRenderCB oceanRenderCB = fftOcean_->GetOceanRenderCB();
	oceanRenderCB.sunDirection = { 0.0f, 0.7f, 0.9f };
	oceanRenderCB.sunPower = 24.0f;
	oceanRenderCB.fresnelBias = 0.02f;
	oceanRenderCB.roughness = 0.08f;
	oceanRenderCB.sssStrength = 0.01f;
	oceanRenderCB.foamSoftness = 1.0f;
	fftOcean_->SetOceanRenderCB(oceanRenderCB);

	// 初回更新
	fftOcean_->Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GroundOcean::Update() {

	// アクティブカメラを毎フレーム同期
	fftOcean_->SetCamera(Service::Camera::GetActiveCamera());
	// オーシャンの更新
	fftOcean_->Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void GroundOcean::Draw(MiiEngine::BlendMode mode) {
	// 描画
	fftOcean_->Draw(mode);
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void GroundOcean::ShowImGui() {
#ifdef USE_IMGUI
	// オーシャンのImGui表示
	fftOcean_->ShowImGui();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void GroundOcean::OnCollision(MiiEngine::Collider* collider) {
	collider;
}
