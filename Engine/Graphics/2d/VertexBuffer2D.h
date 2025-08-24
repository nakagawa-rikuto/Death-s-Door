#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/FunctionData.h"

///=====================================================/// 
/// VertexBuffer2D
///=====================================================///
class VertexBuffer2D {
public:

	VertexBuffer2D();
	~VertexBuffer2D();

	/// <summary>
	/// Resourceの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sizeInBytes"></param>
	void Create(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// データの書き込み
	/// </summary>
	/// <param name="transform"></param>
	void WriteData(VertexData2D* data);

	/// <summary>
	/// リソースの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetBuffer();

	/// <summary>
	/// データの取得
	/// </summary>
	/// <returns></returns>
	VertexData2D* GetData();

	/// <summary>
	/// データのセット
	/// </summary>
	void SetData(VertexData2D* data);

private:

	ComPtr<ID3D12Resource> buffer_;

	VertexData2D* data_;
};

