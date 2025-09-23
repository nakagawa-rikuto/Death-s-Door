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
/// CylinderParticle
///=====================================================///
class CylinderParticle : public ParticleGroup {
public:
    CylinderParticle() = default;
    ~CylinderParticle();

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

    CylinderParticle(const CylinderParticle&) = delete;
    CylinderParticle& operator=(const CylinderParticle&) = delete;

    // パーティクルが発生したかのフラグ
    bool hasExploded_;

    // 半径
    float radius_ = 1.0f;
    // 回転速度
    float rotationSpeed_ = 1.0f;

private:
    ParticleData MakeParticle(std::mt19937& randomEngine, const Vector3& translate)override;
    // エミっと
    std::list<ParticleData> Emit(const Group& group, std::mt19937& randomEngine)override;
};

