#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/FunctionData.h"

///=====================================================/// 
/// Transform2D
///=====================================================///
class Transform2D {
public:

	Transform2D();
	~Transform2D();

	/// <summary>
	/// Resourceの生成
	/// </summary>
	void Create(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// データの書き込み
	/// </summary>
	/// <param name="transform"></param>
	void WriteData(TransformationMatrix2D* data);

	/// <summary>
	/// リソースの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetBuffer();

	/// <summary>
	/// データの取得
	/// </summary>
	/// <returns></returns>
	TransformationMatrix2D* GetData();

	/// <summary>
	/// データのセット
	/// </summary>
	void SetData(TransformationMatrix2D* data);

private:

	ComPtr<ID3D12Resource> buffer_;

	TransformationMatrix2D* data_;
};

