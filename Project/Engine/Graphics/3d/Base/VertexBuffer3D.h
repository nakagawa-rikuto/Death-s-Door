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

	// Resourceの生成
	// <param name="device">リソースを作成するための ID3D12Device オブジェクトへのポインター。</param>
	// <param name="sizeInBytes">作成するリソースのサイズ (バイト単位)。</param>
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

