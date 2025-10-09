#pragma once
/// ===Include=== ///
#include "Engine/Graphics/3d/Model/Model.h"
#include "Engine/Graphics/3d/Model/AnimationModel.h"

/// ===オブジェクトのタイプ=== ///
enum class ObjectType {
	Model,
	AnimationModel
};

///=====================================================/// 
/// Object3D
///=====================================================///
class Object3d {
public:
	Object3d() = default;
	~Object3d();

	// 初期化
	void Init(ObjectType type, const std::string& modelName, LightType light = LightType::None);
	// 更新
	void Update();
	// 描画
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal);

public: /// ===親子関係=== ///
	// 親の設定
	void SetParent(ModelCommon* parent);
	// 親の解除
	void ClearParent();

public: /// ===Getter=== ///
	// モデル座標
	const Vector3& GetTranslate() const;
	// モデル回転
	const Quaternion& GetRotate() const;
	// モデル拡縮
	const Vector3& GetScale() const;
	// モデルカラー
	const Vector4& GetColor() const;
	// Object
	ModelCommon* GetModelCommon();

public: /// ===Setter=== ///
	// モデルTransform
	void SetTranslate(const Vector3& translate);
	void SetRotate(const Quaternion& rotate);
	void SetScale(const Vector3& scale);
	// モデルColor
	void SetColor(const Vector4& color);
	// Light
	void SetLight(LightType type);
	// LightData
	void SetLightData(LightInfo light);
	// 環境マップ
	void SetEnviromentMapData(bool flag, float string);
	// Animation
	void SetAnimation(const std::string& animationName, bool isLoop = true);

private: /// ===Variables(変数)=== ///

	/// ===クラス=== ///
	std::unique_ptr<Model> model_;
	std::unique_ptr<AnimationModel> animationModel_;

	/// ===モデルかアニメーションモデルかを判断するフラグ=== ///
	ObjectType type_ = ObjectType::Model;
};

