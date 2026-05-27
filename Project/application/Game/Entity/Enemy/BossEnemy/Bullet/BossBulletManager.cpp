#include "BossBulletManager.h"
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
void BossBulletManager::Initialize() {
	// Orbiting弾のインスタンスを生成
	for (auto& b : orbitingBullets_) {
		b = std::make_unique<BossEnemyBullet>();
		b->Initialize();
	}
}

///-------------------------------------------///
/// 更新処理
///-------------------------------------------///
void BossBulletManager::Update() {
	UpdateOrbitingBullets();
	UpdateParabolicBullets();
}

///-------------------------------------------///
/// 描画処理
///-------------------------------------------///
void BossBulletManager::Draw() {}

///-------------------------------------------///
/// Orbiting弾を生成（再利用）する
///-------------------------------------------///
void BossBulletManager::SpawnOrbitingBullets(const std::array<Vector3, BossAttackOrbitingOrbsComponent::kOrbCount>& positions, float lifetime) {
	for (int i = 0; i < BossAttackOrbitingOrbsComponent::kOrbCount; ++i) {
		orbitingBullets_[i]->Create(positions[i], lifetime);
	}
}

///-------------------------------------------///
/// Orbiting弾の速度ベクトルを一括設定する
///-------------------------------------------///
void BossBulletManager::SetOrbitingVelocities( const std::array<Vector3, BossAttackOrbitingOrbsComponent::kOrbCount>& velocities) {
	for (int i = 0; i < BossAttackOrbitingOrbsComponent::kOrbCount; ++i) {
		if (orbitingBullets_[i] && orbitingBullets_[i]->GetIsAlive()) {
			orbitingBullets_[i]->SetVelocity(velocities[i]);
		}
	}
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
void BossBulletManager::UpdateOrbitingBullets() {
	for (auto& b : orbitingBullets_) {
		if (b && b->GetIsAlive()) {
			b->Update();
		}
	}
}

///-------------------------------------------///
/// ParabolicShot弾の更新処理・削除処理
///-------------------------------------------///
void BossBulletManager::UpdateParabolicBullets() {

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
