#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Graphics/Particle/Base/ParticleGroup.h"
// Math
#include "Engine/DataInfo/ColliderData.h"
// c++
#include <random>
#include <numbers>

///=====================================================/// 
/// RingParticle
///=====================================================///
class RingParticle : public ParticleGroup {
public:
	RingParticle() = default;
	~RingParticle();

    /// ===Override=== ///
   // 初期化
    void Initialze(const Vector3& translate) override;
    // 更新
    void Update() override;
    // 描画
    void Draw(BlendMode mode = BlendMode::KBlendModeNormal) override;
    // クローン
    std::unique_ptr<ParticleGroup> Clone() override;

private:

    RingParticle(const RingParticle&) = delete;
    RingParticle& operator=(const RingParticle&) = delete;

    // パーティクルが発生したかのフラグ
    bool hasExploded_;

private:
    ParticleData MakeParticle(std::mt19937& randomEngine, const Vector3& translate)override;
    // エミっと
    std::list<ParticleData> Emit(const Group& group, std::mt19937& randomEngine)override;
};

