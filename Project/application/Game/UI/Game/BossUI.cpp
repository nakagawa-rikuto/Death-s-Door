#include "BossUI.h"
// BossEnemy
#include <application/Game/Entity/Enemy/BossEnemy/BossEnemy.h>
// Service
#include <Service/GraphicsResourceGetter.h>
#include <Service/Sprite.h>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
BossUI::~BossUI() {
	hpUI_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void BossUI::Initialize(BossEnemy * boss) {
	// ボスの取得
	boss_ = boss;

	/// ===HPUI=== ///
	// 位置とサイズの設定
	Vector2 hpUIPosition = { 50.0f, 50.0f };
	Vector2 hpUISize = { 900.0f, 80.0f };
	//　生成
	hpUI_ = std::make_unique<HpUI>();
	float maxHp = static_cast<float>(boss_->GetHP());
	hpUI_->Initialize(hpUIPosition, hpUISize, maxHp);
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BossUI::Update() {
	// HPUIの更新
	float currentHp = static_cast<float>(boss_->GetHP());
	hpUI_->Update(boss_->GetDeltaTime(), currentHp);
}
