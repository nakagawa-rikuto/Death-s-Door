#pragma once
/// ===Include=== ///
#include "Engine/Collider/OBBCollider.h"
#include "Engine/Collider/SphereCollider.h"
#include "GameCharacterCollision.h"

///-------------------------------------------/// 
/// GameCharacterBase
///-------------------------------------------///
template<typename TCollider>
class GameCharacter : public TCollider {
public:

    GameCharacter() = default;
    virtual ~GameCharacter();

    // 初期化
    virtual void Initialize() override;
    // 更新
    virtual void Update() override;
    // 描画
    virtual void Draw(BlendMode mode) override;
    // ImGui
    virtual void Information() override;

public: /// ===衝突判定=== ///
    virtual void OnCollision(Collider* collider) override;

public: /// ===Setter・Getter=== ///
    // Velocity
    Vector3 GetVelocity() const { return baseInfo_.velocity; }
    void SetVelocity(const Vector3 vel) { baseInfo_.velocity = vel; };
    // deltaTime
    float GetDeltaTime() const { return baseInfo_.deltaTIme; };
    // isDead
    bool GetIsDead() const { return baseInfo_.isDead; };
    void SetIsDead(const bool isDead) { baseInfo_.isDead = isDead; };
protected:

	// GameCharacterCollision
    std::unique_ptr<GameCharacterCollision> collision_;

    /// ===基本情報=== ///
    struct BaseInfo {
        Vector3 velocity; // 速度
        float deltaTIme;  // 時間
        bool isDead;      // 死亡フラグ
    };
    BaseInfo baseInfo_;
};

