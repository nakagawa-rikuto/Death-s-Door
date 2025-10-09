#pragma once
/// ===前方宣言=== ///
class BaseEnemy;

///=====================================================/// 
/// エネミーのベースステート
///=====================================================///
class EnemyState {
public:
	virtual ~EnemyState() = default;

	// 状態に入ったときに呼ばれる
	virtual void Enter(BaseEnemy* enemy) = 0;

	// 状態の更新処理
	virtual void Update(BaseEnemy* enemy) = 0;

	// 終了処理
	virtual void Finalize();

protected:

	BaseEnemy* enemy_ = nullptr;
};

