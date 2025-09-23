#include "RingParticle.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
RingParticle::~RingParticle() {
	group_.particles.clear();
	group_.particle.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void RingParticle::Initialze(const Vector3& translate) {
	/// ===初期化=== ///
	ParticleGroup::InstancingInit("Particle", translate, 300, shapeType::kCircle);
	/// ===フラグと設定の初期化=== ///
	hasExploded_ = false;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void RingParticle::Update() {
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
		}

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
void RingParticle::Draw(BlendMode mode) {
	if (group_.numInstance > 0) {
		ParticleGroup::Draw(mode);
	}
}

///-------------------------------------------/// 
/// クローン
///-------------------------------------------///
std::unique_ptr<ParticleGroup> RingParticle::Clone() {
	// 新しいインスタンスを作成
	std::unique_ptr<RingParticle> clone = std::make_unique<RingParticle>();

	// 初期化は Emit 側で呼ばれるので不要
	return clone;
}

///-------------------------------------------/// 
/// パーティクルの生成
///-------------------------------------------///
ParticleData RingParticle::MakeParticle(std::mt19937& randomEngine, const Vector3& translate) {

	std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> distScale(0.1f, 3.0f);

	ParticleData particleData;
	particleData.transform.scale = { 0.1f, 1.0f, distRotate(randomEngine) };
	particleData.transform.rotate = { 0.0f, 0.0f, distRotate(randomEngine) };
	particleData.transform.translate = translate;
	particleData.velocity = { 0.0f, 0.0f, 0.0f };
	particleData.color = { 1.0f, 1.0f, 0.0f, 1.0f };
	particleData.currentTime = 0;
	particleData.lifeTime = 1.0f; // 寿命を1秒に設定

	return particleData;
}

///-------------------------------------------/// 
/// エミット
///-------------------------------------------///
std::list<ParticleData> RingParticle::Emit(const Group& group, std::mt19937& randomEngine) {
	std::list<ParticleData> particles;
	for (uint32_t i = 0; i < group_.maxInstance; ++i) {
		particles.push_back(MakeParticle(randomEngine_, group.transform.translate));
	}
	return particles;
}
