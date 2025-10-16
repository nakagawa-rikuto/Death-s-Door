#pragma once
/// ===include=== ///
// Common
#include "Engine/Graphics/3d/Model/ModelCommon.h"

///=====================================================/// 
/// モデル
///=====================================================///
class Model : public ModelCommon {
public: /// ===基本的な関数=== ///

	Model();
	~Model();

	// 初期化
	void Initialize(const std::string& filename, LightType type = LightType::None) override; // オブジェクトを読み込まない場合の初期化
	// 更新
	void Update() override;
	// 描画
	void Draw(BlendMode mode) override;

public: /// ===Getter=== ///
	// モデル座標
	const Vector3& GetTranslate() const;
	// モデル回転
	const Quaternion& GetRotate() const;
	// モデル拡縮
	const Vector3& GetScale() const;
	// モデルカラー
	const Vector4& GetColor() const;

public: /// ===Setter=== ///
	// モデルTransform
	void SetTranslate(const Vector3& position);
	void SetRotate(const Quaternion& rotate);
	void SetScale(const Vector3& scale);
	// モデルカラー
	void SetColor(const Vector4& color);
	// Light
	void SetLight(LightType type);
	// LightData
	void SetLightData(LightInfo light);
	// 環境マップ
	void SetEnviromentMapData(bool flag, float string);
};

