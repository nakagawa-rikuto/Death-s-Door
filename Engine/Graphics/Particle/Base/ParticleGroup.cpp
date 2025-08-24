#include "ParticleGroup.h"
// Camera
#include "application/Game/Camera/Camera.h"
// Service
#include "Engine/System/Service/GraphicsResourceGetter.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"
// c++
#include <numbers>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
ParticleGroup::~ParticleGroup() { 
    group_.particles.clear();
    group_.particle.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ParticleGroup::InstancingInit(const std::string& modelName, const Vector3& translate, const uint32_t maxInstance, Camera* camera, shapeType type) {
    /// ===乱数生成器の初期化=== ///
    std::random_device seedGenerator;
    randomEngine_.seed(seedGenerator());

    /// ===最大パーティクル数の設定=== ///
    group_.maxInstance = maxInstance;
    group_.numInstance = 0;

    /// ===トランスフォームの初期化=== ///
    group_.transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, translate };
    group_.cameraTransform = {
       {1.0f,1.0f,1.0f},
       {std::numbers::pi_v<float> / 3.0f, std::numbers::pi_v<float>, 0.0f },
       {0.0f, 23.0f, 10.0f}
    };

    /// ===パーティクルグループの初期化=== ///
    group_.particle = std::make_unique<ParticleSetUp>();
    group_.particle->Initialze(modelName, group_.maxInstance, type);

    /// ===Cameraの設定=== ///
    group_.camera = camera;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void ParticleGroup::InstancingUpdate(std::list<ParticleData>::iterator it) {

    Matrix4x4 worldMatrix = Math::MakeAffineEulerMatrix(it->transform.scale, it->transform.rotate, it->transform.translate);
    Matrix4x4 wvpMatrix;

    if (group_.camera) {
        const Matrix4x4& viewProjectionMatrix = group_.camera->GetViewProjectionMatrix();
        wvpMatrix = Multiply(worldMatrix, viewProjectionMatrix);
    } else {
        Matrix4x4 viewMatrix = Math::Inverse4x4(Math::MakeAffineEulerMatrix(group_.cameraTransform.scale, group_.cameraTransform.rotate, group_.cameraTransform.translate));
        Matrix4x4 projectionMatrix = Math::MakePerspectiveFovMatrix(0.45f, static_cast<float>(GraphicsResourceGetter::GetWindowWidth()) / static_cast<float>(GraphicsResourceGetter::GetWindowHeight()), 0.1f, 100.0f);
        wvpMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
    }

    // インスタンシングデータを設定
    group_.particle->SetInstancingData(group_.numInstance, it->color, wvpMatrix, worldMatrix);
    ++group_.numInstance;
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ParticleGroup::Draw(BlendMode mode) {
	group_.particle->Darw(group_.numInstance, mode);
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
bool ParticleGroup::IsFinish() {
    // パーティクルがすべて消えたらtrue
    return group_.particles.empty();
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Texture
void ParticleGroup::SetTexture(const std::string& fileName) { group_.particle->SetTexture(fileName); }
