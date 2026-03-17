#pragma once
/// ===Include=== ///
#include "BaseEnemy.h"
#include <memory>
#include <vector>

// 前方宣言
class Player;

/// ===enum=== ///
enum class EnemyType {
    CloseRange,
    LongRange,
    Boss,
};

///=====================================================/// 
/// EnemyManager
///=====================================================///
class EnemyManager {
public:
    EnemyManager() = default;
    ~EnemyManager();

    /// <summary>
    /// プレイヤーを設定
    /// </summary>
    /// <param name="player">設定する Player オブジェクトへのポインタ。</param>
    void SetPlayer(Player* player);

    /// <summary>
    /// 指定したタイプ・位置・回転で敵を生成します。
    /// </summary>
    /// <param name="type">生成する敵の種類。EnemyType 型。</param>
    /// <param name="pos">生成位置。Vector3 の const リファレンス。</param>
    /// <param name="rot">生成時の回転。Quaternion の const リファレンス。</param>
    /// <returns>生成された敵インスタンスへのポインタ (BaseEnemy*)。</returns>
    BaseEnemy* Spawn(EnemyType type, const Vector3& pos, const Quaternion& rot);

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

	/// <summary>
	/// アニメーション時の更新処理
	/// </summary>
	void UpdateAnimation();

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="mode">描画に使用するブレンドモード。省略した場合は BlendMode::KBlendModeNormal が使用されます。</param>
    void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal);

    /// <summary>
	/// ImGui情報の表示
    /// </summary>
    void UpdateImGui();

    /// <summary>
    /// オブジェクトの内部状態や保持しているデータをクリア
    /// </summary>
    void Clear();

public: /// ===Getter=== ///
    
    // 指定タイプの敵数を取得
    int GetEnemyCount(EnemyType type) const;
    // 全敵数を取得
    int GetTotalEnemyCount() const;

private:
    // Player
    Player* player_ = nullptr;

	// BaseEnemyの配列
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;

    // タイプ判定
    EnemyType GetEnemyType(BaseEnemy* enemy) const;

#ifdef USE_IMGUI
private:
    /// <summary>
    /// 指定した敵タイプに対応する代表的な BaseEnemy インスタンスへのポインタを取得します。
    /// </summary>
    /// <param name="type">取得したい敵の種類を表す EnemyType 値。</param>
    /// <returns>指定したタイプに対応する BaseEnemy へのポインタ。該当する敵が存在しない場合は nullptr を返すことがあります。</returns>
    BaseEnemy* GetRepresentative(EnemyType type);
#endif // USE_IMGUI
};