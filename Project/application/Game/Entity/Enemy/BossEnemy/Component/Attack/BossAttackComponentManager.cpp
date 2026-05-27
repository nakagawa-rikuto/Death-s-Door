#include "BossAttackComponentManager.h"
// BulletManager
#include "application/Game/Entity/Enemy/BossEnemy/Bullet/BossBulletManager.h"
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
void BossAttackComponentManager::ApplyConfig(const Config& newConfig) {
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
BossAttackComponentManager::~BossAttackComponentManager() {
	// Componentの解放
	rotate_.reset();
	downswing_.reset();
	jumpSmash_.reset();
	orbitingOrbs_.reset();
	parabolicShot_.reset();
}

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackComponentManager::Initialize(const Config& config, BossBulletManager* bulletManager) {
	config_ = config;
	cooldowns_ = Cooldowns{};
	bulletManager_ = bulletManager;

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
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BossAttackComponentManager::Update(const Vector3& bossPosition, const Vector3& playerPosition, float deltaTime) {

	/// ===Timer=== ///
	UpdateTimers(deltaTime);

	/// ===Orbiting Orbs=== ///
	UpdateOrbiting(bossPosition, deltaTime);;

	/// ===Parabolic Shot=== ///
	UpdateParabolicShot(bossPosition, playerPosition, deltaTime);
}


///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackComponentManager::Information() {
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
void BossAttackComponentManager::StartOrbitingOrbs(const Vector3& bossPos) {
	// 攻撃開始
	orbitingOrbs_->StartAttack();

	// 弾の初期位置を入れるための配列
	std::array<Vector3, BossAttackOrbitingOrbsComponent::kOrbCount> bulletPositions;

	// 初期位置を設定して弾を生成
	for (int i = 0; i < BossAttackOrbitingOrbsComponent::kOrbCount; ++i) {
		BossAttackOrbitingOrbsComponent::UpdateContext ctx{ bossPos, 0.0f };
		auto result = orbitingOrbs_->Update(ctx);

		// 初期位置を配列に格納
		bulletPositions[i] = result.bullets[i].position;
	}

	// BulletManagerに弾の生成を依頼
	bulletManager_->SpawnOrbitingBullets(bulletPositions, config_.orbitingOrbsConfig.lifetime);

	// クールダウン開始
	StartOrbitingOrbsCooldown();
}

///-------------------------------------------/// 
/// 放物線ショット攻撃の開始
///-------------------------------------------///
void BossAttackComponentManager::StartParabolicShot(const Vector3 & bossPos, float groundY) {
	// 攻撃開始
	parabolicShot_->StartAttack(groundY);
	// 弾の生存時間の計算
	float totalLifetime = config_.parabolicShotConfig.lifetime + config_.parabolicShotConfig.trembleDuration;

	// BulletManagerに弾の生成を依頼
	bulletManager_->SpawnParabolicBullets(bossPos, totalLifetime);

	// クールダウン開始
	StartParabolicShotCooldown();
}

///-------------------------------------------///
/// タイマー更新
///-------------------------------------------///
void BossAttackComponentManager::UpdateTimers(float deltaTime) {
	cooldowns_.rotate = (std::max)(0.0f, cooldowns_.rotate - deltaTime);
	cooldowns_.downswing = (std::max)(0.0f, cooldowns_.downswing - deltaTime);
	cooldowns_.jumpSmash = (std::max)(0.0f, cooldowns_.jumpSmash - deltaTime);
	cooldowns_.orbitingOrbs = (std::max)(0.0f, cooldowns_.orbitingOrbs - deltaTime);
	cooldowns_.parabolicShot = (std::max)(0.0f, cooldowns_.parabolicShot - deltaTime);
}

///-------------------------------------------/// 
/// Orbiting Orbs攻撃の弾の更新
///-------------------------------------------///
void BossAttackComponentManager::UpdateOrbiting(const Vector3& bossPosition, float deltaTime) {
	if (orbitingOrbs_->IsActive()) {
		// コンテキストの作成と更新の実行
		BossAttackOrbitingOrbsComponent::UpdateContext context{
			.bossPosition = bossPosition,
			.deltaTime = deltaTime
		};
		BossAttackOrbitingOrbsComponent::UpdateResult result = orbitingOrbs_->Update(context);

		std::array<Vector3, BossAttackOrbitingOrbsComponent::kOrbCount> bulletVelocity;

		// 公転中であれば弾の位置を更新
		if (result.isOrbiting) {
			for (int i = 0; i < BossAttackOrbitingOrbsComponent::kOrbCount; ++i) {
				// 弾の位置を更新
				bulletVelocity[i] = result.bullets[i].direction;
			}

			// BulletManagerに弾の位置更新を依頼
			bulletManager_->SetOrbitingVelocities(bulletVelocity);
		}
	}
}

///-------------------------------------------/// 
/// 放物線ショット攻撃の弾の更新
///-------------------------------------------///
void BossAttackComponentManager::UpdateParabolicShot(const Vector3& bossPosition, const Vector3& playerPosition, float deltaTime) {
	if (parabolicShot_->IsActive()) {
		// コンテキストの作成と更新の実行
		BossAttackParabolicShotComponent::UpdateContext context{
			.bulletPosition = bulletManager_->GetParabolicBulletPosition(),
			.bossPosition = bossPosition,
			.targetPosition = playerPosition,
			.deltaTime = deltaTime
		};
		BossAttackParabolicShotComponent::UpdateResult result = parabolicShot_->Update(context);

		if (result.isFlying) {
			// 弾の位置を更新
			bulletManager_->SetParabolicVelocity(result.velocity);
		}

		// 地面に到達していれば弾を非アクティブにする
		if (parabolicShot_->IsHitGround()) {
			bulletManager_->KillLatestParabolicBullets();
		}

		if (result.isFinished) {
			bulletManager_->KillLatestParabolicBullets(); // 生存時間切れなどの場合にも念のため消去
			parabolicShot_->Reset();
		}
	}
}