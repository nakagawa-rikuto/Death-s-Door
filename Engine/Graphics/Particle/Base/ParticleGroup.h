#pragma once
/// ===incled=== ///
#include "ParticleSetUp.h"
#include <list>
#include <random>

/// ===Camera=== ///
class Camera;

///=====================================================/// 
/// パーティクルのグループ
///=====================================================///
class ParticleGroup {
public:

	ParticleGroup() = default;
	virtual ~ParticleGroup();

	// 初期化
	virtual void Initialze(const Vector3& translate, Camera* camera) = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw(BlendMode mode);
	
	// 初期化
	void InstancingInit(const std::string& modelName, const Vector3& translate, const uint32_t maxInstance, Camera* camera, shapeType type = shapeType::kNone);
	// 更新
	void InstancingUpdate(std::list<ParticleData>::iterator it);
	// 生存判定
	bool IsFinish();

public: /// ===Setter=== ///
	// Texture
	void SetTexture(const std::string& fileName);

protected:
	/// ===Emitter=== ///
	struct Group {
		std::unique_ptr<ParticleSetUp> particle; // パーティクルグループ
		std::list<ParticleData> particles; // パーティクルのリスト
		EulerTransform transform; // エミッタのTransform
		EulerTransform cameraTransform; // カメラのTransform
		uint32_t maxInstance; // パーティクルの最大数
		uint32_t numInstance; // インスタンス数
		uint32_t frequencyCount; // パーティクルの発生頻度のカウント
		float frequency; // パーティクルの発生頻度
		float frequencyTime; // パーティクルの発生頻度の時間
		Camera* camera = nullptr;
	};

	// エミッタ
	Group group_{};
	// 時間の進む速度
	const float kDeltaTime_ = 1.0f / 60.0f;

	/// ===プライベートフィールド=== ///
	std::mt19937 randomEngine_; // 乱数生成器

public:

	// クローン 
	virtual std::unique_ptr<ParticleGroup> Clone() = 0;
	// パーティクルの生成
	virtual ParticleData MakeParticle(std::mt19937& randomEngine, const Vector3& translate) = 0;
	// パーティクル発生
	virtual std::list<ParticleData> Emit(const Group& group, std::mt19937& randomEngine) = 0;
};

