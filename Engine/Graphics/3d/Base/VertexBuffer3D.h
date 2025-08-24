#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/FunctionData.h"

///=====================================================/// 
/// VertexBuffer3D
///=====================================================///
class VertexBuffer3D {
public:

	VertexBuffer3D();
	~VertexBuffer3D();

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
	VertexData3D* GetData();

private:

	ComPtr<ID3D12Resource> buffer_;

	VertexData3D* data_;
};

