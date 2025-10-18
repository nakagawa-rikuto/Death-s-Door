#pragma once
/// ===前方宣言=== ///
class BaseEnemy;

///=====================================================/// 
/// エネミーのベースステート
///=====================================================///
class EnemyState {
public:
	virtual ~EnemyState() = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理、純粋仮想関数
	/// </summary>
	/// <param name="enemy">処理対象の BaseEnemy オブジェクトへのポインタ。ステートへ入る対象となる敵を指す</param>
	virtual void Enter(BaseEnemy* enemy) = 0;

	/// <summary>
	/// 更新処理、純粋仮想関数
	/// </summary>
	/// <param name="enemy">更新対象の BaseEnemy 型オブジェクトへのポインタ。</param>
	virtual void Update(BaseEnemy* enemy) = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

protected:

	BaseEnemy* enemy_ = nullptr;
};

