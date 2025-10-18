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

    /// <summary>
    /// プレイヤーを設定
    /// </summary>
    /// <param name="player">設定する Player オブジェクトへのポインタ。</param>
    void SetPlayer(Player* player);

    /// <summary>
    /// 指定した種類の敵を指定した位置に出現
    /// </summary>
    /// <param name="type">出現させる敵の種類を表す値。</param>
    /// <param name="pos">出現位置。位置を表す Vector3 への const 参照（読み取り専用）。</param>
    /// <returns>生成された BaseEnemy オブジェクトへのポインター。出現に失敗した場合は nullptr を返すことがある。</returns>
    BaseEnemy* Spawn(EnemyType type, const Vector3& pos);

    // ライフサイクル

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="mode">描画に使用するブレンドモード。省略した場合は BlendMode::KBlendModeNormal が使用されます。</param>
    void Draw(BlendMode mode = BlendMode::KBlendModeNormal);

    /// <summary>
	/// ImGui情報の表示
    /// </summary>
    void UpdateImGui();

    /// <summary>
    /// オブジェクトの内部状態や保持しているデータをクリア
    /// </summary>
    void Clear();

    /// <summary>
    /// 指定した敵の種類にソース敵の設定を適用
    /// </summary>
    /// <param name="type">設定を適用する対象の敵の種類を表す値。</param>
    /// <param name="sourceEnemy">設定を取得する元となる敵を指す const ポインタ（このオブジェクトから設定がコピーされます）。</param>
    void ApplySettingsToType(EnemyType type, const BaseEnemy* sourceEnemy);

    /// <summary>
    /// 現存する全ての敵にソース敵の設定を適用
    /// </summary>
    /// <param name="sourceEnemy">設定を取得する元となる敵を指す const ポインタ</param>
    void ApplySettingsToAll(const BaseEnemy* sourceEnemy);

    // 指定タイプの代表敵を取得（調整用）

    /// <summary>
    /// 
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
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