#pragma once
/// ===Include=== ///
// Collider
#include "Engine/Collider/OBBCollider.h"
#include "Engine/Collider/SphereCollider.h"
#include "Engine/Collider/ColliderConcept.h"
// GameCharacterCollision
#include "Collision/GameCharacterCollision.h"

/// ===前方宣言=== ///
class GroundOcean;

///-------------------------------------------/// 
/// GameCharacterBase
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
class GameCharacter : public TCollider {
public:

    GameCharacter();
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
    virtual void Draw(MiiEngine::BlendMode mode) override;

    /// <summary>
    /// ImGui情報の表示
    /// </summary>
    virtual void Information() override;

    /// <summary>
    /// 更新処理の前処理
    /// </summary>
    void PreUpdate();
   

public: /// ===衝突判定=== ///
    /// <summary>
    /// 衝突時の処理
    /// </summary>
    /// <param name="collider">衝突した相手を表す Collider へのポインター。</param>
    virtual void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===Getter=== ///
    // Velocity
    Vector3 GetVelocity() const { return baseInfo_.velocity; }
    // isDead
    bool GetIsDead() const { return baseInfo_.isDead; };
    // deltaTime
    float GetDeltaTime() const { return baseInfo_.deltaTime; };
    // HP
    uint32_t GetHP() const { return baseInfo_.HP; }
    // GroundOcean
    GroundOcean* GetGroundOcean() const { return groundOcean_; }
    // GroundInfo
	float GetGroundYPos() const { return groundInfo_.currentGroundYPos; }
   
public: /// ===Setter=== ///
    // Velocity
    void SetVelocity(const Vector3 vel) { baseInfo_.velocity = vel; };
    // isDead
    void SetIsDead(const bool isDead) { baseInfo_.isDead = isDead; };
	// HalfSizeY
	void SetHalfSizeY(const float halfSizeY) { characterHalfSizeY_ = halfSizeY; }
    // GroundOcean
    void SetGroundOcean(GroundOcean* ocean) { groundOcean_ = ocean; }

protected:
    // GroundOcean
    GroundOcean* groundOcean_ = nullptr;

    // Collisionクラス
    std::unique_ptr<MiiEngine::ColliderCollision> collision_;
    std::unique_ptr<GameCharacterCollision> gCollision_;

    /// ===基本情報=== ///
    struct BaseInfo {
        Vector3 velocity{};     // 速度
        float deltaTime = 0.0f; // 時間
		float gravity = -9.8f;  // 重力
		uint32_t HP = 10;       // 体力
        bool isDead = false;    // 死亡フラグ
		MiiEngine::LightInfo lightInfo_; // ライト情報
    };
    BaseInfo baseInfo_;

private: /// ===地面との衝突処理=== ///

    // キャラクターの半サイズY軸
    float characterHalfSizeY_ = 0.0f;   

    /// ===GroundInfo=== ///
    struct GroundInfo {
        float currentGroundYPos = 0.0f;    // 現在の地面のY軸
        bool isGrounded = false;		   // 地面接地フラグ
    };
	GroundInfo groundInfo_;

    /// <summary>
    /// 指定した中心位置と半サイズを持つオブジェクトの地面との衝突を検出して処理します。
    /// </summary>
    void GroundCollision();
};

