#include "OffScreenRenderer.h"
// Service
#include "Engine/System/Service/Render.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
OffScreenRenderer::~OffScreenRenderer() {
	copyImage_.reset();
	grayscale_.reset();
	vignette_.reset();
	boxFilter3x3_.reset();
	boxFilter5x5_.reset();
	radiusBlur_.reset();
	outLine_.reset();
	dissolve_.reset();
	renderTexture_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void OffScreenRenderer::Initialize(
	ID3D12Device* device,
	SRVManager* srv, RTVManager* rtv,
	uint32_t width, uint32_t height, const Vector4& clearColor) {

	// RenderTextureを初期化
	renderTexture_ = std::make_shared<RenderTexture>();
	renderTexture_->Initialize(srv, rtv, width, height, clearColor, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	renderTexture_->CreateRenderTexture(device);

	/// ===RenderPassの登録=== ///
	// SceneRenderPass
	copyImage_ = std::make_shared<CopyImageEffect>();
	copyImage_->Initialize(device,renderTexture_);

	// グレースケールレンダーパス
	grayscale_ = std::make_shared<GrayscaleEffect>();
	grayscale_->Initialize(device, renderTexture_);

	// ビネットレンダーパス
	vignette_ = std::make_shared<VignetteEffect>();
	vignette_->Initialize(device, renderTexture_);

	// Dissolveエフェクト
	dissolve_ = std::make_shared<DissolveEffect>();
	dissolve_->Initialize(device, renderTexture_);

	// 3x3ボックスフィルタレンダーパス
	boxFilter3x3_ = std::make_shared<BoxFilter3x3Effect>();
	boxFilter3x3_->Initialize(device, renderTexture_);

	// 5x5ボックスフィルタレンダーパス
	boxFilter5x5_ = std::make_shared<BoxFilter5x5Effect>();
	boxFilter5x5_->Initialize(device, renderTexture_);

	// 半径ブラーエフェクト
	radiusBlur_ = std::make_shared<RadiusBlurEffect>();
	radiusBlur_->Initialize(device, renderTexture_);

	// OutLineエフェクト
	outLine_ = std::make_shared<OutLineEffect>();
	outLine_->Initialize(device, renderTexture_);
}

///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
void OffScreenRenderer::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
	switch (type_) {
	case OffScreenType::CopyImage:
		copyImage_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::Grayscale:
		grayscale_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::Vignette:
		vignette_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::BoxFilter3x3:
		boxFilter3x3_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::BoxFilter5x5:
		boxFilter5x5_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::RadiusBlur:
		radiusBlur_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::OutLine:
		outLine_->PreDraw(commandList, dsvHandle);
		break;
	case OffScreenType::Dissolve:
		dissolve_->PreDraw(commandList, dsvHandle);
		break;
	}
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void OffScreenRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
	switch (type_) {
	case OffScreenType::CopyImage:
		copyImage_->Draw(commandList);
		break;
	case OffScreenType::Grayscale:
		grayscale_->Draw(commandList);
		break;
	case OffScreenType::Vignette:
		vignette_->Draw(commandList);
		break;
	case OffScreenType::BoxFilter3x3:
		boxFilter3x3_->Draw(commandList);
		break;
	case OffScreenType::BoxFilter5x5:
		boxFilter5x5_->Draw(commandList);
		break;
	case OffScreenType::RadiusBlur:
		radiusBlur_->Draw(commandList);
		break;
	case OffScreenType::OutLine:
		outLine_->Draw(commandList);
		break;
	case OffScreenType::Dissolve:
		dissolve_->Draw(commandList);
		break;
	}
}

///-------------------------------------------/// 
/// 描画後処理
///-------------------------------------------///
void OffScreenRenderer::PostDraw(ID3D12GraphicsCommandList* commandList) {}

///-------------------------------------------/// 
/// ImGuiの描画
///-------------------------------------------///
#ifdef USE_IMGUI
void OffScreenRenderer::DrawImGui() {
	const char* typeNames[] = {
		"CopyImage",
		"Grayscale",
		"Vignette",
		"BoxFilter3x3",
		"BoxFilter5x5",
		"RadiusBlur",
		"OutLine",
		"Dissolve",
	};

	int current = static_cast<int>(type_);
	if (ImGui::Begin("OffScreen Effect")) {
		ImGui::Text("Current Effect: %s", typeNames[current]);  // ← ★ ここで名前表示

		if (ImGui::Combo("Effect Type", &current, typeNames, IM_ARRAYSIZE(typeNames))) {
			type_ = static_cast<OffScreenType>(current);
		}
	}

	switch (type_) {
	case OffScreenType::CopyImage:
		copyImage_->ImGuiInfo();
		break;
	case OffScreenType::Grayscale:
		grayscale_->ImGuiInfo();
		break;
	case OffScreenType::Vignette:
		vignette_->ImGuiInfo();
		break;
	case OffScreenType::BoxFilter3x3:
		boxFilter3x3_->ImGuiInfo();
		break;
	case OffScreenType::BoxFilter5x5:
		boxFilter5x5_->ImGuiInfo();
		break;
	case OffScreenType::RadiusBlur:
		radiusBlur_->ImGuiInfo();
		break;
	case OffScreenType::OutLine:
		outLine_->ImGuiInfo();
		break;
	case OffScreenType::Dissolve:
		dissolve_->ImGuiInfo();
		break;
	}

	ImGui::End();
}
#endif // USE_IMGUI

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// RTV
D3D12_CPU_DESCRIPTOR_HANDLE OffScreenRenderer::GetResultRTV() const { return renderTexture_->GetRTVHandle(); }
// SRV
D3D12_GPU_DESCRIPTOR_HANDLE OffScreenRenderer::GetResultSRV() const { return renderTexture_->GetSRVHandle(); }
// RTVIndex
uint32_t OffScreenRenderer::GetRTVHandleIndex() const { return renderTexture_->GetRTVHandleIndex(); }
// SRVIndex
uint32_t OffScreenRenderer::GetSRVHandleIndex() const { return renderTexture_->GetSRVHandleIndex(); }
// Reosurce
ID3D12Resource* OffScreenRenderer::GetBuffer() const { return renderTexture_->GetBuffer(); }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Typeの設定
void OffScreenRenderer::SetType(OffScreenType type) { type_ = type; }
