#include "BossBulletManager.h"
// Component
#include "application/Game/Entity/Enemy/BossEnemy/Component/BossComponent.h"
// C++
#include <algorithm>

///-------------------------------------------///
/// デストラクタ
///-------------------------------------------///
BossBulletManager::~BossBulletManager() {
	// Orbiting弾の解放
	for (auto& b : orbitingBullets_) {
		b.reset();
	}
	// ParabolicShot弾の解放
	parabolicBullets_.clear();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
Vector3 BossBulletManager::GetParabolicBulletPosition() const {
	if (!parabolicBullets_.empty() && parabolicBullets_.back()->GetIsAlive()) {
		// 末尾の弾の位置を取得
		return parabolicBullets_.back()->GetTransform().translate;
	}
	return Vector3{ 0.0f, 0.0f, 0.0f }; // デフォルト位置
}

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossBulletManager::Initialize(const BossComponent::OrbitingAttackComponent& component) {
	// コンポーネントから値を取得
	orbitInfo_.bulletCount = component.bulletCount;
	orbitInfo_.lifeTime = component.lifeTime;
	orbitInfo_.orbitSpeed = component.orbitSpeed;

	// 途中で再確保が発生しないようにメモリを予約
	orbitingBullets_.reserve(orbitInfo_.bulletCount);

	// Orbiting弾のインスタンスを生成
	for (int i = 0; i < orbitInfo_.bulletCount; ++i) {
		orbitingBullets_.push_back(std::make_unique<BossEnemyBullet>());
		orbitingBullets_.back()->Initialize();
	}

	// OrbitBulletの位置と向きを計算するクラスを生成
	orbitCalculator_ = std::make_unique<BossOrbitingOrbsCalculator>();
	orbitCalculator_->Initialize(component);
}

///-------------------------------------------///
/// 更新処理
///-------------------------------------------///
void BossBulletManager::Update(float deltaTime) {
	// Orbiting弾の更新
	UpdateOrbitingBullets(deltaTime);
	// ParabolicShot弾の更新
	UpdateParabolicBullets(deltaTime);
}

///-------------------------------------------///
/// 描画処理
///-------------------------------------------///
void BossBulletManager::Draw() {}

///-------------------------------------------///
/// Orbiting弾を生成（再利用）する
///-------------------------------------------///
void BossBulletManager::SpawnOrbitingBullets() {
	// 計算情報の取得
	const auto& bulletInfos = orbitCalculator_->GetBulletInfo();

	// ループ回数の設定
	size_t updateCount = (std::min)(orbitingBullets_.size(), bulletInfos.size());

	for (int i = 0; i < updateCount; ++i) {
		orbitingBullets_[i]->Create(bulletInfos[i].position, orbitInfo_.lifeTime);
	}
	// アクティブ状態にする
	isOrbitingActive_ = true;
	// 攻撃開始
	orbitCalculator_->StartAttack();
}

///-------------------------------------------/// 
/// Orbiting弾を停止する
///-------------------------------------------///
void BossBulletManager::StopOrbitingBullets() {
	for (auto& b : orbitingBullets_) {
		if (b && b->GetIsAlive()) {
			b->SetAlive(false);
		}
	}
	// アクティブ状態を解除
	isOrbitingActive_ = false;
}

///-------------------------------------------///
/// ParabolicShot弾を生成
///-------------------------------------------///
void BossBulletManager::SpawnParabolicBullets(const Vector3& position, float lifetime) {
	// 新しい弾を生成して初期化・配置
	auto bullet = std::make_unique<BossEnemyBullet>();
	bullet->Initialize();
	bullet->Create(position, lifetime);
	// リストに追加
	parabolicBullets_.push_back(std::move(bullet));
}

///-------------------------------------------///
/// 末尾のParabolicShot弾の速度ベクトルを設定する
///-------------------------------------------///
void BossBulletManager::SetParabolicVelocity(const Vector3& velocity) {
	if (!parabolicBullets_.empty()) {
		parabolicBullets_.back()->SetVelocity(velocity);
	}
}

///-------------------------------------------///
/// 末尾のParabolicShot弾を非アクティブにする
///-------------------------------------------///
void BossBulletManager::KillLatestParabolicBullets() {
	if (!parabolicBullets_.empty()) {
		parabolicBullets_.back()->SetAlive(false);
	}
}

///-------------------------------------------///
/// Orbiting弾の更新処理
///-------------------------------------------///
void BossBulletManager::UpdateOrbitingBullets(float deltaTime) {
	// アクティブ状態のときのみ更新
	if (isOrbitingActive_) {
		// OrbitBulletの位置と向きを計算
		orbitCalculator_->Update(deltaTime);

		// 計算情報の取得
		const auto& bulletInfos = orbitCalculator_->GetBulletInfo();

		// ループ回数の設定
		size_t updateCount = (std::min)(orbitingBullets_.size(), bulletInfos.size());

		for (size_t i = 0; i < updateCount; ++i) {
			auto& b = orbitingBullets_[i];
			const auto& info = bulletInfos[i];
			// bulletが存在していたら
			if (b && b->GetIsAlive()) {
				Vector3 velocity = info.direction * orbitInfo_.orbitSpeed;
				b->SetVelocity(velocity);
				b->Update();
			}
		}
	}
}

///-------------------------------------------///
/// ParabolicShot弾の更新処理・削除処理
///-------------------------------------------///
void BossBulletManager::UpdateParabolicBullets(float deltaTime) {

	deltaTime;

	// 生存中の弾を更新
	for (auto& b : parabolicBullets_) {
		if (b && b->GetIsAlive()) {
			b->Update();
		}
	}

	// 非アクティブになった弾を削除
	parabolicBullets_.erase(
		std::remove_if(parabolicBullets_.begin(), parabolicBullets_.end(),
			[](const std::unique_ptr<BossEnemyBullet>& b) { return !b->GetIsAlive();}),
		parabolicBullets_.end());
}
