#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
// PipelineType
#include "Engine/DataInfo/PipelineStateObjectType.h"
// directX
#include <d3d12.h>
#include <dxcapi.h>
// c++
#include <string>
#include <unordered_map>

#pragma comment(lib, "dxcompiler.lib")

/// ===前方宣言=== ///
class DXCommon;

///=====================================================/// 
/// Compiler
///=====================================================///
class Compiler {
public:

	Compiler() = default;
	~Compiler();

	// shaderをコンパイルする
	void Initialize(DXCommon* dxCommon, PipelineType Type);

	/// ===ShaderBlobの取得=== ///
	// VS
	IDxcBlob* GetObjVS();
	// PS
	IDxcBlob* GetObjPS();

private:

	//　ObjectShader
	ComPtr<IDxcBlob> objVSBlob_;
	ComPtr<IDxcBlob> objPSBlob_;

private:
	/// <summary>
	/// CompileShader関数
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <param name="dxcUtils"></param>
	/// <param name="dxcCompiler"></param>
	/// <param name="includeHandler"></param>
	/// <returns></returns>
	ComPtr<IDxcBlob> CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,

		// Compilerに使用するProfile
		const wchar_t* profile,

		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

};