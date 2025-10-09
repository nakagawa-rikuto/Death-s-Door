#include "InputLayout.h"

///-------------------------------------------/// 
/// テーブルで使用するDescを作成し設定
///-------------------------------------------///
namespace {
	/// ===配列が２つ=== ///
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs2[2] = {};
	// デスクに対応した設定
	void InitLayout2Array() {
		inputElementDescs2[0].SemanticName = "POSITION";
		inputElementDescs2[0].SemanticIndex = 0;
		inputElementDescs2[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs2[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs2[1].SemanticName = "TEXCOORD";
		inputElementDescs2[1].SemanticIndex = 0;
		inputElementDescs2[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs2[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	}

	/// ===Line3D用=== ///
	static D3D12_INPUT_ELEMENT_DESC inputElementDescsLine[2] = {};
	// デスクに対応した設定
	void InitLayoutLine3D() {
		inputElementDescsLine[0].SemanticName = "POSITION";
		inputElementDescsLine[0].SemanticIndex = 0;
		inputElementDescsLine[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescsLine[0].InputSlot = 0;
		inputElementDescsLine[0].AlignedByteOffset = 0;
		inputElementDescsLine[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementDescsLine[0].InstanceDataStepRate = 0;

		inputElementDescsLine[1].SemanticName = "COLOR";
		inputElementDescsLine[1].SemanticIndex = 0;
		inputElementDescsLine[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescsLine[1].InputSlot = 0;
		inputElementDescsLine[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescsLine[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementDescsLine[1].InstanceDataStepRate = 0;
	}

	/// ===配列が3つ=== ///
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs3[3] = {};
	void InitLayout3Array() {
		inputElementDescs3[0].SemanticName = "POSITION";
		inputElementDescs3[0].SemanticIndex = 0;
		inputElementDescs3[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs3[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs3[1].SemanticName = "TEXCOORD";
		inputElementDescs3[1].SemanticIndex = 0;
		inputElementDescs3[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs3[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs3[2].SemanticName = "NORMAL";
		inputElementDescs3[2].SemanticIndex = 0;
		inputElementDescs3[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs3[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs3[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	}

	/// ===配列が5つ=== ///
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs5[5] = {};
	void InitLayout5Array() {
		inputElementDescs5[0].SemanticName = "POSITION";
		inputElementDescs5[0].SemanticIndex = 0;
		inputElementDescs5[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs5[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs5[1].SemanticName = "TEXCOORD";
		inputElementDescs5[1].SemanticIndex = 0;
		inputElementDescs5[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs5[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs5[2].SemanticName = "NORMAL";
		inputElementDescs5[2].SemanticIndex = 0;
		inputElementDescs5[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs5[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs5[3].SemanticName = "WEIGHT";
		inputElementDescs5[3].SemanticIndex = 0;
		inputElementDescs5[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // float4
		inputElementDescs5[3].InputSlot = 1; // 一番目のslotのVBVのことだと伝える
		inputElementDescs5[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs5[4].SemanticName = "INDEX";
		inputElementDescs5[4].SemanticIndex = 0;
		inputElementDescs5[4].Format = DXGI_FORMAT_R32G32B32A32_SINT; // int4
		inputElementDescs5[4].InputSlot = 1; // 一番目のslotのVBVのことだと伝える
		inputElementDescs5[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	}

	// 情報
	struct LayoutInfo {
		const D3D12_INPUT_ELEMENT_DESC* elements;
		UINT numElements;
	};

	// テーブル
	const std::unordered_map<PipelineType, LayoutInfo> kLayoutTable_ = [] {
		InitLayout2Array();
		InitLayoutLine3D();
		InitLayout3Array();
		InitLayout5Array();
		// タイプに応じて設定
		return std::unordered_map<PipelineType, LayoutInfo>{
			{ PipelineType::ForGround2D,  { inputElementDescs2,      _countof(inputElementDescs2) } },
			{ PipelineType::BackGround2D, { inputElementDescs2,      _countof(inputElementDescs2) } },
			{ PipelineType::Obj3D,        { inputElementDescs3,      _countof(inputElementDescs3) } },
			{ PipelineType::PrimitiveSkyBox,       { inputElementDescs3,      _countof(inputElementDescs3) } },
			{ PipelineType::Particle,     { inputElementDescs3,      _countof(inputElementDescs3) } },
			{ PipelineType::Skinning3D,   { inputElementDescs5,		 _countof(inputElementDescs5) } },
			{ PipelineType::Line3D,       { inputElementDescsLine,   _countof(inputElementDescsLine) } },
			{ PipelineType::OffScreen,    { nullptr,                  0 } },
			{ PipelineType::Grayscale,    { nullptr,                  0 } },
			{ PipelineType::Vignette,     { nullptr,                  0 } },
			{ PipelineType::Dissolve,     { nullptr,                  0 } },
			{ PipelineType::BoxFilter3x3, { nullptr,                  0 } },
			{ PipelineType::BoxFilter5x5, { nullptr,                  0 } },
			{ PipelineType::RadiusBlur,   { nullptr,                  0 } },
			{ PipelineType::OutLine,      { nullptr,                  0 } },
		};
	}();
}



///-------------------------------------------/// 
/// InputLayoutの生成
///-------------------------------------------///
void InputLayout::Create(PipelineType Type) {

	auto it = kLayoutTable_.find(Type);
	if (it != kLayoutTable_.end()) {
		inputLayoutDesc_.pInputElementDescs = it->second.elements;
		inputLayoutDesc_.NumElements = it->second.numElements;
	} else {
		inputLayoutDesc_.pInputElementDescs = nullptr;
		inputLayoutDesc_.NumElements = 0;
	}
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
D3D12_INPUT_LAYOUT_DESC InputLayout::GetInputLayout() const { return inputLayoutDesc_; }
