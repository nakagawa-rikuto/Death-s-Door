#pragma once
/// ===include=== ///
// c++
#include <map>
#include <memory>
#include <string>
// PartlceGroup
#include "Engine/Graphics/Particle/Base/ParticleGroup.h"

/// ===前方宣言=== ///
class Camera;

///=====================================================/// 
/// ParticleManager
///=====================================================///
class ParticleManager {
public:
	ParticleManager() = default;
	~ParticleManager();

    // パーティクルの追加
    void AddParticle(const std::string& name, std::unique_ptr<ParticleGroup> particle);
    // パーティクルの発生
    void Emit(const std::string& name, const Vector3& translate);
    // テクスチャの設定
    void SetTexture(const std::string& name, const std::string& textureName);
    // パーティクルの削除
    void RemoveParticle(const std::string& name);

    // パーティクルの全体更新
    void Update();
    // すべて描画
    void Draw(BlendMode mode = BlendMode::kBlendModeAdd);

private:
    // 登録されたテンプレート
    std::map<std::string, std::unique_ptr<ParticleGroup>> prototype_;
    // 実際に動いているパーティクル
    std::map<std::string, std::vector<std::unique_ptr<ParticleGroup>>> activeParticles_;
};

