#include "ParticleManager.h"
// Service
#include "Service/DeltaTime.h"
// c++
#include <fstream>
#include <cassert>

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	ParticleManager::~ParticleManager() {
		definitions_.clear();
		activeParticles_.clear();
		timelines_.clear();
		activeTimelines_.clear();
	}

	///-------------------------------------------/// 
	/// パーティクル定義の追加（JSONから）
	///-------------------------------------------///
	void ParticleManager::LoadParticleDefinition(const std::string& jsonPath) {
		try {
			// ファイルを開く
			std::ifstream file(jsonPath);
			// ファイルが開けなかった場合のエラーチェック
			if (!file.is_open()) {
				assert(false && "Failed to open particle definition file");
				return;
			}

			// JSONをパース
			nlohmann::json j;
			file >> j;
			file.close();

			// JSONからParticleDefinitionを生成
			ParticleDefinition def = ParticleDefinition::FromJson(j);
			// すでに定義が存在していたらreturn
			if (definitions_.find(def.name) != definitions_.end()) {
				return;
			}
			// 定義を追加
			definitions_[def.name] = def;

		} catch (const std::exception& e) {
			// エラーハンドリング（ログ出力など）
			assert(false && "Failed to parse particle definition JSON");
			e;
		}
	}

	///-------------------------------------------/// 
	/// パーティクル定義の追加（直接指定）
	///-------------------------------------------///
	void ParticleManager::AddParticleDefinition(const std::string& name, const ParticleDefinition& definition) {
		definitions_[name] = definition;
	}

	///-------------------------------------------/// 
	/// 発生
	///-------------------------------------------///
	ParticleGroup* ParticleManager::Emit(const std::string& name, const Vector3& translate) {
		// 定義が存在するか確認
		auto it = definitions_.find(name);
		if (it == definitions_.end()) {
			return nullptr; // 定義が見つからない
		}

		// 新しいParticleGroupを作成
		auto particleGroup = std::make_unique<ParticleGroup>();
		particleGroup->Initialize(translate, it->second);

		// ポインタを保存してから返す
		ParticleGroup* ptr = particleGroup.get();
		activeParticles_.push_back(std::move(particleGroup));

		return ptr;
	}

	///-------------------------------------------/// 
	/// 全てのParticleの更新
	///-------------------------------------------///
	void ParticleManager::Update() {
		float deltaTime = Service::DeltaTime::GetDeltaTime();

		// アクティブなタイムラインを更新
		for (auto& active : activeTimelines_) {
			active.currentTime += deltaTime;

			// 各エントリのタイミングチェック
			for (size_t i = 0; i < active.timeline.entries.size(); ++i) {
				if (!active.emitted[i]) {
					const auto& entry = active.timeline.entries[i];
					if (active.currentTime >= entry.startTime) {
						// パーティクルを発生させる
						Vector3 emitPos = {
							active.basePosition.x + entry.offset.x,
							active.basePosition.y + entry.offset.y,
							active.basePosition.z + entry.offset.z
						};
						Emit(entry.particleName, emitPos);
						active.emitted[i] = true;
					}
				}
			}
		}

		// 終了したタイムラインを削除
		activeTimelines_.erase(
			std::remove_if(activeTimelines_.begin(), activeTimelines_.end(),
				[](const ActiveTimeline& at) {
					return at.currentTime >= at.timeline.totalDuration;
				}),
			activeTimelines_.end()
		);

		// 完了したパーティクルを削除
		activeParticles_.erase(
			std::remove_if(activeParticles_.begin(), activeParticles_.end(),
				[](const std::unique_ptr<ParticleGroup>& particle) {
					return particle->IsFinish();
				}),
			activeParticles_.end()
		);

		// アクティブなパーティクルを更新
		for (auto& particle : activeParticles_) {
			particle->Update();
		}
	}

	///-------------------------------------------/// 
	/// 全てのParticleの描画
	///-------------------------------------------///
	void ParticleManager::Draw(BlendMode mode) {
		for (auto& particle : activeParticles_) {
			particle->Draw(mode);
		}
	}

	///-------------------------------------------/// 
	/// 停止処理
	///-------------------------------------------///
	void ParticleManager::StopParticle(const std::string& name) {
		// アクティブなパーティクルから該当する名前のものを全て停止
		for (auto& particle : activeParticles_) {
			if (particle->GetDefinition().name == name) {
				particle->Stop();
			}
		}
	}

	///-------------------------------------------/// 
	/// パーティクル削除
	///-------------------------------------------///
	void ParticleManager::RemoveParticleDefinition(const std::string& name) {
		// 定義から削除
		definitions_.erase(name);

		// アクティブなパーティクルからも該当するものを削除
		activeParticles_.erase(
			std::remove_if(activeParticles_.begin(), activeParticles_.end(),
				[&name](const std::unique_ptr<ParticleGroup>& particle) {
					return particle->GetDefinition().name == name;
				}),
			activeParticles_.end()
		);
	}

	///-------------------------------------------/// 
	/// 全てのパーティクルを削除
	///-------------------------------------------///
	void ParticleManager::RemoveAllParticles() {
		activeParticles_.clear();
	}

	///-------------------------------------------/// 
	/// タイムライン定義の読み込み（JSONファイルから）
	///-------------------------------------------///
	void ParticleManager::LoadParticleTimeline(const std::string& jsonPath) {
		try {
			std::ifstream file(jsonPath);
			if (!file.is_open()) {
				assert(false && "Failed to open particle timeline file");
				return;
			}

			nlohmann::json j;
			file >> j;
			file.close();

			ParticleTimeline timeline = ParticleTimelineSerializer::FromJson(j);
			timelines_[timeline.name] = timeline;

		} catch (const std::exception& e) {
			assert(false && "Failed to parse particle timeline JSON");
			e;
		}
	}

	///-------------------------------------------/// 
	/// タイムライン定義の追加（直接指定）
	///-------------------------------------------///
	void ParticleManager::AddParticleTimeline(const std::string& name, const ParticleTimeline& timeline) {
		timelines_[name] = timeline;
	}

	///-------------------------------------------/// 
	/// タイムライン再生（指定位置を起点に時間経過で各パーティクルを発生させる）
	///-------------------------------------------///
	void ParticleManager::EmitTimeline(const std::string& timelineName, const Vector3& basePosition) {
		auto it = timelines_.find(timelineName);
		if (it == timelines_.end()) {
			return; // タイムラインが見つからない
		}

		ActiveTimeline active;
		active.timeline = it->second;
		active.basePosition = basePosition;
		active.currentTime = 0.0f;
		active.emitted.assign(it->second.entries.size(), false);

		activeTimelines_.push_back(std::move(active));
	}

	///-------------------------------------------/// 
	/// タイムライン定義の存在確認
	///-------------------------------------------///
	bool ParticleManager::HasTimeline(const std::string& name) const {
		return timelines_.find(name) != timelines_.end();
	}

	///-------------------------------------------/// 
	/// テクスチャ設定
	///-------------------------------------------///
	void ParticleManager::SetTexture(const std::string& name, const std::string& textureName) {
		// 定義を更新
		auto defIt = definitions_.find(name);
		if (defIt != definitions_.end()) {
			defIt->second.appearance.texturePath = textureName;
		}

		// アクティブなパーティクルにも適用
		for (auto& particle : activeParticles_) {
			if (particle->GetDefinition().name == name) {
				particle->SetTexture(textureName);
			}
		}
	}

	///-------------------------------------------/// 
	/// パラメータ設定（型安全版）
	///-------------------------------------------///
	void ParticleManager::SetParameter(const std::string& name, ParticleParameter param, float value) {
		// 定義を更新
		auto defIt = definitions_.find(name);
		if (defIt != definitions_.end()) {
			// 定義内のパラメータを更新（次回発生時に反映）
			switch (param) {
				// 物理パラメータ
			case ParticleParameter::Gravity:
				defIt->second.physics.gravity = value;
				break;
			case ParticleParameter::ExplosionRadiusX:
				defIt->second.physics.explosionRange.x = value;
				break;
			case ParticleParameter::ExplosionRadiusY:
				defIt->second.physics.explosionRange.y = value;
				break;
			case ParticleParameter::ExplosionRadiusZ:
				defIt->second.physics.explosionRange.z = value;
				break;
			case ParticleParameter::UpwardForce:
				defIt->second.physics.upwardForce = value;
				break;
			case ParticleParameter::AccelerationX:
				defIt->second.physics.acceleration.x = value;
				break;
			case ParticleParameter::AccelerationY:
				defIt->second.physics.acceleration.y = value;
				break;
			case ParticleParameter::AccelerationZ:
				defIt->second.physics.acceleration.z = value;
				break;

				// 発生パラメータ
			case ParticleParameter::EmissionRate:
				defIt->second.emission.emissionRate = value;
				break;
			case ParticleParameter::Frequency:
				defIt->second.emission.frequency = value;
				break;
			case ParticleParameter::LifetimeMin:
				defIt->second.emission.lifetimeMin = value;
				break;
			case ParticleParameter::LifetimeMax:
				defIt->second.emission.lifetimeMax = value;
				break;
			case ParticleParameter::BurstCount:
				defIt->second.emission.burstCount = static_cast<uint32_t>(value);
				break;

				// 見た目パラメータ
			case ParticleParameter::StartColorR:
				defIt->second.appearance.startColor.x = value;
				break;
			case ParticleParameter::StartColorG:
				defIt->second.appearance.startColor.y = value;
				break;
			case ParticleParameter::StartColorB:
				defIt->second.appearance.startColor.z = value;
				break;
			case ParticleParameter::StartColorA:
				defIt->second.appearance.startColor.w = value;
				break;
			case ParticleParameter::EndColorR:
				defIt->second.appearance.endColor.x = value;
				break;
			case ParticleParameter::EndColorG:
				defIt->second.appearance.endColor.y = value;
				break;
			case ParticleParameter::EndColorB:
				defIt->second.appearance.endColor.z = value;
				break;
			case ParticleParameter::EndColorA:
				defIt->second.appearance.endColor.w = value;
				break;

				// 回転パラメータ
			case ParticleParameter::RotationSpeedX:
				defIt->second.rotation.rotationSpeedMin.x = value;
				break;
			case ParticleParameter::RotationSpeedY:
				defIt->second.rotation.rotationSpeedMin.y = value;
				break;
			case ParticleParameter::RotationSpeedZ:
				defIt->second.rotation.rotationSpeedMin.z = value;
				break;

				// 速度パラメータ
			case ParticleParameter::VelocityMinX:
				defIt->second.physics.velocityMin.x = value;
				break;
			case ParticleParameter::VelocityMinY:
				defIt->second.physics.velocityMin.y = value;
				break;
			case ParticleParameter::VelocityMinZ:
				defIt->second.physics.velocityMin.z = value;
				break;
			case ParticleParameter::VelocityMaxX:
				defIt->second.physics.velocityMax.x = value;
				break;
			case ParticleParameter::VelocityMaxY:
				defIt->second.physics.velocityMax.y = value;
				break;
			case ParticleParameter::VelocityMaxZ:
				defIt->second.physics.velocityMax.z = value;
				break;
			}
		}

		// アクティブなパーティクルにも適用
		for (auto& particle : activeParticles_) {
			if (particle->GetDefinition().name == name) {
				particle->SetParameter(param, value);
			}
		}
	}

	///-------------------------------------------/// 
	/// 情報取得
	///-------------------------------------------///
	const ParticleDefinition* ParticleManager::GetDefinition(const std::string& name) const {
		auto it = definitions_.find(name);
		if (it != definitions_.end()) {
			return &it->second;
		}
		return nullptr;
	}

	///-------------------------------------------/// 
	/// パーティクル定義が登録されているか確認
	///-------------------------------------------///
	bool ParticleManager::HasDefinition(const std::string& name) const {
		return definitions_.find(name) != definitions_.end();
	}

	///-------------------------------------------/// 
	/// アクティブなパーティクル数を取得
	///-------------------------------------------///
	uint32_t ParticleManager::GetActiveParticleCount(const std::string& name) const {
		uint32_t totalCount = 0;

		// vectorを線形探索して該当する名前のパーティクルをカウント
		for (const auto& particle : activeParticles_) {
			if (particle && particle->GetDefinition().name == name) {
				totalCount += particle->GetActiveParticleCount();
			}
		}

		return totalCount;
	}

	///-------------------------------------------/// 
	/// 登録されているパーティクル定義の一覧を取得
	///-------------------------------------------///
	std::vector<std::string> ParticleManager::GetDefinitionNames() const {
		std::vector<std::string> names;
		names.reserve(definitions_.size());

		for (const auto& [name, def] : definitions_) {
			names.push_back(name);
		}

		return names;
	}

	///-------------------------------------------/// 
	/// アクティブなパーティクルグループ数を取得
	///-------------------------------------------///
	size_t ParticleManager::GetActiveGroupCount(const std::string& name) const {
		size_t count = 0;

		// vectorを線形探索して該当する名前のグループをカウント
		for (const auto& particle : activeParticles_) {
			if (particle && particle->GetDefinition().name == name) {
				++count;
			}
		}

		return count;
	}
}