#include "EnemyManager.h"
#include "application/Game/Entity/Enemy/MobEnemy/CloseRange/CloseRangeEnemy.h"
#include "application/Game/Entity/Enemy/MobEnemy/LongRange/LongRangeEnemy.h"
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// c++
#include <algorithm> 
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void EnemyManager::SetPlayer(Player* player) {
	player_ = player;
}

///-------------------------------------------/// 
/// 生成
///-------------------------------------------///
BaseEnemy* EnemyManager::Spawn(EnemyType type, const Vector3& pos, const Quaternion& rot) {
	std::unique_ptr<BaseEnemy> enemy; // 最終的に enemies_ へ move する器

	switch (type) {
	case EnemyType::CloseRange: {
		auto e = std::make_unique<CloseRangeEnemy>();
		// 初期化
		e->InitGameScene(pos);		// 出現位置を渡してゲームシーン用初期化
		e->SetRotate(rot);			// 回転を設定
		enemy = std::move(e);		// 所有権移動
		break;
	}
	case EnemyType::LongRange: {
		auto e = std::make_unique<LongRangeEnemy>();
		// 初期化
		e->InitGameScene(pos);
		e->SetRotate(rot); 
		enemy = std::move(e);
		break;
	}
	case EnemyType::Boss: {
		auto e = std::make_unique<BossEnemy>();
		// 初期化
		e->InitGameScene(pos);
		e->SetRotate(rot); 
		enemy = std::move(e);
		break;
	}
	default:
		return nullptr; // 未知の enum なら生成しない
	}


	BaseEnemy* raw = enemy.get(); // 呼び出し側が直後に触れるように生ポインタを保持
	enemy->SetPlayer(player_); // Player をセット
	enemies_.push_back(std::move(enemy));
	return raw;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void EnemyManager::Update() {
	for (auto& e : enemies_) {
		if (e) {
			e->PreUpdate();
			e->Update();
		}
	}

	// 死亡した敵を削除
	auto it = std::remove_if(enemies_.begin(), enemies_.end(),
		[](const std::unique_ptr<BaseEnemy>& e) {
			return !e || e->GetIsTentativeDeath();
		});
	enemies_.erase(it, enemies_.end());
}

///-------------------------------------------/// 
/// アニメーション時の更新処理
///-------------------------------------------///
void EnemyManager::UpdateAnimation() {
	for (auto& e : enemies_) {
		if (e) {
			e->PreUpdate();
			e->UpdateAnimation();
		}
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void EnemyManager::Draw(MiiEngine::BlendMode mode) {
	for (auto& e : enemies_) {
		if (e) e->Draw(mode);
	}
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void EnemyManager::UpdateImGui() {
#ifdef USE_IMGUI

	// EnemyMangerのウィンドウ(ウィンドウを閉じた時に中身を書かないようにするため)
	if (ImGui::Begin("EnemyManager")) {

		// ===== 統計情報 ===== //
		int closeCount = GetEnemyCount(EnemyType::CloseRange);
		int longCount = GetEnemyCount(EnemyType::LongRange);
		int bossCount = GetEnemyCount(EnemyType::Boss);
		int totalCount = GetTotalEnemyCount();

		ImGui::Text("Enemy Statistics:");
		ImGui::Text("  CloseRange: %d", closeCount);
		ImGui::Text("  LongRange : %d", longCount);
		ImGui::Text("  Boss      : %d", bossCount);
		ImGui::Text("  Total     : %d", totalCount);
		ImGui::Separator();

		// ===== CloseRangeEnemy の調整 ===== //
		BaseEnemy* closeRep = GetRepresentative(EnemyType::CloseRange);
		if (closeRep) {
			ImGui::Text("CloseRange Enemy Settings:");
			closeRep->Information(); // 代表1体のImGuiを表示
		} else {
			ImGui::TextDisabled("No CloseRange enemies exist.");
		}

		ImGui::Separator();

		// ===== LongRangeEnemy の調整 ===== //
		BaseEnemy* longRep = GetRepresentative(EnemyType::LongRange);
		if (longRep) {
			ImGui::Text("LongRange Enemy Settings:");
			longRep->Information(); // 代表1体のImGuiを表示
		} else {
			ImGui::TextDisabled("No LongRange enemies exist.");
		}

		ImGui::Separator();

		// ===== 全体操作 ===== //
		ImGui::Text("Bulk Operations:");
		if (ImGui::Button("Clear All Enemies")) {
			Clear();
		}
	}
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// クリア
///-------------------------------------------///
void EnemyManager::Clear() {
	enemies_.clear(); // unique_ptr 破棄で各Enemyのデストラクトへ
}

///-------------------------------------------/// 
/// 指定タイプの敵数を取得
///-------------------------------------------///
int EnemyManager::GetEnemyCount(EnemyType type) const {
	int count = 0;
	for (const auto& e : enemies_) {
		if (!e) continue;

		if (GetEnemyType(e.get()) == type) {
			count++;
		}
	}
	return count;
}

///-------------------------------------------/// 
/// 全敵数を取得
///-------------------------------------------///
int EnemyManager::GetTotalEnemyCount() const {
	int count = 0;
	for (const auto& e : enemies_) {
		if (e) count++;
	}
	return count;
}

///-------------------------------------------/// 
/// タイプ判定
///-------------------------------------------///
EnemyType EnemyManager::GetEnemyType(BaseEnemy* enemy) const {
	if (dynamic_cast<CloseRangeEnemy*>(enemy)) {
		return EnemyType::CloseRange;
	} else if (dynamic_cast<LongRangeEnemy*>(enemy)) {
		return EnemyType::LongRange;
	}
	// デフォルト（本来ここは通らない想定）
	return EnemyType::CloseRange;
}

#ifdef USE_IMGUI
///-------------------------------------------/// 
/// 指定タイプの代表敵を取得
///-------------------------------------------///
BaseEnemy* EnemyManager::GetRepresentative(EnemyType type) {
	for (auto& e : enemies_) {
		if (!e) continue;

		if (GetEnemyType(e.get()) == type) {
			return e.get();
		}
	}
	return nullptr;
}
#endif // USE_IMGUI

