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
		// ===== 集計とサンプル取得 ===== //
		int countClose = 0;
		int countLong = 0;
		CloseRangeEnemy* sampleClose = nullptr; // 表示用サンプル1体
		LongRangeEnemy* sampleLong = nullptr; // 表示用サンプル1体

		for (auto& e : enemies_) {
			if (!e) continue;
			if (auto* c = dynamic_cast<CloseRangeEnemy*>(e.get())) {
				++countClose;
				if (!sampleClose) sampleClose = c;
			} else if (auto* l = dynamic_cast<LongRangeEnemy*>(e.get())) {
				++countLong;
				if (!sampleLong) sampleLong = l;
			}
		}

		// ===== 種類別カウント＋合計 ===== //
		ImGui::Text("CloseRange: %d", countClose);
		ImGui::Text("LongRange : %d", countLong);
		ImGui::Text("Total     : %d", countClose + countLong);

		ImGui::Separator();
		ImGui::TextDisabled("Per-type config (one window each):");

		// ===== CloseRangeEnemy の UI を1つだけ ===== //
		if (sampleClose) {
			sampleClose->UpdateImGui();   // CloseRange 用ウィンドウが1つだけ開く
		} else {
			ImGui::TextDisabled("No CloseRangeEnemy exists.");
		}

		// ===== LongRangeEnemy の UI を1つだけ ===== //
		if (sampleLong) {
			sampleLong->UpdateImGui();    // LongRange 用ウィンドウが1つだけ開く
		} else {
			ImGui::TextDisabled("No LongRangeEnemy exists.");
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