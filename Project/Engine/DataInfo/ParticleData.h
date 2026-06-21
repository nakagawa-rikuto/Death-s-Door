#pragma once
#include "Engine/DataInfo/CData.h"

namespace MiiEngine {
    ///=====================================================/// 
    /// パーティクル形状タイプ
    ///=====================================================///
    enum class shapeType {
        kCircle,    // 円
        kCylinder,  // 円柱
        kNone,      // なし（通常のプレーン）
    };

    ///=====================================================/// 
    /// パーティクルブレンドモード
    /// ※既存のBlendModeと重複する場合は削除
    ///=====================================================///
    enum class ParticleBlendMode {
        kNormal,    // 通常
        kAdd,       // 加算
        kSubtract,  // 減算
        kMultiply,  // 乗算
    };

    ///=====================================================/// 
    /// パーティクルデータ（実行時）
    ///=====================================================///
    struct ParticleData {
        EulerTransform transform;  // トランスフォーム
        Vector3 velocity;          // 速度
        Vector4 color;             // 色
        float lifeTime;            // 寿命
        float currentTime;         // 経過時間
        Vector3 rotationSpeed;     // 回転速度
        Vector3 initialScale;      // 初期スケール
        Vector4 initialColor;      // 初期色
    };

    ///=====================================================/// 
    /// パーティクルGPU送信用データ
    ///=====================================================///
    struct ParticleForGPU {
        Matrix4x4 WVP;    // ワールドビュープロジェクション行列
        Matrix4x4 World;  // ワールド行列
        Vector4 color;    // 色
    };

    ///=====================================================/// 
    /// パーティクル物理設定
    ///=====================================================///
    struct ParticlePhysicsSettings {
        Vector3 velocityMin = { -1.0f, 1.0f, -1.0f };      // 速度の最小値
        Vector3 velocityMax = { 1.0f, 5.0f, 1.0f };        // 速度の最大値
        Vector3 acceleration = { 0.0f, 0.0f, 0.0f };       // 加速度
        float gravity = 0.0f;                              // 重力
        Vector3 explosionRange = { 1.0f, 1.0f, 1.0f };     // 発生範囲（X、Y、Z軸）
        float upwardForce = 0.0f;                          // 上方向の力
        bool useRandomVelocity = true;                     // ランダム速度を使用
    };

    ///=====================================================/// 
    /// パーティクル見た目設定
    ///=====================================================///
    struct ParticleAppearanceSettings {
        Vector4 startColor = { 1.0f, 1.0f, 1.0f, 1.0f };  // 開始色
        Vector4 endColor = { 1.0f, 1.0f, 1.0f, 0.0f };    // 終了色
        Vector3 startScaleMin = { 1.0f, 1.0f, 1.0f };     // 開始スケール最小
        Vector3 startScaleMax = { 1.0f, 1.0f, 1.0f };     // 開始スケール最大
        Vector3 endScale = { 0.5f, 0.5f, 0.5f };          // 終了スケール
        std::string texturePath = "";                     // テクスチャパス
        bool useColorGradient = false;                    // 色のグラデーション使用
        bool useScaleAnimation = false;                   // スケールアニメーション使用
    };

    ///=====================================================/// 
    /// パーティクル回転設定
    ///=====================================================///
    struct ParticleRotationSettings {
        Vector3 initialRotationMin = { 0.0f, 0.0f, 0.0f }; // 初期回転最小
        Vector3 initialRotationMax = { 0.0f, 0.0f, 0.0f }; // 初期回転最大
        Vector3 rotationSpeedMin = { 0.0f, 0.0f, 0.0f };  // 回転速度最小
        Vector3 rotationSpeedMax = { 0.0f, 0.0f, 0.0f };  // 回転速度最大
        bool randomRotation = false;                      // ランダム回転を使用
        bool enableRotation = false;                      // 回転を有効化
        bool randomInitialRotation = false;               // ランダム初期回転を使用
    };

    ///=====================================================/// 
    /// パーティクル発生設定
    ///=====================================================///
    struct ParticleEmissionSettings {
        float lifetimeMin = 1.0f;           // 寿命最小値
        float lifetimeMax = 2.0f;           // 寿命最大値
        float emissionRate = 10.0f;         // 発生レート（秒間発生数）
        uint32_t burstCount = 0;            // バースト数（0なら連続発生）
        float frequency = 0.5f;             // 発生頻度（秒）
        bool isBurst = false;               // バーストモード
    };

    ///=====================================================/// 
    /// パーティクル動作設定
    ///=====================================================///
    struct ParticleMotionSettings {
        // 渦巻き設定
        bool enableSwirling = false;           // 渦巻き運動を有効化
        float swirlingSpeed = 3.0f;            // 渦巻き速度
        float expansionRate = 0.8f;            // 拡散速度

        // 軌跡追従設定
        bool followEmitter = false;            // エミッタに追従するか
        float followStrength = 1.0f;           // 追従強度(0.0~1.0)

        // 回転影響設定
        bool useRotationInfluence = false;     // エミッタの回転影響を受けるか
        float rotationInfluence = 1.2f;        // 回転速度の影響係数

        // 速度減衰
		bool enableVelocityDamping = true;     // 速度減衰を有効化
        float velocityDamping = 0.95f;         // 速度減衰率(0.0~1.0)

        // ビルボード回転
        bool enableBillboardRotation = false;  // ビルボード回転を有効化
        float billboardRotationSpeed = 3.0f;   // ビルボード回転速度
    };

    ///=====================================================/// 
    /// パーティクル発生パターン設定
    ///=====================================================///
    struct ParticleEmissionPattern {
        enum class Pattern {
            Point,          // 点から発生
            Sphere,         // 球状に発生
            Cone,           // 円錐状に発生
            Trail,          // 軌跡として発生
            Ring,           // リング状に発生
            Burst           // 爆発的に発生
        };

        Pattern pattern = Pattern::Sphere;
        float patternRadius = 0.25f;           // パターンの半径
        float patternAngle = 0.0f;            // 円錐の角度(度)
        int particlesPerEmit = 5;             // 1回の発生での粒子数
    };

    ///=====================================================/// 
    /// パーティクルカラーグラデーション設定
    ///=====================================================///
    struct ParticleColorGradient {
        bool useGradient = false;              // グラデーションを使用するか
        Vector4 primaryColor = { 0.2f, 0.8f, 1.0f, 1.0f };    // 主要色
        Vector4 secondaryColor = { 1.0f, 0.9f, 0.3f, 1.0f };  // セカンダリ色
        float colorBlendCurve = 1.0f;          // ブレンドカーブ(1.0=線形)
        bool oscillateColor = false;           // 色を振動させるか
    };

    ///=====================================================/// 
    /// パーティクルスケールアニメーション設定
    ///=====================================================///
    struct ParticleScaleAnimation {
        enum class AnimationType {
            None,           // アニメーションなし
            Linear,         // 線形
            EaseIn,         // 徐々に大きく
            EaseOut,        // 徐々に小さく
            Bounce,         // 膨らんで縮む
            Pulse           // 脈動
        };

        AnimationType type = AnimationType::Bounce;
        float startScale = 0.3f;               // 開始スケール
        float maxScale = 1.5f;                 // 最大スケール
        float endScale = 0.1f;                 // 終了スケール
    };

    ///-------------------------------------------/// 
    /// パーティクル高度設定
    ///-------------------------------------------///
    struct ParticleAdvancedSettings {
        ParticleMotionSettings motion;
        ParticleEmissionPattern emissionPattern;
        ParticleColorGradient colorGradient;
        ParticleScaleAnimation scaleAnimation;

        // 軌跡特有の設定
        bool isTrajectoryParticle = false;     // 軌跡パーティクルモード
        float trailSpacing = 0.008f;           // 軌跡の間隔(秒)
        bool clearOnStop = true;               // 停止時にクリアするか
    };
}