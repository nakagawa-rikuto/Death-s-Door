#pragma once
// Common
#include "Engine/Graphics/3d/Model/ModelCommon.h"
// Data
#include "Engine/DataInfo/AnimationData.h"
#include "Engine/DataInfo/PipelineStateObjectType.h"

///=====================================================/// 
/// アニメーションモデル
///=====================================================///
class AnimationModel : public ModelCommon {
public:

	AnimationModel();
	~AnimationModel();

	// 初期化
	void Initialize(const std::string& filename, LightType type) override;
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
	// Animation
	void SetAnimation(const std::string& animationName, bool isLoop);

private: /// ===Variables(変数)=== ///

	/// ===Animation=== ///
	float animationTime_;
	std::map<std::string, Animation> animation_;
	std::string animationName_;
	Skeleton skeleton_;
	SkinCluster skinCluster_;
	bool isLoop_;

private: /// ===Functions(関数)=== ///

	// 任意の時刻を取得する関数(Vector3)
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	// 任意の時刻を取得する関数(Quaternion)
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
	// Nodeの階層構造からSkeletonを作る関数
	Skeleton CreateSkeleton(const Node& rootNode);
	// NodeからJointを作る関数
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	// Skeletonに対してAnimationの適用を行う関数
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);
	// Skeletonの更新関数
	void SkeletonUpdate(Skeleton& skeleton);
	// SkinClusterの生成関数
	SkinCluster CreateSkinCluster(const ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData);
	// SkinClusterの更新関数
	void SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton);
};

