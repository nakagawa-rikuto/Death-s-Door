#pragma once
/// ===Include=== ///
// Math
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Engine/DataInfo/LineObjectData.h"
// C++
#include <vector>
#include <string>

///-------------------------------------------/// 
/// 軌道の種類
///-------------------------------------------///
enum class TrajectoryType {
    WeaponTrajectory,    // 武器軌道
    RightHandTrajectory, // 右手軌道
    LeftHandTrajectory   // 左手軌道
};

///-------------------------------------------/// 
/// 攻撃データ構造体
///-------------------------------------------///
struct AttackData {
    // ===定数定義=== //
	static constexpr int kDefaultBezierSegments = 20;   // デフォルトのベジェ曲線分割数
	static constexpr float kDefaultWeaponLength = 8.0f; // デフォルトの武器距離
	static constexpr float kControlPointSize = 1.5f;    // 制御点の表示サイズ

    // ===基本情報=== //
    std::string attackName;              // 攻撃名
    std::string description;             // 説明
    int attackID;                        // 攻撃ID（ユニーク）

    // ===タイミング設定=== //
    float activeDuration;                // 攻撃のアクティブ時間（秒）
    float comboWindowTime;               // コンボ受付時間（秒）
    float cooldownTime;                  // クールダウン時間（秒）

    // ===軌道設定（ベジェ曲線）=== //
    bool isRightHandAttack;              // 右手攻撃かどうか
    bool isLeftHandAttack;               // 左手攻撃かどうか

    // 軌道
    std::vector<MiiEngine::BezierControlPointData> weaponTrajectoryPoints;    // 武器用軌道の制御点リスト
	std::vector<MiiEngine::BezierControlPointData> rightHandTrajectoryPoints; // 右手用軌道の制御点リスト
	std::vector<MiiEngine::BezierControlPointData> leftHandTrajectoryPoints;  // 左手用軌道の制御点リスト

    int curveSegments;                   // 曲線の分割数
    float weaponLength;                  // 武器の距離

    // ===コンボ設定=== //
    bool canComboToNext;                 // 次のコンボに繋げるか
    int nextComboID;                     // 次のコンボのID
    std::vector<int> branchComboIDs;     // 分岐可能なコンボID（複数選択肢）

    // ===エフェクト設定=== //
    std::string particleEffectName;      // パーティクルエフェクト名
    Vector3 particleOffset;              // パーティクルのオフセット
    float cameraShakeIntensity;          // カメラシェイクの強度
    float cameraShakeDuration;           // カメラシェイクの持続時間

    // ===サウンド設定=== //
    std::string swingSoundName;          // 振りサウンド名
    std::string hitSoundName;            // ヒット時サウンド名

    // ===プレイヤーの動き=== //
    float moveSpeedMultiplier;           // 攻撃中の移動速度倍率（0.0～1.0）
    Vector3 rootMotion;                  // ルートモーション（前進など）

    // ===デバッグ/プレビュー用=== //
    bool showTrajectory;                 // 軌道を表示するか
    Vector3 weaponColor;             // 軌道の色
    Vector3 rightHandColor;              // 右手軌道の色
    Vector3 leftHandColor;               // 左手軌道の色

    /// デフォルトコンストラクタ
    AttackData()
        : attackName("NewAttack")
        , description("")
        , attackID(0)
        , activeDuration(0.4f)
        , comboWindowTime(1.0f)
        , cooldownTime(0.3f)
        , isRightHandAttack(false)
        , isLeftHandAttack(false)
        , curveSegments(kDefaultBezierSegments)
        , weaponLength(kDefaultWeaponLength)
        , canComboToNext(false)
        , nextComboID(-1)
        , particleEffectName("WeaponAttack")
        , particleOffset(Vector3{ 0.0f, 0.0f, 0.0f })
        , cameraShakeIntensity(0.0f)
        , cameraShakeDuration(0.0f)
        , swingSoundName("")
        , hitSoundName("")
        , moveSpeedMultiplier(0.4f)
        , rootMotion(Vector3{ 0.0f, 0.0f, 0.0f })
        , showTrajectory(true)
        , weaponColor(Vector3{ 1.0f, 0.0f, 0.0f })     // 赤
        , rightHandColor(Vector3{ 0.0f, 1.0f, 0.0f })  // 緑
        , leftHandColor(Vector3{ 0.0f, 0.0f, 1.0f })   // 青
    {
        // デフォルトで3点のベジェ曲線を設定（左→中央→右）
        weaponTrajectoryPoints.push_back({ Vector3{-8.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},0.0f });
        weaponTrajectoryPoints.push_back({ Vector3{0.0f, 0.0f, 8.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},0.5f });
        weaponTrajectoryPoints.push_back({ Vector3{8.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},1.0f });

        // 両手用デフォルト軌道
        // 右手：右→前
        rightHandTrajectoryPoints.push_back({ Vector3{5.0f, 0.0f, -3.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},0.0f });
        rightHandTrajectoryPoints.push_back({ Vector3{5.0f, 2.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},0.5f });
        rightHandTrajectoryPoints.push_back({ Vector3{0.0f, 0.0f, 5.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},1.0f });

        // 左手：左→前
        leftHandTrajectoryPoints.push_back({ Vector3{-5.0f, 0.0f, -3.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.0f });
        leftHandTrajectoryPoints.push_back({ Vector3{-5.0f, 2.0f, 0.0f},Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.5f });
        leftHandTrajectoryPoints.push_back({ Vector3{0.0f, 0.0f, 5.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f},1.0f });
    }
};