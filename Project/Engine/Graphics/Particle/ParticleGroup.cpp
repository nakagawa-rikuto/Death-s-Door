#include "ParticleGroup.h"
#include "ParticleFactory.h"
// Camera
#include "Engine/Camera/Base/CameraCommon.h"
// Service
#include "Service/GraphicsResourceGetter.h"
#include "Service/Camera.h"
#include "Service/DeltaTime.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"
#include "Math/TransformationMath.h"
// c++
#include <numbers>

namespace MiiEngine {
    ///-------------------------------------------/// 
    /// デストラクタ
    ///-------------------------------------------///
    ParticleGroup::~ParticleGroup() {
        group_.particles.clear();
        group_.particle.reset();
    }

    ///-------------------------------------------/// 
    /// 初期化（パラメータ駆動型）
    ///-------------------------------------------///
    void ParticleGroup::Initialize(const Vector3& translate, const ParticleDefinition& definition) {
        /// ===定義を保存=== ///
        definition_ = definition;

        /// ===乱数生成器の初期化=== ///
        std::random_device seedGenerator;
        randomEngine_.seed(seedGenerator());

        /// ===最大パーティクル数の設定=== ///
        group_.maxInstance = definition_.maxInstance;
        group_.numInstance = 0;

        /// ===トランスフォームの初期化=== ///
        group_.transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, translate };
        group_.cameraTransform = {
           {1.0f, 1.0f, 1.0f},
           {std::numbers::pi_v<float> / 3.0f, std::numbers::pi_v<float>, 0.0f},
           {0.0f, 23.0f, 10.0f}
        };

        /// 軌跡用の初期化
        group_.previousEmitterPosition = translate;
        group_.previousEmitterRotation = { 0.0f, 0.0f, 0.0f };
        group_.trajectoryTimer = 0.0f;

        /// ===パーティクルグループの初期化=== ///
        InstancingInit(definition_.modelName, translate, group_.maxInstance, definition_.shape);

        /// ===Cameraの設定=== ///
        group_.camera = Service::Camera::GetActiveCamera();

        /// ===発生タイマーの初期化=== ///
        group_.frequencyTimer = 0.0f;
        group_.hasEmitted = false;

        // デルタタイムの取得
        kDeltaTime_ = Service::DeltaTime::GetDeltaTime();

        // テクスチャの設定
        if (!definition_.appearance.texturePath.empty()) {
            SetTexture(definition_.appearance.texturePath);
        }

        // 
        Update();
    }

    ///-------------------------------------------/// 
    /// 更新
    ///-------------------------------------------///
    void ParticleGroup::Update() {
        // デルタタイムの取得
        kDeltaTime_ = Service::DeltaTime::GetDeltaTime();

        // 停止中の場合は新規発生を止めて、既存パーティクルの寿命を短縮
        if (isStopped_) {
            // 既存のパーティクルの寿命を強制終了
            for (auto& particle : group_.particles) {
                particle.currentTime = particle.lifeTime;  // 即座に寿命を終了させる
            }
            // パーティクルの更新のみ実行（発生処理はスキップ）
            UpdateParticles();
            return;
        }

        // 通常時の処理
        // パーティクルの発生処理
        Emit();

        // パーティクルの更新処理
        UpdateParticles();
    }

    ///-------------------------------------------/// 
    /// 描画
    ///-------------------------------------------///
    void ParticleGroup::Draw(BlendMode mode) {
        if (group_.numInstance > 0) {
            group_.particle->Draw(group_.numInstance, mode);
        }
    }

    ///-------------------------------------------/// 
    /// 停止処理
    ///-------------------------------------------///
    void ParticleGroup::Stop() { isStopped_ = true; }

    ///-------------------------------------------/// 
    /// 終了判定
    ///-------------------------------------------///
    bool ParticleGroup::IsFinish() const {
        // 停止されている場合、全パーティクルが消えたら終了
        if (isStopped_) {
            return group_.particles.empty();
        }

        // バーストモードの場合は全パーティクルが消えたらtrue
        if (definition_.emission.isBurst) {
            return group_.particles.empty() && group_.hasEmitted;
        }

        // 連続発生モードの場合は常にfalse（手動で停止する必要がある）
        return false;
    }

    ///-------------------------------------------/// 
    /// 停止判定
    ///-------------------------------------------///
    bool ParticleGroup::IsStopped() const { return isStopped_; }

    ///-------------------------------------------/// 
    /// Setter
    ///-------------------------------------------///
    // テクスチャ設定
    void ParticleGroup::SetTexture(const std::string& fileName) {
        if (group_.particle) {
            group_.particle->SetTexture(fileName);
        }
    }
    // パーティクル定義設定
    void ParticleGroup::SetDefinition(const ParticleDefinition& definition) {
        definition_ = definition;

        // テクスチャも更新
        if (!definition_.appearance.texturePath.empty()) {
            SetTexture(definition_.appearance.texturePath);
        }
    }
    // パラメーター変更
    void ParticleGroup::SetParameter(ParticleParameter param, float value) {
        // 実行時にパラメータを変更
        switch (param) {
            // 物理パラメータ
        case ParticleParameter::Gravity:
            definition_.physics.gravity = value;
            break;
        case ParticleParameter::ExplosionRadiusX:
            definition_.physics.explosionRange.x = value;
            break;
        case ParticleParameter::ExplosionRadiusY:
            definition_.physics.explosionRange.y = value;
            break;
        case ParticleParameter::ExplosionRadiusZ:
            definition_.physics.explosionRange.z = value;
            break;
        case ParticleParameter::UpwardForce:
            definition_.physics.upwardForce = value;
            break;
        case ParticleParameter::AccelerationX:
            definition_.physics.acceleration.x = value;
            break;
        case ParticleParameter::AccelerationY:
            definition_.physics.acceleration.y = value;
            break;
        case ParticleParameter::AccelerationZ:
            definition_.physics.acceleration.z = value;
            break;

            // 発生パラメータ
        case ParticleParameter::EmissionRate:
            definition_.emission.emissionRate = value;
            break;
        case ParticleParameter::Frequency:
            definition_.emission.frequency = value;
            break;
        case ParticleParameter::LifetimeMin:
            definition_.emission.lifetimeMin = value;
            break;
        case ParticleParameter::LifetimeMax:
            definition_.emission.lifetimeMax = value;
            break;
        case ParticleParameter::BurstCount:
            definition_.emission.burstCount = static_cast<uint32_t>(value);
            break;

            // 見た目パラメータ
        case ParticleParameter::StartColorR:
            definition_.appearance.startColor.x = value;
            break;
        case ParticleParameter::StartColorG:
            definition_.appearance.startColor.y = value;
            break;
        case ParticleParameter::StartColorB:
            definition_.appearance.startColor.z = value;
            break;
        case ParticleParameter::StartColorA:
            definition_.appearance.startColor.w = value;
            break;
        case ParticleParameter::EndColorR:
            definition_.appearance.endColor.x = value;
            break;
        case ParticleParameter::EndColorG:
            definition_.appearance.endColor.y = value;
            break;
        case ParticleParameter::EndColorB:
            definition_.appearance.endColor.z = value;
            break;
        case ParticleParameter::EndColorA:
            definition_.appearance.endColor.w = value;
            break;

            // 回転パラメータ
        case ParticleParameter::RotationSpeedX:
            definition_.rotation.rotationSpeedMin.x = value;
            break;
        case ParticleParameter::RotationSpeedY:
            definition_.rotation.rotationSpeedMin.y = value;
            break;
        case ParticleParameter::RotationSpeedZ:
            definition_.rotation.rotationSpeedMin.z = value;
            break;

            // 速度パラメータ
        case ParticleParameter::VelocityMinX:
            definition_.physics.velocityMin.x = value;
            break;
        case ParticleParameter::VelocityMinY:
            definition_.physics.velocityMin.y = value;
            break;
        case ParticleParameter::VelocityMinZ:
            definition_.physics.velocityMin.z = value;
            break;
        case ParticleParameter::VelocityMaxX:
            definition_.physics.velocityMax.x = value;
            break;
        case ParticleParameter::VelocityMaxY:
            definition_.physics.velocityMax.y = value;
            break;
        case ParticleParameter::VelocityMaxZ:
            definition_.physics.velocityMax.z = value;
            break;
        }
    }
    // エミッタ位置設定
    void ParticleGroup::SetEmitterPosition(const Vector3& position) {
        group_.previousEmitterPosition = group_.transform.translate;
        group_.transform.translate = position;
    }
    // 
    void ParticleGroup::MoveEmitterPosition(const Vector3& offset) { group_.transform.translate += offset; }
    // エミッタ回転設定
    void ParticleGroup::SetEmitterRotate(const Vector3& rotate) {
        group_.previousEmitterRotation = group_.transform.rotate;
        group_.transform.rotate = rotate;
    }

    ///-------------------------------------------/// 
    /// Getter
    ///-------------------------------------------///
    const ParticleDefinition& ParticleGroup::GetDefinition() const { return definition_; }
    uint32_t ParticleGroup::GetActiveParticleCount() const { return static_cast<uint32_t>(group_.particles.size()); }
    const Vector3& ParticleGroup::GetEmitterPosition() const { return group_.transform.translate; }

    ///-------------------------------------------/// 
    /// インスタンシング初期化
    ///-------------------------------------------///
    void ParticleGroup::InstancingInit(const std::string& modelName, const Vector3& translate, const uint32_t maxInstance, shapeType type) {
        /// ===パーティクルグループの初期化=== ///
        group_.particle = std::make_unique<ParticleSetUp>();
        group_.particle->Initialize(modelName, maxInstance, type);
        translate;
    }

    ///-------------------------------------------/// 
    /// インスタンシング更新
    ///-------------------------------------------///
    void ParticleGroup::InstancingUpdate(std::list<ParticleData>::iterator it) {
        // ワールド行列とWVP行列の計算
        Matrix4x4 worldMatrix = Math::MakeAffineEulerMatrix(
            it->transform.scale,
            it->transform.rotate,
            it->transform.translate);
        Matrix4x4 wvpMatrix;

        // カメラが存在する場合はWVP行列を計算
        if (group_.camera) {
            const Matrix4x4& viewProjectionMatrix = group_.camera->GetViewProjectionMatrix();
            wvpMatrix = Multiply(worldMatrix, viewProjectionMatrix);
        } else {
            Matrix4x4 viewMatrix = Math::Inverse4x4(
                Math::MakeAffineEulerMatrix(
                    group_.cameraTransform.scale,
                    group_.cameraTransform.rotate,
                    group_.cameraTransform.translate));
            Matrix4x4 projectionMatrix = Math::MakePerspectiveFovMatrix(
                0.45f,
                static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()) /
                static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight()),
                0.1f, 100.0f);
            wvpMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
        }

        // インスタンシングデータを設定
        group_.particle->SetInstancingData(group_.numInstance, it->color, wvpMatrix, worldMatrix);
        ++group_.numInstance;
    }

    ///-------------------------------------------/// 
    /// パーティクル生成（パラメータ駆動型）
    ///-------------------------------------------///
    ParticleData ParticleGroup::MakeParticle(const Vector3& translate) {
        // 基本パーティクルを生成
        ParticleData particle = ParticleFactory::CreateParticle(definition_, randomEngine_, translate);

        // 軌跡モードの場合、エミッタの回転を考慮
        if (definition_.advanced.isTrajectoryParticle) {
            // エミッタの回転をクォータニオンに変換
            Quaternion emitterQuat = Math::EulerToQuaternion(group_.transform.rotate);

            // 発生位置のオフセットをエミッタの回転空間で適用
            Vector3 localOffset = particle.transform.translate - translate;
            Vector3 worldOffset = Math::RotateVector(localOffset, emitterQuat);
            particle.transform.translate = translate + worldOffset;

            // 初速度もエミッタの回転を適用
            particle.velocity = Math::RotateVector(particle.velocity, emitterQuat);

            // パーティクル自体の初期回転もエミッタに合わせる
            if (definition_.rotation.enableRotation) {
                Quaternion particleRot = Math::EulerToQuaternion(particle.transform.rotate);
                Quaternion finalRot = Multiply(emitterQuat, particleRot);
                particle.transform.rotate = Math::QuaternionToEuler(finalRot);
            }
        }

        return particle;
    }

    ///-------------------------------------------/// 
    /// パーティクル発生処理
    ///-------------------------------------------///
    void ParticleGroup::Emit() {
        // 軌跡パーティクルモード
        if (definition_.advanced.isTrajectoryParticle) {
            group_.trajectoryTimer += kDeltaTime_;

            if (group_.trajectoryTimer >= definition_.advanced.trailSpacing) {
                // エミッタが移動している場合のみ発生
                Vector3 movementDelta = group_.transform.translate - group_.previousEmitterPosition;
                float distanceMoved = std::sqrt(
                    movementDelta.x * movementDelta.x +
                    movementDelta.y * movementDelta.y +
                    movementDelta.z * movementDelta.z
                );

                if (distanceMoved > 0.01f) {
                    // パーティクル生成数
                    uint32_t particlesPerEmit = definition_.advanced.emissionPattern.particlesPerEmit;

                    for (uint32_t i = 0; i < particlesPerEmit; ++i) {
                        if (group_.particles.size() < group_.maxInstance) {
                            group_.particles.push_back(MakeParticle(group_.transform.translate));
                        }
                    }
                }

                group_.trajectoryTimer = 0.0f;
            }
            return;
        }

        // バーストモード
        if (definition_.emission.isBurst) {
            if (!group_.hasEmitted) {
                std::list<ParticleData> burstParticles = ParticleFactory::CreateParticleBurst(
                    definition_,
                    randomEngine_,
                    group_.transform.translate,
                    definition_.emission.burstCount);

                group_.particles.splice(group_.particles.end(), burstParticles);
                group_.hasEmitted = true;
            }
            return;
        }

        // 連続発生モード
        group_.frequencyTimer += kDeltaTime_;

        if (group_.frequencyTimer >= definition_.emission.frequency) {
            uint32_t emitCount = static_cast<uint32_t>(
                definition_.emission.emissionRate * definition_.emission.frequency);

            for (uint32_t i = 0; i < emitCount; ++i) {
                if (group_.particles.size() < group_.maxInstance) {
                    group_.particles.push_back(MakeParticle(group_.transform.translate));
                }
            }

            group_.frequencyTimer = 0.0f;
        }
    }

    ///-------------------------------------------/// 
    /// パーティクル更新処理
    ///-------------------------------------------///
    void ParticleGroup::UpdateParticles() {
        group_.numInstance = 0;

        // エミッタの移動量計算
        Vector3 emitterDelta = group_.transform.translate - group_.previousEmitterPosition;

        for (auto it = group_.particles.begin(); it != group_.particles.end();) {
            // 寿命チェック
            if (it->currentTime >= it->lifeTime) {
                it = group_.particles.erase(it);
                continue;
            }

            // 基本更新（ParticleFactory使用）
            ParticleFactory::UpdateParticle(*it, definition_, kDeltaTime_, randomEngine_);

            // 進行度の計算
            float progress = it->currentTime / it->lifeTime;

            // エミッタ追従
            if (definition_.advanced.motion.followEmitter) {
                Vector3 followOffset = emitterDelta * definition_.advanced.motion.followStrength;
                it->transform.translate += followOffset;
            }

            // 速度減衰
            if (definition_.advanced.motion.enableVelocityDamping) {
                it->velocity = it->velocity * definition_.advanced.motion.velocityDamping;
            }

            // 渦巻き運動
            if (definition_.advanced.motion.enableSwirling) {
                ApplySwirlMotion(*it, progress);
            }

            // 回転影響
            if (definition_.advanced.motion.useRotationInfluence) {
                ApplyRotationInfluence(*it, progress);
            }

            // ビルボード回転
            if (definition_.advanced.motion.enableBillboardRotation) {
                float rotationSpeed = definition_.advanced.motion.billboardRotationSpeed;
                it->transform.rotate.z += rotationSpeed * kDeltaTime_;
            }

            // インスタンシング更新
            InstancingUpdate(it);
            ++it;
        }

        // 前回の状態を保存
        group_.previousEmitterPosition = group_.transform.translate;
        group_.previousEmitterRotation = group_.transform.rotate;
    }

    ///-------------------------------------------/// 
    /// 渦巻き運動の適用
    ///-------------------------------------------///
    void ParticleGroup::ApplySwirlMotion(ParticleData& particle, float progress) {
        float swirl = particle.currentTime * definition_.advanced.motion.swirlingSpeed;

        // エミッタの回転を考慮したローカル空間での渦巻き
        Vector3 localSwirlingOffset = {
            std::cos(swirl) * definition_.advanced.motion.expansionRate * progress,
            std::sin(swirl * 1.3f) * definition_.advanced.motion.expansionRate * progress * 0.5f,
            std::sin(swirl) * definition_.advanced.motion.expansionRate * progress
        };

        // エミッタの回転をクォータニオンに変換
        Quaternion emitterRotation = Math::EulerToQuaternion(group_.transform.rotate);

        // ワールド空間に変換して適用
        Vector3 worldSwirlingOffset = Math::RotateVector(localSwirlingOffset, emitterRotation);
        particle.transform.translate += worldSwirlingOffset * kDeltaTime_;
    }

    ///-------------------------------------------/// 
    /// 回転影響の適用
    ///-------------------------------------------///
    void ParticleGroup::ApplyRotationInfluence(ParticleData& particle, float progress) {
        progress;
        // 現在と前回のエミッタ回転を比較
        Quaternion currentRot = Math::EulerToQuaternion(group_.transform.rotate);
        Quaternion previousRot = Math::EulerToQuaternion(group_.previousEmitterRotation);
        Quaternion rotationDelta = Multiply(currentRot, Math::Conjugate(previousRot));

        // 回転による遠心力的な速度を追加
        Vector3 rotationalVelocity = Math::RotateVector(
            particle.velocity * definition_.advanced.motion.rotationInfluence,
            rotationDelta
        );

        particle.transform.translate += rotationalVelocity * kDeltaTime_ * 0.3f;
    }
}
