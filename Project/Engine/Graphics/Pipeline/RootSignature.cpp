#include "RootSignature.h"
// Engine
#include "Engine/Core/Logger.h"
#include "Engine/Core/DXCommon.h"
// c++
#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>
#include <format>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
RootSignature::~RootSignature() {
	rootSignature_.Reset();
}

///-------------------------------------------/// 
/// パイプラインの設定
///-------------------------------------------///
namespace {

	/// ===2D=== ///
	ComPtr<ID3D12RootSignature> Type2D(ID3D12Device* device) {
		// DescriptroRangeの生成
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0; // 0から始める
		descriptorRange[0].NumDescriptors = 1; // 数は1つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[3] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号の指定

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号の指定

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数


		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の長さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===Line3D=== ///
	ComPtr<ID3D12RootSignature> TypeLine3D(ID3D12Device* device) {
		D3D12_ROOT_PARAMETER rootParameters[1] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[0].Descriptor.ShaderRegister = 0;

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters;
		desc.NumParameters = _countof(rootParameters);
		desc.pStaticSamplers = nullptr;
		desc.NumStaticSamplers = 0;

		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));
		return rootSignature;
	}

	/// ===3D=== ///
	ComPtr<ID3D12RootSignature> Type3D(ID3D12Device* device) {
		// DescriptroRangeの生成				
		// t0用
		D3D12_DESCRIPTOR_RANGE descriptorRange0 = {};
		descriptorRange0.BaseShaderRegister = 0; // t0
		descriptorRange0.NumDescriptors = 1; // 数は1つ
		descriptorRange0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange0.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
		// t1用
		D3D12_DESCRIPTOR_RANGE descriptorRange1 = {};
		descriptorRange1.BaseShaderRegister = 1; // t0
		descriptorRange1.NumDescriptors = 1; // 数は1つ
		descriptorRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[9] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使用
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使用
		rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使用
		rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使用
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange0; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[3].DescriptorTable.pDescriptorRanges = &descriptorRange1; // Tableの中身の配列を指定
		rootParameters[3].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[4].Descriptor.ShaderRegister = 1; // レジスタ番号1を使用

		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[5].Descriptor.ShaderRegister = 2; // レジスタ番号2を使用

		rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[6].Descriptor.ShaderRegister = 3; // レジスタ番号3を使用

		rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[7].Descriptor.ShaderRegister = 4; // レジスタ番号4を使用

		rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[8].Descriptor.ShaderRegister = 5; // レジスタ番号5を使用


		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0; // S0
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の高さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===Particle=== ///
	ComPtr<ID3D12RootSignature> TypeParticle(ID3D12Device* device) {
		// DescriptroRangeの生成
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0; // 0から始める
		descriptorRange[0].NumDescriptors = 1; // 数は1つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[3] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0を使う

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
		rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange; // テーブルの中身の配列を指定
		rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // テーブルで利用可能

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数


		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の高さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===Skinning3D=== ///
	ComPtr<ID3D12RootSignature> TypeSkinning3D(ID3D12Device* device) {
		// DescriptroRangeの生成
		// t0用
		D3D12_DESCRIPTOR_RANGE descriptorRange0 = {};
		descriptorRange0.BaseShaderRegister = 0; // t0
		descriptorRange0.NumDescriptors = 1; // 数は1つ
		descriptorRange0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange0.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
		// t1用
		D3D12_DESCRIPTOR_RANGE descriptorRange1 = {};
		descriptorRange1.BaseShaderRegister = 1; // t0
		descriptorRange1.NumDescriptors = 1; // 数は1つ
		descriptorRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[10] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使用
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使用
		rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使用
		rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使用
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange0; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[3].DescriptorTable.pDescriptorRanges = &descriptorRange1; // Tableの中身の配列を指定
		rootParameters[3].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[4].Descriptor.ShaderRegister = 1; // レジスタ番号1を使用

		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[5].Descriptor.ShaderRegister = 2; // レジスタ番号2を使用

		rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[6].Descriptor.ShaderRegister = 3; // レジスタ番号3を使用

		rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[7].Descriptor.ShaderRegister = 4; // レジスタ番号4を使用

		rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[8].Descriptor.ShaderRegister = 5; // レジスタ番号5を使用

		rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[9].DescriptorTable.pDescriptorRanges = &descriptorRange0; // Tableの中身の配列を指定
		rootParameters[9].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数


		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の高さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===OffScreen=== ///
	ComPtr<ID3D12RootSignature> TypeOffScreen(ID3D12Device* device) {
		// DescriptroRangeの生成
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0; // 0から始める
		descriptorRange[0].NumDescriptors = 1; // 数は1つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[1]{};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
		rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の長さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===OffScreen(Vignette)=== ///
	ComPtr<ID3D12RootSignature> TypeOffScreenOneBuffer(ID3D12Device* device) {
		// DescriptroRangeの生成
		D3D12_DESCRIPTOR_RANGE descriptorRange = {};
		descriptorRange.BaseShaderRegister = 0; // 0から始める
		descriptorRange.NumDescriptors = 1; // 数は1つ
		descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[2]{};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange; // Tableの中身の配列を指定
		rootParameters[0].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使用
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使用
		rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の長さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===OffScreen(Dissolve)=== ///
	ComPtr<ID3D12RootSignature> TypeOffScreenDissolve(ID3D12Device* device) {
		// DescriptroRangeの生成
		D3D12_DESCRIPTOR_RANGE descriptorRange0 = {};
		descriptorRange0.BaseShaderRegister = 0; // 0から始める
		descriptorRange0.NumDescriptors = 1; // 数は1つ
		descriptorRange0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange0.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
		// t1用
		D3D12_DESCRIPTOR_RANGE descriptorRange1 = {};
		descriptorRange1.BaseShaderRegister = 1; // t0
		descriptorRange1.NumDescriptors = 1; // 数は1つ
		descriptorRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[3]{};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange0; // Tableの中身の配列を指定
		rootParameters[0].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRange1; // Tableの中身の配列を指定
		rootParameters[1].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使用
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使用
		rootParameters[2].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の長さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===OffScreen(ShatterGlass)=== ///
	ComPtr<ID3D12RootSignature> TypeOffScreenShatterGlass(ID3D12Device* device) {
		// DescriptorRangeの生成
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0; // 0から始める (t0)
		descriptorRange[0].NumDescriptors = 1; // 数は1つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[2]{}; // 2つに増やす

		// [0] SRV (テクスチャ)
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
		rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

		// [1] CBV (定数バッファ) - 追加
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを直接指定
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0; // b0
		rootParameters[1].Descriptor.RegisterSpace = 0; // space0

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0; // s0
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の長さ (2に変更)
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}

	/// ===SkyBox=== ///
	ComPtr<ID3D12RootSignature> TypeSkyBox(ID3D12Device* device) {
		// DescriptroRangeの生成				
		// t0用
		D3D12_DESCRIPTOR_RANGE descriptorRange0 = {};
		descriptorRange0.BaseShaderRegister = 0; // t0
		descriptorRange0.NumDescriptors = 1; // 数は1つ
		descriptorRange0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
		descriptorRange0.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

		// RootParameterの生成
		D3D12_ROOT_PARAMETER rootParameters[9] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使用
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使用
		rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使用
		rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0を使用

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使用
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange0; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0; // S0
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		desc.NumParameters = _countof(rootParameters); // 配列の高さ
		desc.pStaticSamplers = staticSamplers;
		desc.NumStaticSamplers = _countof(staticSamplers);

		// --- シリアライズ & 作成 ---
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		return rootSignature;
	}


	/// ===マップに登録=== ///
	using RootSigGenerator = std::function<ComPtr<ID3D12RootSignature>(ID3D12Device*)>;
	const std::unordered_map<PipelineType, RootSigGenerator> kRootSignatureTable_ = {
		{ PipelineType::Obj3D,				Type3D },
		{ PipelineType::PrimitiveSkyBox,	Type3D },
		{ PipelineType::ForGround2D,		Type2D },
		{ PipelineType::BackGround2D,		Type2D },
		{ PipelineType::Particle,			TypeParticle },
		{ PipelineType::Skinning3D,			TypeSkinning3D  },
		{ PipelineType::Line3D,				TypeLine3D },
		{ PipelineType::OffScreen,			TypeOffScreen },
		{ PipelineType::Grayscale,			TypeOffScreen },
		{ PipelineType::Vignette ,			TypeOffScreenOneBuffer },
		{ PipelineType::Dissolve,           TypeOffScreenDissolve },
		{ PipelineType::BoxFilter3x3,		TypeOffScreen },
		{ PipelineType::BoxFilter5x5,		TypeOffScreen },
		{ PipelineType::RadiusBlur,			TypeOffScreenOneBuffer },
		{ PipelineType::OutLine,			TypeOffScreen },
		{ PipelineType::ShatterGlass,       TypeOffScreenShatterGlass },
	};
}

///-------------------------------------------/// 
/// ルートシグネイチャの生成
///-------------------------------------------///
void RootSignature::Create(DXCommon* dxCommon, PipelineType Type) {
	auto it = kRootSignatureTable_.find(Type);
	if (it != kRootSignatureTable_.end()) {
		// 直接 RootSignature を作って返す構成に変更
		rootSignature_ = it->second(dxCommon->GetDevice());
	} else {
		assert(false && "Unknown PipelineType in RootSignature");
	}
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12RootSignature* RootSignature::GetRootSignature() const { return rootSignature_.Get(); }

