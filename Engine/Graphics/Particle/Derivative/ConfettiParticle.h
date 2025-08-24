#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Graphics/Particle/Base/ParticleGroup.h"
// Math
#include "Engine/DataInfo/ColliderData.h"
// c++
#include <random>

///=====================================================/// 
/// コンフェッティパーティクル
///=====================================================///
class ConfettiParticle : public ParticleGroup {
public:

    ConfettiParticle(); // コンストラクタでの処理は行わない
    ~ConfettiParticle();

    /// ===Override=== ///
    // 初期化
    void Initialze(const Vector3& translate, Camera* camera) override;
    // 更新
    void Update() override;
    // 描画
    void Draw(BlendMode mode = BlendMode::KBlendModeNormal) override;
    // クローン
    std::unique_ptr<ParticleGroup> Clone() override;

private:
    // === コピー禁止 === ///
    ConfettiParticle(const ConfettiParticle&) = delete;
    ConfettiParticle& operator=(const ConfettiParticle&) = delete;

    // コンフェッティ設定
    float explosionRadius_; // 爆発の半径
    float gravity_; // 重力加速度
    float upwardForce_; // 上方向の初期加速度
    float maxLifetime_; // パーティクルの最大寿命
    bool hasExploded_; // パーティクルが発生したかのフラグ

    // ランダムパーティクル生成
    ParticleData MakeParticle(std::mt19937& randomEngine, const Vector3& translate)override;
    // エミっと
    std::list<ParticleData> Emit(const Group& group, std::mt19937& randomEngine)override;
};