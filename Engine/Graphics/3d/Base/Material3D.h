#pragma once
/// ===include=== ///
// Engine
#include "Engine/DataInfo/FunctionData.h"

///=====================================================/// 
/// マテリアル3D
///=====================================================///
class Material3D {
public:
	
	Material3D();
	~Material3D();

	/// <summary>
	///　リソースの生成
	/// </summary>
	void Create(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// マテリアルデータの書き込み
	/// </summary>
	/// <param name="material"></param>
	void WriteData(MaterialData3D* material);

	/// <summary>
	/// リソースの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetBuffer();

private:
	// リソース
	ComPtr<ID3D12Resource> buffer_;

	// データ
	MaterialData3D* data_;
};

