#include "HitEffectParticle.h" 
// Math
#include "Math/sMath.h"
// c++
#include <numbers>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
HitEffectParticle::~HitEffectParticle() {
    group_.particles.clear();
    group_.particle.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void HitEffectParticle::Initialze(const Vector3& translate) {
    /// ===初期化=== ///
    ParticleGroup::InstancingInit("ParticlePlane", translate, 10);

	/// ===フラグと設定の初期化=== ///
    hasExploded_ = false;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void HitEffectParticle::Update() {
    if (!hasExploded_) {
        group_.particles.splice(group_.particles.end(), Emit(group_, randomEngine_)); // 発生処理
        hasExploded_ = true;
    }

    // パーティクルの更新
    group_.numInstance = 0; // インスタンス数をリセット
    for (std::list<ParticleData>::iterator particleIterator = group_.particles.begin(); particleIterator != group_.particles.end();) {

		// 寿命が尽きたパーティクルを削除
        if (particleIterator->lifeTime <= particleIterator->currentTime) {
            particleIterator = group_.particles.erase(particleIterator); // 寿命が尽きたパーティクルを削除
            continue;
        }

        // 時間の更新
        particleIterator->currentTime += kDeltaTime_;

        // アルファ値の更新
        float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
        particleIterator->color.w = alpha;

        /// ===ParticleEmitterの更新=== ///
        ParticleGroup::InstancingUpdate(particleIterator);
        ++particleIterator; // 次のイテレータに進める
    }
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void HitEffectParticle::Draw(BlendMode mode) {
    if (group_.numInstance > 0) {
        ParticleGroup::Draw(mode);
    }
}

///-------------------------------------------/// 
/// クローン
///-------------------------------------------///
std::unique_ptr<ParticleGroup> HitEffectParticle::Clone() {
    // 新しいインスタンスを作成
    std::unique_ptr<HitEffectParticle> clone = std::make_unique<HitEffectParticle>();

    // 初期化は Emit 側で呼ばれるので不要
    return clone;
}

///-------------------------------------------/// 
/// パーティクルの生成
///-------------------------------------------///
ParticleData HitEffectParticle::MakeParticle(std::mt19937& randomEngine, const Vector3& translate) {

    std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	ParticleData particleData;
    particleData.transform.scale = { 0.05f, distRotate(randomEngine), 1.0f };
	particleData.transform.rotate = { 0.0f, 0.0f, distRotate(randomEngine)};
	particleData.transform.translate = translate;
    particleData.velocity = { 0.0f, 0.0f, 0.0f };
    particleData.color = { 1.0f, 1.0f, 0.0f, 1.0f };
    particleData.currentTime = 0;
	particleData.lifeTime = 1.0f; // 寿命を1秒に設定
    return particleData;
}

///-------------------------------------------/// 
/// パーティクル生成
///-------------------------------------------///
std::list<ParticleData> HitEffectParticle::Emit(const Group& group, std::mt19937& randomEngine) {
    std::list<ParticleData> particles;
    for (uint32_t i = 0; i < group_.maxInstance; ++i) {
        particles.push_back(MakeParticle(randomEngine_, group_.transform.translate));
    }
    return particles;
}
