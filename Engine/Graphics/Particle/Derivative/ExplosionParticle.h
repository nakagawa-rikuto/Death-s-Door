#pragma once
/// ===Include=== ///
#include "Engine/Graphics/Particle/Base/ParticleGroup.h"
// Math
#include "Engine/DataInfo/ColliderData.h"
// c++
#include <random>

///=====================================================/// 
/// 爆破エミッタ
///=====================================================///
class ExplosionParticle : public ParticleGroup {
public:

    ExplosionParticle(); // コンストラクタでの処理は行わない
    ~ExplosionParticle();

    /// ===Override=== ///
    // 初期化
    void Initialze(const Vector3& translate)override;
    // 更新
    void Update() override;
    // 描画
    void Draw(BlendMode mode = BlendMode::KBlendModeNormal) override;
    // クローン
    std::unique_ptr<ParticleGroup> Clone() override;

private:
    // === コピー禁止 === ///
    ExplosionParticle(const ExplosionParticle&) = delete;
    ExplosionParticle& operator=(const ExplosionParticle&) = delete;

    // 爆発の設定
    float explosionRadius_; // 爆発の半径
    float maxLifetime_; // パーティクルの最大寿命
    bool hasExploded_; // パーティクルが発生したかのフラグ

    // ランダムパーティクル生成
    ParticleData MakeParticle(std::mt19937& randomEngine, const Vector3& translate)override;
    // エミっと
    std::list<ParticleData> Emit(const Group& group, std::mt19937& randomEngine)override;
};