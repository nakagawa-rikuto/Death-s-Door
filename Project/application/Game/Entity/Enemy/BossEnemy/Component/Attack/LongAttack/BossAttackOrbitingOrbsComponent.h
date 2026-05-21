#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <array>

///=====================================================/// 
/// OrbitionOrbs
/// 周りを回る球体を出現させる
///=====================================================///
class BossAttackOrbitingOrbsComponent {
public:
    static constexpr int kOrbCount = 3; // 公転弾数

private:
    /// ===フェーズ定義=== ///
    enum class OrbitPhase {
        Idle,       // 非アクティブ（待機）
        Orbiting,   // 公転中（生存タイマーが尽きるまで）
        Finished,   // 攻撃終了
    };

    /// ===状態=== ///
    struct OrbitState {
        float orbitTimer = 0.0f; // 公転開始からの経過時間（角度計算に使用）
        float lifeTimer = 0.0f;  // 残り生存時間
    };

public:
    /// ===設定パラメータ=== ///
    struct OrbitConfig {
        // --- 公転パラメータ ---
        float orbitRadius = 3.0f;     // 公転半径（ボスからの距離）
        float orbitSpeedDeg = 120.0f; // 1秒あたりの公転角速度（度/秒）
        float orbitHeight = 0.0f;     // ボスY座標からの高さオフセット

        // --- 弾の生存時間 ---
        float lifetime = 5.0f;   // 公転を続ける時間（秒）

        // --- 弾の初期位相オフセット ---
        float initialAngleDeg = 0.0f;
    };

    /// ===更新コンテキスト（毎フレーム渡す）=== ///
    struct UpdateContext {
        Vector3 bossPosition{};  // ボスのワールド座標（公転中心）
        float deltaTime = 0.0f;
    };

    /// ===1発分の弾情報=== ///
    struct BulletInfo {
        Vector3 position{};   // ワールド座標（弾をここに移動させる）
        Vector3 direction{};  // 移動方向（BossEnemyBullet::SetDirection に渡す）
    };

    /// ===更新結果=== ///
    struct UpdateResult {
        std::array<BulletInfo, kOrbCount> bullets{};  // 各弾の位置・方向
        bool isOrbiting = false; // 公転中フラグ
        bool isFinished = false; // 攻撃終了フラグ
    };

public:

    BossAttackOrbitingOrbsComponent() = default;
    ~BossAttackOrbitingOrbsComponent() = default;

    BossAttackOrbitingOrbsComponent(const BossAttackOrbitingOrbsComponent&) = delete;
    BossAttackOrbitingOrbsComponent& operator=(const BossAttackOrbitingOrbsComponent&) = delete;

    /// <summary>
    /// 初期化（コンポーネント生成時に一度だけ呼ぶ）
    /// </summary>
    void Initialize(const OrbitConfig& config = OrbitConfig{});

    /// <summary>
    /// 毎フレーム呼び出す。各弾の位置・方向を返す。
    /// </summary>
    UpdateResult Update(const UpdateContext& context);

    /// <summary>
    /// 攻撃を強制リセットし Idle へ戻す。
    /// </summary>
    void Reset();

    /// <summary>
    /// ImGui 情報の表示
    /// </summary>
    void Information();

    /// <summary>
    /// 攻撃を開始する。呼び出した瞬間から Orbiting フェーズへ遷移。
    /// </summary>
    void StartAttack();

public: /// ===Getter=== ///
    bool IsActive()   const { return phase_ == OrbitPhase::Orbiting; }
    bool IsFinished() const { return phase_ == OrbitPhase::Finished; }
    const OrbitConfig& GetConfig() const { return config_; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
    void ApplyConfig(const OrbitConfig& newConfig);
#endif // USE_IMGUI

private:
    OrbitConfig  config_{};
    OrbitState   state_{};
    OrbitPhase   phase_ = OrbitPhase::Idle;

private:
    /// <summary>
    /// 弾 index の公転角度（ラジアン）を計算する。
    /// </summary>
    /// <param name="index">弾のインデックス</param>
    float CalcOrbitAngleRad(int index) const;

    /// <summary>
    /// 公転角度からワールド座標を計算する。
    /// </summary>
    /// <param name="center">公転中心の座標</param>
    /// <param name="angleRad">現在の公転角度（ラジアン）</param>
    Vector3 CalcOrbitPosition(const Vector3& center, float angleRad) const;

    /// <summary>
    /// 次フレームの向きを現在位置と次位置の差分から計算する。
    /// </summary>
    /// <param name="center">公転中心の座標</param>
    /// <param name="currentPos">現在の弾の座標</param>
    /// <param name="angleRad">現在の公転角度（ラジアン）</param>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    Vector3 CalcOrbitDirection(const Vector3& center, const Vector3& currentPos, float angleRad, float deltaTime) const;
};

