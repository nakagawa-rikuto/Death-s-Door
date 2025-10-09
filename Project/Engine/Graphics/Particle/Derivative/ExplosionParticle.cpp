#include "ExplosionParticle.h"
// Math
#include "Math/sMath.h"
// c++
#include <numbers>

///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
ExplosionParticle::ExplosionParticle() {}
ExplosionParticle::~ExplosionParticle() {
    group_.particles.clear();
    group_.particle.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ExplosionParticle::Initialze(const Vector3& translate) {
    /// ===初期化=== ///
    ParticleGroup::InstancingInit("Particle", translate, 150);

    /// ===フラグと設定の初期化=== ///
    hasExploded_ = false;
    explosionRadius_ = 2.0f; // 飛び散る範囲を狭める
    maxLifetime_ = 2.0f;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void ExplosionParticle::Update() {
    if (!hasExploded_) {
        group_.particles.splice(group_.particles.end(), Emit(group_, randomEngine_)); // 発生処理
        hasExploded_ = true;
    }

    // パーティクルの更新
    group_.numInstance = 0; // インスタンス数をリセット
    for (auto it = group_.particles.begin(); it != group_.particles.end();) {
        if (it->currentTime >= it->lifeTime) {
            it = group_.particles.erase(it); // 寿命が尽きたパーティクルを削除
            continue;
        }

        // 位置と時間の更新
        it->transform.translate += it->velocity * 2.0f * kDeltaTime_; // 速度を速く
        it->transform.scale += Vector3{ 0.1f, 0.1f, 0.1f } *kDeltaTime_; // 拡大速度を上げる
        it->currentTime += kDeltaTime_;

        // カラーの変化: 黄色からオレンジに
        float colorShift = it->currentTime / it->lifeTime;
        it->color = { 1.0f, 0.8f - (0.8f * colorShift), 0.0f + (0.5f * colorShift), it->color.w };

        // アルファ値の更新
        float alpha = 1.0f - (it->currentTime / it->lifeTime);
        it->color.w = alpha;

        /// ===ParticleEmitterの更新=== ///
        ParticleGroup::InstancingUpdate(it);
        ++it;
    }
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ExplosionParticle::Draw(BlendMode mode) {
    if (group_.numInstance > 0) {
        ParticleGroup::Draw(mode);
    }
}

///-------------------------------------------/// 
/// クローン
///-------------------------------------------///
std::unique_ptr<ParticleGroup> ExplosionParticle::Clone() {
    // 新しいインスタンスを作成
    std::unique_ptr<ExplosionParticle> clone = std::make_unique<ExplosionParticle>();

    // 状態のコピー（Emit側でTranslateは上書きされるので最小限でOK）
    clone->explosionRadius_ = this->explosionRadius_;
    clone->maxLifetime_ = this->maxLifetime_;

    // 初期化は Emit 側で呼ばれるので不要
    return clone;
}

///-------------------------------------------/// 
/// 爆発パーティクルの生成
///-------------------------------------------///
ParticleData ExplosionParticle::MakeParticle(std::mt19937& randomEngine, const Vector3& center) {
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> distRadius(0.0f, explosionRadius_ * 0.5f); // 初期位置をさらに密集
    std::uniform_real_distribution<float> distLifetime(0.1f, maxLifetime_); // 寿命をさらに短く

    ParticleData particle;
    particle.transform.scale = { 0.5f, 0.5f, 0.5f }; // 小さめのサイズから開始
    particle.transform.rotate = { 0.0f, 0.0f, 0.0f };

    // ランダムな方向と大きさを生成
    float angle1 = distAngle(randomEngine);
    float angle2 = distAngle(randomEngine);
    float radius = distRadius(randomEngine);
    Vector3 direction = {
        radius * std::sin(angle1) * std::cos(angle2),
        radius * std::sin(angle1) * std::sin(angle2),
        radius * std::cos(angle1)
    };

    particle.transform.translate = center + direction;
    particle.velocity = direction * 2.5f; // 散らばる速度をさらに速く
    particle.color = { 1.0f, 0.8f, 0.0f, 1.0f }; // 黄色から始まる
    particle.lifeTime = distLifetime(randomEngine) / 3.0f;
    particle.currentTime = 0.0f;

    return particle;
}

///-------------------------------------------/// 
/// パーティクルの発生
///-------------------------------------------///
std::list<ParticleData> ExplosionParticle::Emit(const Group& group, std::mt19937& randomEngine) {
    std::list<ParticleData> particles;
    // 爆発のパーティクルを生成
    for (uint32_t i = 0; i < group.maxInstance; ++i) {
        particles.push_back(MakeParticle(randomEngine, group.transform.translate));
    }
    return particles;
}
