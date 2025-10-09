#include "EnemyManager.h"
#include "application/Game/Entity/Enemy/CloseRange/CloseRangeEnemy.h"
#include "application/Game/Entity/Enemy/LongRange/LongRangeEnemy.h"
// c++
#include <algorithm> 
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void EnemyManager::SetPlayer(Player* player) {
	player_ = player;
	for (auto& e : enemies_) {
		if (e) e->SetPlayer(player_);
	}
}

///-------------------------------------------/// 
/// 生成
///-------------------------------------------///
BaseEnemy* EnemyManager::Spawn(EnemyType type, const Vector3& pos) {
	std::unique_ptr<BaseEnemy> enemy; // 最終的に enemies_ へ move する器

	switch (type) {
	case EnemyType::CloseRange: {
		auto e = std::make_unique<CloseRangeEnemy>();
		if (player_) { // 初フレームから Update が進むようにプレイヤー注入
			e->SetPlayer(player_);
		}
		e->InitGameScene(pos); // 出現位置を渡してゲームシーン用初期化
		enemy = std::move(e);
		break;
	}
	case EnemyType::LongRange: {
		auto e = std::make_unique<LongRangeEnemy>();
		if (player_) {
			e->SetPlayer(player_);
		}
		e->InitGameScene(pos);
		enemy = std::move(e);
		break;
	}
	default:
		return nullptr; // 未知の enum なら生成しない
	}


	BaseEnemy* raw = enemy.get(); // 呼び出し側が直後に触れるように生ポインタを保持
	enemies_.push_back(std::move(enemy));
	return raw;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void EnemyManager::Update() {
	for (auto& e : enemies_) {
		if (e) e->Update();
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void EnemyManager::Draw(BlendMode mode) {
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
		int totalCount = GetTotalEnemyCount();

		ImGui::Text("Enemy Statistics:");
		ImGui::Text("  CloseRange: %d", closeCount);
		ImGui::Text("  LongRange : %d", longCount);
		ImGui::Text("  Total     : %d", totalCount);
		ImGui::Separator();

		// ===== CloseRangeEnemy の調整 ===== //
		BaseEnemy* closeRep = GetRepresentative(EnemyType::CloseRange);
		if (closeRep) {
			ImGui::Text("CloseRange Enemy Settings:");
			closeRep->Information(); // 代表1体のImGuiを表示

			if (ImGui::Button("Apply to All CloseRange")) {
				ApplySettingsToType(EnemyType::CloseRange, closeRep);
			}
			ImGui::SameLine();
			if (ImGui::Button("Apply CloseRange to ALL Enemies")) {
				ApplySettingsToAll(closeRep);
			}
		} else {
			ImGui::TextDisabled("No CloseRange enemies exist.");
		}

		ImGui::Separator();

		// ===== LongRangeEnemy の調整 ===== //
		BaseEnemy* longRep = GetRepresentative(EnemyType::LongRange);
		if (longRep) {
			ImGui::Text("LongRange Enemy Settings:");
			longRep->Information(); // 代表1体のImGuiを表示

			if (ImGui::Button("Apply to All LongRange")) {
				ApplySettingsToType(EnemyType::LongRange, longRep);
			}
			ImGui::SameLine();
			if (ImGui::Button("Apply LongRange to ALL Enemies")) {
				ApplySettingsToAll(longRep);
			}
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

//-------------------------------------------/// 
/// 指定タイプの全敵に設定をコピー
///-------------------------------------------///
void EnemyManager::ApplySettingsToType(EnemyType type, const BaseEnemy* sourceEnemy) {
	if (!sourceEnemy) return;

	for (auto& e : enemies_) {
		if (!e) continue;

		// 同じタイプかチェック
		if (GetEnemyType(e.get()) == type) {
			// ソースと同じオブジェクトは除外
			if (e.get() != sourceEnemy) {
				sourceEnemy->CopyTuningTo(e.get());
			}
		}
	}
}

///-------------------------------------------/// 
/// 全敵に設定をコピー
///-------------------------------------------///
void EnemyManager::ApplySettingsToAll(const BaseEnemy* sourceEnemy) {
	if (!sourceEnemy) return;

	for (auto& e : enemies_) {
		if (!e) continue;

		// ソースと同じオブジェクトは除外
		if (e.get() != sourceEnemy) {
			sourceEnemy->CopyTuningTo(e.get());
		}
	}
}

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
/// 代表敵の設定を同タイプ全体に適用
///-------------------------------------------///
void EnemyManager::ApplyRepresentativeSettingsToType(EnemyType type) {
	BaseEnemy* representative = GetRepresentative(type);
	if (representative) {
		ApplySettingsToType(type, representative);
	}
}

///-------------------------------------------/// 
/// 代表敵の設定を全敵に適用
///-------------------------------------------///
void EnemyManager::ApplyRepresentativeSettingsToAll(EnemyType sourceType) {
	BaseEnemy* representative = GetRepresentative(sourceType);
	if (representative) {
		ApplySettingsToAll(representative);
	}
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