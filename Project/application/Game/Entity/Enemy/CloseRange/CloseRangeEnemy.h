#pragma once
/// ===Include=== ///
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"

///=====================================================/// 
/// 近接戦闘Enemy
///=====================================================///
class CloseRangeEnemy : public BaseEnemy {
public:
	CloseRangeEnemy() = default;
	~CloseRangeEnemy();

	/// <summary>
	/// ゲームシーンで呼び出す初期化処理
	/// </summary>
	/// <param name="translate">シーンの位置や平行移動を表す Vector3 型の const 参照。初期化時の位置や変換を指定します。</param>
	void InitGameScene(const Vector3& translate)override;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;
	
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">適用するブレンドモード。省略した場合は BlendMode::KBlendModeNormal が使用されます。</param>
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information()override;

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack()override;

public: /// ===衝突判定=== ///
	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="collider">衝突した相手を表す Collider へのポインター。</param>
	void OnCollision(Collider* collider) override;

private: /// ===変数=== ///

	/// ===突進情報=== ///
	struct ChargeInfo {
		float moveSpeed;
		float stopTime;
	};
	ChargeInfo chargeInfo_;

protected:

	/// <summary>
	/// 派生側で型固有のチューニング値をコピーするための関数
	/// </summary>
	/// <param name="dst"></param>
	void CopyTypeTuningFromThisTo(BaseEnemy* dst) const override;
};
