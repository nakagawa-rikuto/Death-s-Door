#pragma once
/// ===前方宣言=== ///
class BossEnemy;

///=====================================================/// 
/// ボスのベースステート
/// 移動は決めていない、攻撃はジャンプ攻撃と回りながら進む処理、遠距離攻撃のモブを出しす
///=====================================================///
class BossState {
public:

	virtual ~BossState() = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理、純粋仮想関数
	/// </summary>
	/// <param name="boss">Bossのポインター</param>
	virtual void Enter(BossEnemy* boss) = 0;

	/// <summary>
	/// 更新処理、純粋仮想関数
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

protected:
	// Bossのポインター
	BossEnemy* boss_ = nullptr;
};

