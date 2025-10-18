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

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="type">初期化するオブジェクトの種類を指定します。</param>
	/// <param name="modelName">使用するモデルの名前（識別またはロードに使われる文字列）。</param>
	/// <param name="light">照明の種類を指定します。省略時は LightType::None（照明なし）が使用されます。</param>
	void Init(ObjectType type, const std::string& modelName, LightType light = LightType::None);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。省略した場合は BlendMode::KBlendModeNormal が使用される。</param>
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal);

public: /// ===親子関係=== ///
	/// <summary>
	/// ModelCommon 型の親オブジェクトを設定
	/// </summary>
	/// <param name="parent">設定する親オブジェクトへのポインタ。</param>
	void SetParent(ModelCommon* parent);

	/// <summary>
	/// 親オブジェクトへの参照をクリア
	/// </summary>
	void ClearParent();

public: /// ===Getter=== ///
	// Transform（位置、回転、拡縮）を取得
	const QuaternionTransform& GetTransform() const;
	// Color（色）を取得
	const Vector4& GetColor() const;
	// ModelCommonオブジェクトへのポインターを取得
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

