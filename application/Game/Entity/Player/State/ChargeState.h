/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// ChargeState
///=====================================================///
class ChargeState : public PlayerState {
public:
	ChargeState(const Vector3& direction);
	~ChargeState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(Player* player, Camera* camera) override;
	// 状態の更新処理
	void Update(Player* player, Camera* camera) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private: 

	/// ===突進情報=== ///
	struct ChargeInfo {
		float speed = 0.0f;			  // 突進時の移動速度
		float activeTime = 0.15f;      // 突進の有効時間
		float cooltime = 0.8f;		  // 突進のクールタイム
		float invincibleTime = 0.1f;  // 突進時の無敵時間
		float acceleration = 0.0f;    // 突進の加速度
		Vector3 direction = { 0.0f, 0.0f, 0.0f };
	};
	ChargeInfo info_;
};