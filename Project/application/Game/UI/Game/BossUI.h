#pragma once
/// ===Include=== ///
#include <application/Drawing/2d/Object2d.h>
#include <application/Game/UI/HP/HpUI.h>

/// ===前方宣言=== ///
class BossEnemy;

///=====================================================/// 
/// ボスUI
///=====================================================///
class BossUI {
public:

	BossUI() = default;
	~BossUI();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="boss">初期化するボスエネミーへのポインタ。</param>
	void Initialize(BossEnemy* boss, const Vector2& windowSize);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private:
	// ボスエネミーへのポインタ
	BossEnemy* boss_;

	/// ===Object2D=== ///
	// HPUI
	std::unique_ptr<HpUI> hpUI_;
};

