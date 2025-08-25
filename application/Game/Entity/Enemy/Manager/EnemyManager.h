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

private:
    Player* player_ = nullptr;
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;
};