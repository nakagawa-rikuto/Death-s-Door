#include "BossAttackManager.h"
// C++
#include <cassert>
#include <algorithm>
// Math
#include <Math/sMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

#ifdef USE_IMGUI
///-------------------------------------------///
/// 設定の適用
///-------------------------------------------///
void BossAttackManager::ApplyConfig(const Config& newConfig) {
	config_ = newConfig;
	// 各コンポーネントにも設定を反映
	rotate_->ApplyConfig(config_.rotateConfig);
	downswing_->ApplyConfig(config_.downswingConfig);
	jumpSmash_->ApplyConfig(config_.jumpSmashConfig);
	orbitingOrbs_->ApplyConfig(config_.orbitingOrbsConfig);
}
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
BossAttackManager::~BossAttackManager() {
	// Componentの解放
	rotate_.reset();
	downswing_.reset();
	jumpSmash_.reset();
	orbitingOrbs_.reset();
	parabolicShot_.reset();
	// Bulletの解放
	for (auto& b : orbitingBullets_) {
		b.reset();
	}
	parabolicShotBullets_.clear();
}

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackManager::Initialize(const Config& config) {
	config_ = config;
	cooldowns_ = Cooldowns{};

	/// ===各コンポーネントの生成と初期化=== ///
	// 回転攻撃コンポーネントの生成と初期化
	rotate_ = std::make_unique<BossAttackRotateComponent>();
	rotate_->Initialize(config_.rotateConfig);
	// 振り下ろし攻撃コンポーネントの生成と初期化
	downswing_ = std::make_unique<BossAttackDownwardSwingComponent>();
	downswing_->Initialize(config_.downswingConfig);
	// ジャンプ叩きつけ攻撃コンポーネントの生成と初期化
	jumpSmash_ = std::make_unique<BossAttackJumpSmashComponent>();
	jumpSmash_->Initialize(config_.jumpSmashConfig);
	// 周りを回るオーブ攻撃コンポーネントの生成と初期化
	orbitingOrbs_ = std::make_unique<BossAttackOrbitingOrbsComponent>();
	orbitingOrbs_->Initialize(config_.orbitingOrbsConfig);
	// 放物線ショット攻撃コンポーネントの生成と初期化
	parabolicShot_ = std::make_unique<BossAttackParabolicShotComponent>();
	parabolicShot_->Initialize(config_.parabolicShotConfig);
	// OrbitingBulletの生成
	for (auto& b : orbitingBullets_) {
		b = std::make_unique<BossEnemyBullet>();
		b->Initialize();
	}
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BossAttackManager::Update(const Vector3& bossPosition, const Vector3& playerPosition, float deltaTime) {

	/// ===Timer=== ///
	UpdateTimers(deltaTime);

	/// ===Orbiting Orbs=== ///
	if (orbitingOrbs_->IsActive()) {
		// コンテキストの作成と更新の実行
		BossAttackOrbitingOrbsComponent::UpdateContext context{
			.bossPosition = bossPosition,
			.deltaTime = deltaTime
		};
		BossAttackOrbitingOrbsComponent::UpdateResult result = orbitingOrbs_->Update(context);

		if (result.isOrbiting) {
			for (int i = 0; i < BossAttackOrbitingOrbsComponent::kOrbCount; ++i) {
				auto& bullet = orbitingBullets_[i];
				if (bullet && bullet->GetIsAlive()) {
					// 弾の位置を更新
					orbitingBullets_[i]->SetVelocity(result.bullets[i].direction);
				}
			}
		}
	}

	/// ===Parabolic Shot=== ///
	if (parabolicShot_->IsActive() && !parabolicShotBullets_.empty()) {
		auto& currentBullet = parabolicShotBullets_.back();
		// コンテキストの作成と更新の実行
		BossAttackParabolicShotComponent::UpdateContext context{
			.bulletPosition = currentBullet->GetTransform().translate,
			.bossPosition = bossPosition,
			.targetPosition = playerPosition,
			.deltaTime = deltaTime
		};
		BossAttackParabolicShotComponent::UpdateResult result = parabolicShot_->Update(context);

		if (result.isFlying) {
			// 弾の位置を更新
			currentBullet->SetVelocity(result.velocity);
		} else if (result.isTrembling) {
			currentBullet->SetTranslate(result.position);
		}

		// 地面に到達していれば弾を非アクティブにする
		if (parabolicShot_->IsHitGround()) {
			currentBullet->SetAlive(false);
		}

		if (result.isFinished) {
			currentBullet->SetAlive(false); // 生存時間切れなどの場合にも念のため消去
			parabolicShot_->Reset();
		}
	}

	/// ===弾の更新と削除=== ///
	for (auto& b : orbitingBullets_) { 
		if (b && b->GetIsAlive()) {
			b->Update(); 
		}
	}

	// ParabolicShotの弾の更新
	for (auto& b : parabolicShotBullets_) {
		if (b->GetIsAlive()) {
			b->Update();
		}
	}
	// 非アクティブになった弾を削除する
	parabolicShotBullets_.erase(
		std::remove_if(parabolicShotBullets_.begin(), parabolicShotBullets_.end(),
			[](const std::unique_ptr<BossEnemyBullet>& b) { return !b->GetIsAlive(); }),
		parabolicShotBullets_.end());
}



///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackManager::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("攻撃マネージャー")) {
		// クールダウン表示
		if (ImGui::TreeNode("クールダウン")) {
			ImGui::Text("Rotate:      %.2f / %.2f", cooldowns_.rotate, config_.rotateCooldown);
			ImGui::Text("DownSwing:   %.2f / %.2f", cooldowns_.downswing, config_.downswingCooldown);
			ImGui::Text("JumpSmash:   %.2f / %.2f", cooldowns_.jumpSmash, config_.jumpSmashCooldown);
			ImGui::Text("OrbitingOrbs:%.2f / %.2f", cooldowns_.orbitingOrbs, config_.orbitingOrbsCooldown);
			ImGui::Text("ParabolicShot:%.2f / %.2f", cooldowns_.parabolicShot, config_.parabolicShotCooldown);
			ImGui::TreePop();
		}

		// 射程設定
		if (ImGui::TreeNode("射程設定")) {
			ImGui::DragFloat("Thrust 射程", &config_.rotateRange, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("DownSwing 射程", &config_.downswingRange, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("JumpSmash 最小射程", &config_.jumpSmashMinRange, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("JumpSmash 最大射程", &config_.jumpSmashMaxRange, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("OrbitingOrbs 射程", &config_.orbitingOrbsRange, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("ParabolicShot 射程", &config_.parabolicShotRange, 0.1f, 0.0f, 50.0f);
			ImGui::TreePop();
		}

		// クールダウン設定
		if (ImGui::TreeNode("クールダウン設定")) {
			ImGui::DragFloat("Thrust CT", &config_.rotateCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("DownSwing CT", &config_.downswingCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("JumpSmash CT", &config_.jumpSmashCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("OrbitingOrbs CT", &config_.orbitingOrbsCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("ParabolicShot CT", &config_.parabolicShotCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 各攻撃コンポーネントの詳細
		if (rotate_) rotate_->Information();
		if (downswing_) downswing_->Information();
		if (jumpSmash_) jumpSmash_->Information();
		if (orbitingOrbs_) orbitingOrbs_->Information();
		if (parabolicShot_) parabolicShot_->Information();

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// Orbiting Orbs攻撃の開始
///-------------------------------------------///
void BossAttackManager::StartOrbitingOrbs(const Vector3& bossPos) {
	// 攻撃開始
	orbitingOrbs_->StartAttack();

	// 初期位置を設定して弾を生成
	for (int i = 0; i < BossAttackOrbitingOrbsComponent::kOrbCount; ++i) {
		BossAttackOrbitingOrbsComponent::UpdateContext ctx{ bossPos, 0.0f };
		auto result = orbitingOrbs_->Update(ctx);

		orbitingBullets_[i]->Create(
			result.bullets[i].position,
			config_.orbitingOrbsConfig.lifetime);
	}

	// クールダウン開始
	StartOrbitingOrbsCooldown();
}

///-------------------------------------------/// 
/// 放物線ショット攻撃の開始
///-------------------------------------------///
void BossAttackManager::StartParabolicShot(const Vector3 & bossPos, float groundY) {
	// 攻撃開始
	parabolicShot_->StartAttack(groundY);

	// 弾を生成して初期化・配置
	auto bullet = std::make_unique<BossEnemyBullet>();
	bullet->Initialize();
	float totalLifetime = config_.parabolicShotConfig.lifetime + config_.parabolicShotConfig.trembleDuration;
	bullet->Create(bossPos, totalLifetime);

	// リストに追加
	parabolicShotBullets_.push_back(std::move(bullet));

	// クールダウン開始
	StartParabolicShotCooldown();
}

///-------------------------------------------///
/// タイマー更新
///-------------------------------------------///
void BossAttackManager::UpdateTimers(float deltaTime) {
	cooldowns_.rotate = (std::max)(0.0f, cooldowns_.rotate - deltaTime);
	cooldowns_.downswing = (std::max)(0.0f, cooldowns_.downswing - deltaTime);
	cooldowns_.jumpSmash = (std::max)(0.0f, cooldowns_.jumpSmash - deltaTime);
	cooldowns_.orbitingOrbs = (std::max)(0.0f, cooldowns_.orbitingOrbs - deltaTime);
	cooldowns_.parabolicShot = (std::max)(0.0f, cooldowns_.parabolicShot - deltaTime);
}