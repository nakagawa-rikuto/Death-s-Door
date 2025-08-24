#include "ConfettiParticle.h"
// Math
#include "Math/sMath.h"
// c++
#include <numbers>


///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
ConfettiParticle::ConfettiParticle() {}
ConfettiParticle::~ConfettiParticle() {
    group_.particles.clear();
    group_.particle.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ConfettiParticle::Initialze(const Vector3& translate, Camera* camera) {

    /// ===初期化=== ///
    ParticleGroup::InstancingInit("plane", translate, 300, camera);

    /// ===フラグと設定の初期化=== ///
    hasExploded_ = false;
    explosionRadius_ = 0.5f; // 初期位置を密集させる
    gravity_ = -9.8f; // 重力加速度
    upwardForce_ = 15.0f; // 上方向の初期加速度を追加
    maxLifetime_ = 2.0f; // パーティクルの寿命を調整

    // 位置の更新をかけたい
    //ParticleGroup::InstancingUpdate(Emit(group_, randomEngine_));
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void ConfettiParticle::Update() {
    if (!hasExploded_) {
        // 爆発のパーティクルを生成
        group_.particles.splice(group_.particles.end(), Emit(group_, randomEngine_));
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
        it->velocity.y += gravity_ * kDeltaTime_; // 重力を適用
        it->transform.translate += Vector3{ it->velocity.x * kDeltaTime_, it->velocity.y * kDeltaTime_, it->velocity.z * kDeltaTime_ }; // Z軸も有効

        // ランダムな回転速度を適用
        static std::uniform_real_distribution<float> rotationDist(-1.0f, 1.0f);
        it->transform.rotate += Vector3{ rotationDist(randomEngine_) * 10.0f, rotationDist(randomEngine_) * 10.0f, rotationDist(randomEngine_) * 10.0f } *kDeltaTime_;

        it->currentTime += kDeltaTime_;

        /// ===ParticleEmitterの更新=== ///
        ParticleGroup::InstancingUpdate(it);
        ++it;
    }
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ConfettiParticle::Draw(BlendMode mode) {
    if (group_.numInstance > 0) {
        ParticleGroup::Draw(mode);
    }
}

///-------------------------------------------/// 
/// クローン
///-------------------------------------------///
std::unique_ptr<ParticleGroup> ConfettiParticle::Clone() {
    // 新しいインスタンスを作成
    std::unique_ptr<ConfettiParticle> clone = std::make_unique<ConfettiParticle>();

    // 状態のコピー（Emit側でTranslateは上書きされるので最小限でOK）
    clone->explosionRadius_ = this->explosionRadius_;
    clone->maxLifetime_ = this->maxLifetime_;

    // 初期化は Emit 側で呼ばれるので不要
    return clone;
}

///-------------------------------------------/// 
/// コンフェッティパーティクルの生成
///-------------------------------------------///
ParticleData ConfettiParticle::MakeParticle(std::mt19937& randomEngine, const Vector3& center) {
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> distRadius(0.0f, explosionRadius_);
    std::uniform_real_distribution<float> distLifetime(0.5f, maxLifetime_);
    std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

    ParticleData particle;
    particle.transform.scale = { 0.1f, 0.1f, 0.1f }; // 小さめの四角形サイズ
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
    particle.velocity = direction * 5.0f; // 高速で散らばる
    particle.velocity.y += upwardForce_; // 上方向の初期加速度を加える
    particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f }; // ランダムな色
    particle.lifeTime = 6.0f;
    particle.currentTime = 0.0f;

    return particle;
}

///-------------------------------------------/// 
/// パーティクルの発生処理
///-------------------------------------------///
std::list<ParticleData> ConfettiParticle::Emit(const Group& group, std::mt19937& randomEngine) {
    std::list<ParticleData> particles;
    for (uint32_t i = 0; i < group_.maxInstance; ++i) {
        particles.push_back(MakeParticle(randomEngine_, group_.transform.translate));
    }
    return particles;
}
