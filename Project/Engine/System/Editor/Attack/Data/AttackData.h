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
/// 軌道のチャンネルデータ構造体
///-------------------------------------------///
struct TrajectoryChannel {
    std::string name; // チャンネル名
    Vector3 color = { 1.0f, 1.0f, 1.0f };   // チャンネルの表示色
    bool enabled = true;     // チャンネルが有効かどうか
    std::vector<MiiEngine::BezierControlPointData> points; // ベジェ曲線の制御点リスト

    TrajectoryChannel() = default;
    TrajectoryChannel(const std::string& channelName, const Vector3& channelColor) : name(channelName), color(channelColor), enabled(true) {
        // デフォルトで3点の制御点を設定
        points.push_back({ Vector3{-8.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.0f });
    }
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

    /// ===軌道設定=== ///
    std::vector<TrajectoryChannel> trajectories;
    int curveSegments;                   // 曲線の分割数
    float weaponLength;                  // 武器の距離

    // ===コンボ設定=== //
    bool canComboToNext;                 // 次のコンボに繋げるか
    int nextComboID;                     // 次のコンボのID
    std::vector<int> branchComboIDs;     // 分岐可能なコンボID（複数選択肢）

    // ===プレイヤーの動き=== //
    float moveSpeedMultiplier;           // 攻撃中の移動速度倍率（0.0～1.0）
    Vector3 rootMotion;                  // ルートモーション（前進など）

    // ===デバッグ/プレビュー用=== //
    bool showTrajectory;                 // 軌道を表示するか

    /// <summary>
    /// インデックスで軌道チャンネルを取得
    /// </summary>
    TrajectoryChannel* GetChannel(int index) {
        if (index < 0 || index >= static_cast<int>(trajectories.size())) return nullptr;
        return &trajectories[index];
    }
    const TrajectoryChannel* GetChannel(int index) const {
        if (index < 0 || index >= static_cast<int>(trajectories.size())) return nullptr;
        return &trajectories[index];
    }

    /// <summary>
    /// 名前デ軌道チャンネルを検索
    /// </summary>
    TrajectoryChannel* FindChannel(const std::string& channelName) {
        for (auto& ch : trajectories) {
            if (ch.name == channelName) return &ch;
        }
        return nullptr;
    }

    /// ===デフォルトコンストラクタ=== ///
    AttackData()
        : attackName("NewAttack")
        , description("")
        , attackID(0)
        , activeDuration(0.4f)
        , comboWindowTime(1.0f)
        , cooldownTime(0.3f)
        , curveSegments(kDefaultBezierSegments)
        , weaponLength(kDefaultWeaponLength)
        , canComboToNext(false)
        , nextComboID(-1)
        , moveSpeedMultiplier(0.4f)
        , rootMotion(Vector3{ 0.0f, 0.0f, 0.0f })
        , showTrajectory(true) {

        // デフォルト軌道チャンネルを作成
        TrajectoryChannel weapon("Weapon", Vector3{ 1.0f, 0.0f, 0.0f });
        weapon.points.clear();
        weapon.points.push_back({ Vector3{-8.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.0f });
        weapon.points.push_back({ Vector3{ 0.0f, 0.0f, 8.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 0.5f });
        weapon.points.push_back({ Vector3{ 8.0f, 0.0f, 0.0f}, Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f });
        trajectories.push_back(weapon);
    }
};