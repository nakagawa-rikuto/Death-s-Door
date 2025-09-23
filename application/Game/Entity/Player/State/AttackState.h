/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// AttackState
///=====================================================///
class AttackState : public PlayerState {
public:

	~AttackState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(Player* player, GameCamera* camera) override;
	// 状態の更新処理
	void Update(Player* player, GameCamera* camera) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private:

	/// ===攻撃情報=== ///
	struct AttackInfo {
		// 攻撃目標のリスト(Enemy : マーク用)
		// 攻撃目標のリスト(Enemy : 攻撃用)
		float reachTime = 0.1f;               // 攻撃の最終座標までの到達時間
		float ratio = 0.0f;                   // 攻撃の移動割合
		const float freezeTime = 0.3f;		  // 攻撃完了から爆破までの時間
		float freezeTimer = 0.0f;					  // 
		Vector3 startPos = { 0.0f, 0.0f, 0.0f };					  // 攻撃の最初の座標
		Vector3 endPos = { 0.0f, 0.0f, 0.0f };						  // 攻撃の最終の座標
	};
	AttackInfo attackInfo_;
};