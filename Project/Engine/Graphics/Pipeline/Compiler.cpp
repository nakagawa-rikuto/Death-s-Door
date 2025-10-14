#include "Compiler.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/DXCommon.h"

#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Compiler::~Compiler() {
	objVSBlob_.Reset();
	objPSBlob_.Reset();
}

///-------------------------------------------/// 
/// Shaderデーブルの定義
///-------------------------------------------///
namespace {
	// 情報
	struct ShaderInfo {
		std::wstring vsPath;
		std::wstring psPath;
	};

	const std::unordered_map<PipelineType, ShaderInfo> kShaderTable_ = {
	{ PipelineType::ForGround2D,  { L"2D/Obj2D.VS.hlsl",		      L"2D/Obj2D.PS.hlsl" } },
	{ PipelineType::BackGround2D, { L"2D/Obj2D.VS.hlsl",			  L"2D/Obj2D.PS.hlsl" } },
	{ PipelineType::Obj3D,        { L"3D/Obj3D.VS.hlsl",			  L"3D/Obj3D.PS.hlsl" } },
	{ PipelineType::PrimitiveSkyBox,       { L"3D/SkyBox.VS.hlsl",             L"3D/SkyBox.PS.hlsl"} },
	{ PipelineType::Skinning3D,   { L"3D/SkinningObj3D.VS.hlsl",      L"3D/SkinningObj3D.PS.hlsl" } },
	{ PipelineType::Line3D,       { L"3D/Line3D.VS.hlsl",             L"3D/Line3D.PS.hlsl"}},
	{ PipelineType::Particle,     { L"Particle/Particle.VS.hlsl",     L"Particle/Particle.PS.hlsl" } },
	{ PipelineType::OffScreen,    { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/CopyImage.PS.hlsl" } },
	{ PipelineType::Grayscale,    { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/Grayscale.PS.hlsl" } },
	{ PipelineType::Vignette ,    { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/Vignette.PS.hlsl" } },
	{ PipelineType::Dissolve,     { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/Dissolve.PS.hlsl" } },
	{ PipelineType::BoxFilter3x3, { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/BoxFilter3x3.PS.hlsl" } },
	{ PipelineType::BoxFilter5x5, { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/BoxFilter5x5.PS.hlsl" } },
	{ PipelineType::RadiusBlur,	  { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/RadialBlur.PS.hlsl" } },
	{ PipelineType::OutLine,	  { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/LuminanceBasedOutline.PS.hlsl" } },
	{ PipelineType::ShatterGlass, { L"OffScreen/Fullscreen.VS.hlsl",  L"OffScreen/ShatterGlass.PS.hlsl" } },
	};
}

/// ===LuminanceBasedOutline, RadialBlur, CopyImage, === ///

///-------------------------------------------/// 
/// shaderをコンパイルする
///-------------------------------------------///
void Compiler::Initialize(DXCommon* dxCommon, PipelineType type) {
	// 対応するシェーダーの検索
	const auto& it = kShaderTable_.find(type);
	assert(it != kShaderTable_.end() && "Unknown PipelineType");

	// 対応する情報を取得
	const ShaderInfo& info = it->second;

	// VSシェーダのコンパイル
	objVSBlob_ = CompileShader(info.vsPath, L"vs_6_0",
		dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	// PSシェーダのコンパイル
	objPSBlob_ = CompileShader(info.psPath, L"ps_6_0",
		dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());

	// コンパイル結果を確認
	assert(objVSBlob_ && "Vertex Shader Compile Failed");
	assert(objPSBlob_ && "Pixel Shader Compile Failed");
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// ObjectVertexShaderBlobの取得
IDxcBlob* Compiler::GetObjVS() { return objVSBlob_.Get(); }
// ObjectPixelShaderBlobの取得
IDxcBlob* Compiler::GetObjPS() { return objPSBlob_.Get(); }

///-------------------------------------------/// 
/// CompileShader関数
///-------------------------------------------///
ComPtr<IDxcBlob> Compiler::CompileShader(
	const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {

	HRESULT hr;

	// ベースのファイルパスを設定
	const std::wstring& baseShaderPath = L"Resource/Shaders";
	const std::wstring& shaderFileName = baseShaderPath + L"/" + filePath;

	/// ===1. hlslファイルを読み込む=== ///
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", shaderFileName, profile)));

	// hlslファイルを読む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	hr = dxcUtils->LoadFile(shaderFileName.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	assert(SUCCEEDED(hr));

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	/// ===2. Compileする === ///
#ifdef _DEBUG
	LPCWSTR arguments[] = {
		shaderFileName.c_str(),
		L"-E", L"main",
		L"-T", profile,
		L"-Zi", L"-Qembed_debug",   // デバッグ情報あり
		L"-Od",                     // 最適化オフ
		L"-Zpr",
		L"-D", L"_DEBUG",           // _DEBUGマクロを定義
	};
#else
	LPCWSTR arguments[] = {
		shaderFileName.c_str(),
		L"-E", L"main",
		L"-T", profile,
		// デバッグ情報はなし
		L"-O2",                     // 高速化のための最適化
		L"-Zpr",
		L"-D", L"NDEBUG",           // NDEBUGマクロを定義（必要なら）
	};
#endif

	// 実際にShaderをコンパイルする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,          // 読み込んだファイル
		arguments,                    // コンパイルオプション
		_countof(arguments),          // コンパイルオプションの数
		includeHandler,               // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)   // コンパイル結果
	);

	// コンパイルエラーではなくDxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	/// ===3. 警告・エラーが出ていないかを確認する=== ///
	// 警告・エラーが出てたらログを出して止める
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());

		// 警告・エラーダメゼッタイ
		assert(false);
	}

	/// ===4. Compile結果を受け取ってます=== ///
	// コンパイル結果から実行用のバイナリ部分を取得
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", shaderFileName, profile)));

	// 実行用のバイナリを返却
	return shaderBlob;
}
