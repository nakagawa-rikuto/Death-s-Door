#include "CSRootSignature.h"
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

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	CSRootSignature::~CSRootSignature() {
		rootSignature_.Reset();
	}

	///-------------------------------------------/// 
	/// パイプラインの設定
	///-------------------------------------------///
	namespace {

		/// ===CSFFTOcean=== ///
		ComPtr<ID3D12RootSignature> TypeFFTOcean(ID3D12Device* device) {
			// UAV用のDescriptorRange (u0〜u7)
			D3D12_DESCRIPTOR_RANGE uavRange = {};
			uavRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			uavRange.NumDescriptors = 8; // u0〜u7
			uavRange.BaseShaderRegister = 0; // u0から開始
			uavRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// SRV用のDescriptorRange (t0〜t2)
			D3D12_DESCRIPTOR_RANGE srvRange = {};
			srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			srvRange.NumDescriptors = 3; // t0〜t2
			srvRange.BaseShaderRegister = 0; // t0から開始
			srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// SRV用のRippleMap (t3)
			D3D12_DESCRIPTOR_RANGE rippleRange = {};
			rippleRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			rippleRange.NumDescriptors = 1; // t0〜t2
			rippleRange.BaseShaderRegister = 3; // t0から開始
			rippleRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


			/// RootParameterの生成
			D3D12_ROOT_PARAMETER rootParameters[5] = {};
			// パラメータ0: CBV - OceanParams (b0)
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameters[0].Descriptor.ShaderRegister = 0;
			// パラメータ1: CBV - ButterflyParams (b1)
			rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameters[1].Descriptor.ShaderRegister = 1;
			// パラメータ2: UAV
			rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[2].DescriptorTable.pDescriptorRanges = &uavRange;
			rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			// パラメータ3: SRV
			rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[3].DescriptorTable.pDescriptorRanges = &srvRange;
			rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			// パラメータ4: SRV(t3)
			rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[4].DescriptorTable.pDescriptorRanges = &rippleRange;
			rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			/// RootSignatureの生成
			D3D12_ROOT_SIGNATURE_DESC desc{};
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
			desc.pParameters = rootParameters;
			desc.NumParameters = _countof(rootParameters);
			desc.pStaticSamplers = nullptr;
			desc.NumStaticSamplers = 0;

			/// シリアライズ作成
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

			// RootSignatureの生成
			ComPtr<ID3D12RootSignature> rootSignature;
			hr = device->CreateRootSignature(
				0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
				IID_PPV_ARGS(&rootSignature));
			assert(SUCCEEDED(hr));
			return rootSignature;
		}

		/// ===Riples=== ///
		ComPtr<ID3D12RootSignature> TypeRipples(ID3D12Device* device) {
			// UAV用のDescriptorRang
			D3D12_DESCRIPTOR_RANGE uavRange = {};
			uavRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			uavRange.NumDescriptors = 2; // u0〜u1
			uavRange.BaseShaderRegister = 0; // u0から開始
			uavRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			/// RootParameterの生成
			D3D12_ROOT_PARAMETER rootParameters[3] = {};
			// パラメータ0: CBV - RippleParams (b0)
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameters[0].Descriptor.ShaderRegister = 0;
			// パラメーター1:CBY - RippleInjection (b1)
			rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameters[1].Descriptor.ShaderRegister = 1;
			// パラメータ2: UAV
			rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[2].DescriptorTable.pDescriptorRanges = &uavRange;
			rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			/// RootSignatureの生成
			D3D12_ROOT_SIGNATURE_DESC desc{};
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
			desc.pParameters = rootParameters;
			desc.NumParameters = _countof(rootParameters);
			desc.pStaticSamplers = nullptr;
			desc.NumStaticSamplers = 0;

			/// シリアライズ作成
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

			// RootSignatureの生成
			ComPtr<ID3D12RootSignature> rootSignature;
			hr = device->CreateRootSignature(
				0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
				IID_PPV_ARGS(&rootSignature));
			assert(SUCCEEDED(hr));
			return rootSignature;
		}

		/// ===マップに登録=== ///
		using RootSigGenerator = std::function<ComPtr<ID3D12RootSignature>(ID3D12Device*)>;
		const std::unordered_map<CSPipelineType, RootSigGenerator> kRootSignatureTable_ = {
			{CSPipelineType::FFTOcean,  TypeFFTOcean},
			{CSPipelineType::Ripples,	TypeRipples},
		};
	}

	///-------------------------------------------/// 
	/// ルートシグネイチャの生成
	///-------------------------------------------///
	void CSRootSignature::Create(DXCommon* dxCommon, CSPipelineType Type) {
		auto it = kRootSignatureTable_.find(Type);
		if (it != kRootSignatureTable_.end()) {
			// 直接 RootSignature を作って返す構成に変更
			rootSignature_ = it->second(dxCommon->GetDevice());
		} else {
			assert(false && "Unknown Compute PipelineType in CSRootSignature");
		}
	}

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	ID3D12RootSignature* CSRootSignature::GetRootSignature() const { return rootSignature_.Get(); }
}