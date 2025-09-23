#include "CylinderParticle.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
CylinderParticle::~CylinderParticle() {
	group_.particles.clear();
	group_.particle.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void CylinderParticle::Initialze(const Vector3& translate) {
	/// ===初期化=== ///
	ParticleGroup::InstancingInit("plane", translate, 100, shapeType::kCylinder);
	/// ===フラグと設定の初期化=== ///
	hasExploded_ = false;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void CylinderParticle::Update() {
	if (!hasExploded_) {
		// パーティクル発生
		group_.particles.splice(group_.particles.end(), Emit(group_, randomEngine_));
		hasExploded_ = true;
	}

	// インスタンス数をリセット
	group_.numInstance = 0;

	// パーティクル更新
	for (auto particleIterator = group_.particles.begin(); particleIterator != group_.particles.end(); ) {
		// 時間更新
		particleIterator->currentTime += kDeltaTime_;

		// パーティクルの生存判定
		if (particleIterator->currentTime >= particleIterator->lifeTime) {
			particleIterator = group_.particles.erase(particleIterator);
			continue;
		};

		// アルファ値の更新
		float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
		particleIterator->color.w = alpha;

		// パーティクルをGPUに送る準備
		ParticleGroup::InstancingUpdate(particleIterator);
		++particleIterator;
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void CylinderParticle::Draw(BlendMode mode) {
	if (group_.numInstance > 0) {
		ParticleGroup::Draw(mode);
	}
}

///-------------------------------------------/// 
/// クローン
///-------------------------------------------///
std::unique_ptr<ParticleGroup> CylinderParticle::Clone() {
	// 新しいインスタンスを作成
	std::unique_ptr<CylinderParticle> clone = std::make_unique<CylinderParticle>();

	// 初期化は Emit 側で呼ばれるので不要
	return clone;
}

///-------------------------------------------/// 
/// パーティクルの生成
///-------------------------------------------///
ParticleData CylinderParticle::MakeParticle(std::mt19937& randomEngine, const Vector3& translate) {

	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> distScale(0.1f, 2.0f);

    float angle = angleDist(randomEngine);
	float scale = distScale(randomEngine);

    float x = std::cos(angle) * radius_;
    float z = std::sin(angle) * radius_;

    ParticleData particleData;
    particleData.transform.translate = translate + Vector3{ x, 0.0f, z }; // 円周に配置

    particleData.transform.scale = { 1.0f, scale, 1.0f }; // 細長く縦に伸びた円柱

    // Y軸回転なし（すでに円周に沿って配置されているので回転は基本不要）
    particleData.transform.rotate = { 0.0f, 0.0f, 0.0f };

    // 静止 or わずかに上昇させたい場合
    particleData.velocity = { 0.0f, 0.0f, 0.0f };

    particleData.color = { 0.0f, 0.0f, 1.0f, 1.0f };
    particleData.currentTime = 0.0f;
    particleData.lifeTime = 100.0f; // 1秒で消える

	

    return particleData;
}

///-------------------------------------------/// 
/// エミット
///-------------------------------------------///
std::list<ParticleData> CylinderParticle::Emit(const Group& group, std::mt19937& randomEngine) {
	std::list<ParticleData> particles;
	for (uint32_t i = 0; i < group_.maxInstance; ++i) {
		particles.push_back(MakeParticle(randomEngine_, group.transform.translate));
	}
	return particles;
}
