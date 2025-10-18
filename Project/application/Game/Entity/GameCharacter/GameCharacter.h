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

    /// <summary>
    /// 初期化処理
    /// </summary>
    virtual void Initialize() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() override;

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="mode">描画に使用するブレンドモード。</param>
    virtual void Draw(BlendMode mode) override;

    /// <summary>
    /// ImGui情報の表示
    /// </summary>
    virtual void Information() override;

public: /// ===衝突判定=== ///
    /// <summary>
    /// 衝突時の処理
    /// </summary>
    /// <param name="collider">衝突した相手を表す Collider へのポインター。</param>
    virtual void OnCollision(Collider* collider) override;

public: /// ===Getter=== ///
    // Velocity
    Vector3 GetVelocity() const { return baseInfo_.velocity; }
    // isDead
    bool GetIsDead() const { return baseInfo_.isDead; };
    // deltaTime
    float GetDeltaTime() const { return baseInfo_.deltaTIme; };
   

public: /// ===Setter=== ///
    // Velocity
    void SetVelocity(const Vector3 vel) { baseInfo_.velocity = vel; };
    // isDead
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

