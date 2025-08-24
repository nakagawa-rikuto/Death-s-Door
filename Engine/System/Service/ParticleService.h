#pragma once
/// ===include=== ///
// c++
#include <memory>
#include <string>
// Math
#include "Math/Vector3.h"

/// ===前方宣言=== ///
class ParticleGroup;
class Camera;

///=====================================================/// 
/// ParticleService
///=====================================================///
class ParticleService {
public:

    // パーティクルの追加
    static void AddParticle(const std::string& name, std::unique_ptr<ParticleGroup> particle);
    // パーティクルの削除
    static void RemoveParticle(const std::string& name);
    // パーティクルの発生
    static void Emit(const std::string& name, const Vector3& translate);
    // テクスチャの設定
    static void SetTexture(const std::string& name, const std::string& textureName);
};
