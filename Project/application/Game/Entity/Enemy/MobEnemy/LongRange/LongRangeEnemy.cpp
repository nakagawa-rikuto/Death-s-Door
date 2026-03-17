#include "LongRangeEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
LongRangeEnemy::~LongRangeEnemy() {
	// Bulletのクリア
	bulletInfo_.bullets_.clear();
}

///-------------------------------------------/// 
/// GameScene用初期化
///-------------------------------------------///
void LongRangeEnemy::InitGameScene(const Vector3& translate) {

	/// ===LongRangeEnemyの初期化=== ///
	Initialize();

	/// ===MobEnemyのInitGameScene=== ///
	MobEnemy::InitGameScene(translate);

	/// ===AttackInfoの設定=== ///
	attackInfo_.range = 3.0f;
	attackInfo_.distance = 30.0f;
	attackInfo_.interval = 8.0f;
	attackInfo_.timer = 1.0;
	attackInfo_.power = 1;
	attackInfo_.direction = { 0.0f, 0.0f, 0.0f };
	attackInfo_.isAttack = false;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void LongRangeEnemy::Initialize() {

	/// ===Object3d=== ///
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "LongEnemy");

	/// ===MobEnemyの初期化=== ///
	MobEnemy::Initialize();

	/// ===OBBのサイズを設定=== ///
	SetHalfSize({ 1.75f, 1.75f, 0.75f });
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void LongRangeEnemy::Update() {

	// MobEnemyの更新
	MobEnemy::Update();

	// 弾を更新（null ならスキップ）
	for (auto& bullet : bulletInfo_.bullets_) {
		if (bullet) bullet->Update();
	}

	// 死んだ弾をリストから削除
	bulletInfo_.bullets_.remove_if(
		[](const std::unique_ptr<LongRangeEnemyBullet>& b) {
			return !b || !b->GetIsAlive();
		}
	);
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void LongRangeEnemy::Draw(MiiEngine::BlendMode mode) {
	// Enemyの描画
	MobEnemy::Draw(mode);

	// バレットの描画
	for (auto& bullet : bulletInfo_.bullets_) {
		if (bullet) bullet->Draw(mode);
	}
}

///-------------------------------------------/// 
/// 更新（ImGui）
///-------------------------------------------///
void LongRangeEnemy::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("LongRangeEnemy");
	MobEnemy::Information();
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void LongRangeEnemy::OnCollision(MiiEngine::Collider* collider) {
	// MobEnemyの当たり判定
	MobEnemy::OnCollision(collider);
}

///-------------------------------------------/// 
/// 攻撃処理の初期化
///-------------------------------------------///
void LongRangeEnemy::StartAttack() {
	// 攻撃開始
	attackInfo_.isAttack = true;
	// 攻撃時間の設定
	attackInfo_.timer = 1.0f;
}

///-------------------------------------------/// 
/// 攻撃処理
///-------------------------------------------///
void LongRangeEnemy::Attack() {

	// 弾の生成
	auto bullet = std::make_unique<LongRangeEnemyBullet>();
	bullet->Create(transform_.translate, attackInfo_.direction, player_);
	bulletInfo_.bullets_.push_back(std::move(bullet));

	// フラグをfalse
	attackInfo_.isAttack = false;
	// クールダウン再設定
	attackInfo_.timer = attackInfo_.interval; 
}