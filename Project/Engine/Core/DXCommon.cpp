#include "DXCommon.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/WinApp.h"
#include "Engine/System/Managers/RTVManager.h"
#include "Engine/System/Managers/DSVManager.h"

#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>
#include <format>

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#endif // USE_IMGUI

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
DXCommon::~DXCommon() {
	commandQueue_.Reset();
	swapChain_.Reset();
	commandList_.Reset();
	commandAllocator_.Reset();
	fence_.Reset();
	swapChainResource_->Reset();
	backBuffers_.clear();
	device_.Reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void DXCommon::Initialize(
	WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {
	// nullptrチェック
	assert(winApp);

	// FPS固定の初期化
	InitializeFixFPS();

	/* sleepの分解能を上げておく
	(システムタイマーのカウント数を1ミリ秒にする) */
	timeBeginPeriod(1);

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	// デバッグレイヤー
	DebugLayer();

	// DXGIデバイスの初期化
	InitializeDXGIDevice();

	// エラー・警告
	DebugInfo();

	// コマンド関連の初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	// フェンスの生成
	CreateFence();

	// ビューポート
	CreateViewport(backBufferWidth_, backBufferHeight_);

	// シザー矩形
	CreateScissor(backBufferWidth_, backBufferHeight_);

	// DXCの初期化
	InitializeCompiler();
}

///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
/// ===RenderTexture=== ///
void DXCommon::PreDrawRenderTexture(ID3D12Resource* resource) {

	/* ///////////////////
		　 バリアを張る
	*/ ///////////////////
	// TransitionBarrierの設定
	// 今回のバリアはTransition
	barrierRenderTexture_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// Noneにしておく
	barrierRenderTexture_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrierRenderTexture_.Transition.pResource = resource;
}
/// ===ImGui=== ///
void DXCommon::PreDrawImGui(RTVManager* rtv) {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	/* ///////////////////
		　 バリアを張る
	*/ ///////////////////
	// TransitionBarrierの設定
	// 今回のバリアはTransition
	barrierSwapChain_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// Noneにしておく
	barrierSwapChain_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrierSwapChain_.Transition.pResource = swapChainResource_[backBufferIndex].Get();

	// 遷移前(現在)のResourceState
	barrierSwapChain_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	// 遷移後のResourceState
	barrierSwapChain_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrierSwapChain_);
	
	// RTVの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandel = rtv->GetCPUDescriptorHandle(backBufferIndex);
	commandList_->OMSetRenderTargets(1, &rtvHandel, false, nullptr);
	// RTVのクリア
	const float cleaerColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; // 青色っっぽい色、RGBAの順
	rtv->ClearRenderTarget(commandList_.Get(), backBufferIndex, cleaerColor);
}

///-------------------------------------------/// 
/// コマンドを積む
///-------------------------------------------///
void DXCommon::BeginCommand() {
	// コマンドを積む
	commandList_->RSSetViewports(1, &viewPort_); // viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_); // scissorを設定
}


///-------------------------------------------/// 
/// バリアの状態遷移
///-------------------------------------------///
void DXCommon::TransitionRenderTarget() {
	// 遷移前(現在)のResourceState
	barrierRenderTexture_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// 遷移後のResourceState
	barrierRenderTexture_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrierRenderTexture_);
}


///-------------------------------------------/// 
/// 描画後処理
///-------------------------------------------///
void DXCommon::PostDraw() {
	HRESULT hr;

	// RenderTargetからPresentにする
	// RenderTexture
	barrierRenderTexture_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrierRenderTexture_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// SwapChain
	barrierSwapChain_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierSwapChain_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrierRenderTexture_);
	commandList_->ResourceBarrier(1, &barrierSwapChain_);

	// コマンドリストの内容を確定させる。
   // すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUコマンドリストの実行を行わせる
	ID3D12CommandList* commandList[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandList); // コマンドリストをキック

	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0); // スワップチェーンのバッファを表示

	// GPUがここまでたどり着いたときに、Fenceの当た値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceValue_); // フェンスを更新

	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {

		// FenceのSignalを待つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr); // イベントの作成が成功したか確認

		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent); // フェンスのシグナル設定

		// イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE); // 指定のイベントがシグナル状態になるまで待機

		//イベントの解放
		CloseHandle(fenceEvent); // イベントハンドルの解放
	}

	// FPS固定
	UpdateFixFPS(); // フレームレートを固定する処理

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset(); // コマンドアロケータをリセット
	assert(SUCCEEDED(hr)); // リセットが成功したか確認
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr); // 新しいコマンドリストをリセット
	assert(SUCCEEDED(hr)); // リセットが成功したか確認
}

///-------------------------------------------/// 
/// DescriptorHeapの生成
///-------------------------------------------///
ComPtr<ID3D12DescriptorHeap> DXCommon::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}
///-------------------------------------------/// 
/// DescriptorSizeの取得
///-------------------------------------------///
const uint32_t DXCommon::GetRTVDescriptorSize() { return device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); }
const uint32_t DXCommon::GetDSVDescriptorSize() { return device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); }
const uint32_t DXCommon::GetSRVDescriptorSize() { return device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }

///-------------------------------------------/// 
/// デバッグレイヤー
///-------------------------------------------///
void DXCommon::DebugLayer() {
	ComPtr<ID3D12Debug1> debugController = nullptr;
	if ((SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))) {

		// デバッグレイヤーを有効化
		debugController->EnableDebugLayer();

		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}

///-------------------------------------------/// 
/// エラー・警告
///-------------------------------------------///
void DXCommon::DebugInfo() {
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 解放
		//infoQueue->Release();

		/// *****************************************************
		/// エラーと警告の抑制
		/// *****************************************************
		// 抑制するメッセージ
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https:/stackoverflow.com/questions/69805245/DirectX-12-application-is-crashing-in-windows11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
}

///-------------------------------------------/// 
/// DXGIデバイス初期化
///-------------------------------------------///
void DXCommon::InitializeDXGIDevice() {

	/* /////////////////////////////////////////////////////
						DirectX12の初期化
	*/ /////////////////////////////////////////////////////
	// HEWSULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判断できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでAssertにしておく
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(
		i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {

		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得できないのは一大事

		//  ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {

			// 採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(std::format(L"Use Adapater:{}\n", adapterDesc.Description));
			break;
		}

		useAdapter = nullptr;
	}

	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = {
		"12.2", "12.1", "12.0"
	};

	//高い順に生成できるか試してみる
	for (size_t i = 0; i < _countof(levels); ++i) {

		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), levels[i], IID_PPV_ARGS(&device_));

		// 指定した機能レベルでデバイスが生成できたかを確認	
		if (SUCCEEDED(hr)) {
			//
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	//初期化完了のログを出す
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	/* /////////////////////////////////////////////////////
						エラー・警告、即ち停止
	*/ /////////////////////////////////////////////////////
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		/* /////////////////////////////////////////////////////
							エラーと警告の抑制
		*/ /////////////////////////////////////////////////////
		// 抑制するメッセージ
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https:/stackoverflow.com/questions/69805245/DirectX-12-application-is-crashing-in-windows11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif
}

///-------------------------------------------/// 
/// コマンド関連の初期化
///-------------------------------------------///
void DXCommon::InitializeCommand() {
	HRESULT hr;

	// コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(
		&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	// コマンドアロケータの生成
	hr = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));

	// コマンドリストの生成
	hr = device_->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));
}

///-------------------------------------------/// 
/// スワップチェーンの生成
///-------------------------------------------///
void DXCommon::CreateSwapChain() {
	HRESULT hr;

	// スワップチェーンを生成する
	swapChainDesc_.Width = backBufferWidth_;      // 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = backBufferHeight_;     // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
	swapChainDesc_.SampleDesc.Count = 1;  // マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;  // ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  // モニタに移したら、中身を破棄

	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc_,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// スワップチェーンからResourceを引っ張ってくる
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));

	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr));
}

///-------------------------------------------/// 
/// フェンスの生成
///-------------------------------------------///
void DXCommon::CreateFence() {
	HRESULT hr;

	// 初期値0でFenceを作る
	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
}

///-------------------------------------------/// 
/// DXCの初期化
///-------------------------------------------///
void DXCommon::InitializeCompiler() {
	HRESULT hr;

	// dxcUtils
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));

	// dxcCompiler
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// IncludeHandler
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

///-------------------------------------------/// 
/// ビューポート
///-------------------------------------------///
void DXCommon::CreateViewport(const int32_t kClientWindth, const int32_t kClientHeight) {

	// クライアント領域のサイズと一緒にして画面全体に表示
	viewPort_.Width = float(kClientWindth);
	viewPort_.Height = float(kClientHeight);
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.MinDepth = 0.0f;
	viewPort_.MaxDepth = 1.0f;
}

///-------------------------------------------/// 
/// シザー矩形
///-------------------------------------------///
void DXCommon::CreateScissor(const int32_t kClientWindth, const int32_t kClientHeight) {

	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = kClientWindth;
	scissorRect_.top = 0;
	scissorRect_.bottom = kClientHeight;
}

///-------------------------------------------/// 
/// FPS固定の初期化
///-------------------------------------------///
void DXCommon::InitializeFixFPS() {

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

///-------------------------------------------/// 
/// FPS固定の更新
///-------------------------------------------///
void DXCommon::UpdateFixFPS() {

	// 1/60秒ピッタリの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在の時間を取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 前回の記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// バックバッファの横幅の取得
int32_t DXCommon::GetBackBufferWidth() const { return backBufferWidth_; }
// バックバッファの縦幅の取得
int32_t DXCommon::GetBackBufferHeight() const { return backBufferHeight_; }
// バックバッファの数を取得
size_t DXCommon::GetBackBufferCount() const { return swapChainDesc_.BufferCount; }
// DXGFactoryの取得
IDXGIFactory7* DXCommon::GetDXGFactory() const { return dxgiFactory_.Get(); }
// デバイスの取得
ID3D12Device* DXCommon::GetDevice() const { return device_.Get(); }
// dxcUtilsの取得
IDxcUtils* DXCommon::GetDxcUtils() const { return dxcUtils_.Get(); }
//  dxcCompilerの取得
IDxcCompiler3* DXCommon::GetDxcCompiler() const { return dxcCompiler_.Get(); }
// IncludeHandlerの取得
IDxcIncludeHandler* DXCommon::GetIncludeHandler() const { return includeHandler_.Get(); }
// 描画コマンドリストの取得
ID3D12GraphicsCommandList* DXCommon::GetCommandList() const { return commandList_.Get(); }
// SwapChainの取得
UINT DXCommon::GetBackBufferIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }
// SwapChainResourceの取得
ID3D12Resource* DXCommon::GetSwapChainResource(uint32_t index) const {
	assert(index < 2);
	return swapChainResource_[index].Get(); 
}
// CPUのディスクリプターハンドルの取得 
D3D12_CPU_DESCRIPTOR_HANDLE DXCommon::GetCPUDescriptorHandle(
	const ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	// ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);

	return handleCPU;
}
// GPUのディスクリプターハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE DXCommon::GetGPUDescriptorHandle(
	const ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	// ディスクリプタの先頭を取得
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);

	return handleGPU;
}