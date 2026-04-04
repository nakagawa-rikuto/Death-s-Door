#include "CSCompiler.h"
// Engine
#include "Engine/Core/Logger.h"
#include "Engine/Core/DXCommon.h"
// C++
#include <cassert>
#include <format>

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	CSCompiler::~CSCompiler() {
		kernels_.clear();
		kernelNames_.clear();
	}


	///-------------------------------------------/// 
	/// テーブルの定義
	///-------------------------------------------///
	namespace {
		// 情報
		struct ShaderInfo {
			std::wstring csPath;
			std::vector<std::wstring> entryPoints; // 複数カーネルに対応
		};

		// テーブル本体
		const std::unordered_map<CSPipelineType, ShaderInfo> kCSShaderTable_ = {
			{ CSPipelineType::FFTOcean, { L"Ocean/FFTOcean.CS.hlsl",{
				L"InitSpectrum", L"UpdateSpectrum", L"BitReverseRows", L"ButterflyIFFT",
				L"BitReverseCols", L"SaveIFFTResult", L"AssembleDisplacement", L"ComputeNormalFoam",}}},
			{ CSPipelineType::Ripples,  { L"Ocean/Ripples.CS.hlsl", {L"SimulateRipple", L"AddRipplePoint"}}},
		};
	}


	///-------------------------------------------/// 
	///	初期化
	///-------------------------------------------///
	void CSCompiler::Initialize(DXCommon* dxCommon, CSPipelineType type) {
		// 対応するシェーダーの検索
		const auto& it = kCSShaderTable_.find(type);
		assert(it != kCSShaderTable_.end() && "Unknown CS PipelineType");

		// 対応する情報を取得
		const ShaderInfo& info = it->second;

		// 各エントリーポイントに対してカーネルをコンパイルし、マップに保存
		for (const auto& entry : info.entryPoints) {
			auto blob = CompileKernel(
				info.csPath, entry,
				dxCommon->GetDxcUtils(),
				dxCommon->GetDxcCompiler(),
				dxCommon->GetIncludeHandler());
			assert(blob && "CS Kernel Compile Failed");

			kernelNames_.push_back(entry);
			kernels_[entry] = std::move(blob);
		}
	}


	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// カーネル名を指定してBlobを取得する
	IDxcBlob* CSCompiler::GetKernel(const std::wstring& entryPoint) {
		auto it = kernels_.find(entryPoint);
		assert(it != kernels_.end() && "Kernel not found");
		return it->second.Get();
	}
	// 登録されている全カーネル名のリストを取得する
	const std::vector<std::wstring>& CSCompiler::GetKernelNames() const {
		return kernelNames_;
	}


	///-------------------------------------------/// 
	/// シェーダーカーネルをコンパイルします。
	///-------------------------------------------///
	ComPtr<IDxcBlob> CSCompiler::CompileKernel(const std::wstring& filePath, const std::wstring& entryPoint, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
		// シェーダーファイルのフルパスを構築
		const std::wstring baseShaderPath = L"Resource/Shaders";
		const std::wstring shaderFileName = baseShaderPath + L"/" + filePath;

		/// ===1. hlslファイルを読み込む=== ///
		// これからシェーダーをコンパイルする旨をログに出す
		Log(ConvertString(std::format(
			L"Begin CompileKernel, path:{}, entry:{}\n", shaderFileName, entryPoint)));

		// hlslファイルを読む
		ComPtr<IDxcBlobEncoding> shaderSource;
		HRESULT hr = dxcUtils->LoadFile(shaderFileName.c_str(), nullptr, &shaderSource);
		// 読めなかったら止める
		assert(SUCCEEDED(hr));

		// 読み込んだファイルの内容を設定する
		DxcBuffer shaderSourceBuffer;
		shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
		shaderSourceBuffer.Size = shaderSource->GetBufferSize();
		shaderSourceBuffer.Encoding = DXC_CP_UTF8;

		/// ===2. Compileする === ///
#ifdef _DEBUG
		LPCWSTR arguments[] = {
			shaderFileName.c_str(),
			L"-E", entryPoint.c_str(),
			L"-T", L"cs_6_0",
			L"-Zi", L"-Qembed_debug",   // デバッグ情報あり
			L"-Od",						// 最適化オフ
			L"-Zpr",
			L"-D", L"_DEBUG",			// _DEBUGマクロを定義
		};
#else
		LPCWSTR arguments[] = {
			shaderFileName.c_str(),
			L"-E", entryPoint.c_str(),
			L"-T", L"cs_6_0",
			L"-O2",						// 高速化のための最適化
			L"-Zpr",
			L"-D", L"NDEBUG",			// NDEBUGマクロを定義（必要なら）
		};
#endif

		// 実際にShaderをコンパイルする
		ComPtr<IDxcResult> shaderResult;
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
		ComPtr<IDxcBlobUtf8> shaderError;
		shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
		if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
			Log(shaderError->GetStringPointer());

			// 警告・エラーダメゼッタイ
			assert(false);
		}

		/// ===4. Compile結果を受け取ってます=== ///
		// コンパイル結果から実行用のバイナリ部分を取得
		ComPtr<IDxcBlob> shaderBlob;
		hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
		assert(SUCCEEDED(hr));

		// 成功したログを出す
		Log(ConvertString(std::format(
			L"Compile Succeeded, path:{}, entry:{}\n", shaderFileName, entryPoint)));

		// 実行用のバイナリを返却
		return shaderBlob;
	}
}
