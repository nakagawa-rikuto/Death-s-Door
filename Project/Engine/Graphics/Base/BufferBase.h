#pragma once
/// ===include=== ///
#include "Engine/DataInfo/FunctionData.h"

///=====================================================/// 
/// Light
///=====================================================///
class BufferBase {
public:
	BufferBase() = default;
	~BufferBase();

	/// <summary>
	/// 生成
	/// </summary>
	void Create(ID3D12Device* device, size_t sizeInBytes);

public:/// ===Getter=== ///

	/// <summary>
	/// リソースの取得
	/// </summary>
	ID3D12Resource* GetBuffer();

private:/// ===Variables(変数)=== ///

	ComPtr<ID3D12Resource> buffer_;
};
