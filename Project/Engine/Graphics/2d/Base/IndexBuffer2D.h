#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/FunctionData.h"

///=====================================================/// 
/// IndexBuffer2D
///=====================================================///
class IndexBuffer2D {
public:

	IndexBuffer2D();
	~IndexBuffer2D();

	/// <summary>
	/// Resourceの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sizeInBytes"></param>
	void Create(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// リソースの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetBuffer();

	/// <summary>
	/// データの取得
	/// </summary>
	/// <returns></returns>
	uint32_t* GetData();

private:

	ComPtr<ID3D12Resource> buffer_;

	uint32_t* data_;
};

