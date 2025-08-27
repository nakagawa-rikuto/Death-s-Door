#pragma once
/// ===Include=== ///
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
#include <memory>
#include <vector>

// 前方宣言
class Player;

/// ===enum=== ///
enum class EnemyType {
    CloseRange,
    LongRange
};

///=====================================================/// 
/// EnemyManager
///=====================================================///
class EnemyManager {
public:
    EnemyManager() = default;
    ~EnemyManager() = default;

    // プレイヤーを設定（既存と今後にスポーンする全敵へ反映）
    void SetPlayer(Player* player);

    // 敵を1体スポーン（enumと座標を受け取り、該当敵のInitGameSceneを呼ぶ）
    BaseEnemy* Spawn(EnemyType type, const Vector3& pos);

    // ライフサイクル
    void Update();
    void Draw(BlendMode mode = BlendMode::KBlendModeNormal);
    void UpdateImGui();

    // 全削除
    void Clear();

    // 指定タイプの全敵に設定をコピー
    void ApplySettingsToType(EnemyType type, const BaseEnemy* sourceEnemy);

    // 全敵に設定をコピー（タイプ問わず）
    void ApplySettingsToAll(const BaseEnemy* sourceEnemy);

    // 指定タイプの代表敵を取得（調整用）
    BaseEnemy* GetRepresentative(EnemyType type);

    // 指定タイプの代表敵の設定を、同タイプ全体に適用
    void ApplyRepresentativeSettingsToType(EnemyType type);

    // 指定タイプの代表敵の設定を、全敵に適用
    void ApplyRepresentativeSettingsToAll(EnemyType sourceType);

    // 指定タイプの敵数を取得
    int GetEnemyCount(EnemyType type) const;

    // 全敵数を取得
    int GetTotalEnemyCount() const;

private:
    Player* player_ = nullptr;
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;

    // タイプ判定
    EnemyType GetEnemyType(BaseEnemy* enemy) const;

public:
    // === テンプレート関数の実装 === //

    /// 指定タイプの代表敵を取得して調整用の関数を実行後、同タイプ全体に適用
    template<typename Func>
    void ModifyAndApplyToType(EnemyType type, Func adjustFunction) {
        BaseEnemy* representative = GetRepresentative(type);
        if (representative) {
            // 調整関数を実行
            adjustFunction(representative);
            // 同タイプ全体に適用
            ApplySettingsToType(type, representative);
        }
    }

    /// 指定タイプの代表敵を取得して調整用の関数を実行後、全敵に適用
    template<typename Func>
    void ModifyAndApplyToAll(EnemyType sourceType, Func adjustFunction) {
        BaseEnemy* representative = GetRepresentative(sourceType);
        if (representative) {
            // 調整関数を実行
            adjustFunction(representative);
            // 全敵に適用
            ApplySettingsToAll(representative);
        }
    }
};