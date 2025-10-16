#pragma once
/// ===include=== ///
// Engine
#include "Material2D.h"
#include "Transform2D.h"
//c++
#include <memory>

///=====================================================/// 
/// Object2D共通部
///=====================================================///
class Object2DCommon {
public:

	Object2DCommon();
	~Object2DCommon();

	// 初期化
	void MaterialInitialize(ID3D12Device* device, uint32_t Size);
	void WVPMatrixInitialize(ID3D12Device* device);
	// 描画準備
	void Bind(ID3D12GraphicsCommandList* commandList);

public:/// ===Setter=== ///
	// Mateial
	void SetMateiralData(const Vector4& color, const Matrix4x4& uvTransform);
	// WVP
	void SetWVPData(const Matrix4x4& WVP);

private:/// ===Variables(変数)=== ///

	// バッファリソース
	std::unique_ptr<Material2D> material_;
	std::unique_ptr<Transform2D> wvp_;

	// バッファリソース内のデータを指すポインタ
	MaterialData2D* materialData_ = nullptr;
	TransformationMatrix2D* wvpMatrixData_ = nullptr;
};


