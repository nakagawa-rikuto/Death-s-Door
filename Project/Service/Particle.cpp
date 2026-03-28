#include "Particle.h"
// Manager
#include "Engine/System/Managers/ParticleManager.h"
// Locator
#include "Locator.h"
// Definition
#include "Engine/Graphics/Particle/ParticleDefinition.h"
#include "Engine/Graphics/Particle/ParticleParameter.h"

namespace Service {
	///-------------------------------------------/// 
	/// 定義の管理
	///-------------------------------------------///

	// JSONから定義を読み込み
	void Particle::LoadParticleDefinition(const std::string& jsonPath) {
		// ベースのディレクトリパス
		const std::string& baseDirectoryPath = "./Resource/Json/Particles";
		return Locator::GetParticleManager()->LoadParticleDefinition(baseDirectoryPath + "/" + jsonPath);
	}

	// 定義を直接追加
	void Particle::AddParticleDefinition(const std::string& name, const MiiEngine::ParticleDefinition& definition) {
		Locator::GetParticleManager()->AddParticleDefinition(name, definition);
	}

	// 定義を削除
	void Particle::RemoveParticleDefinition(const std::string& name) {
		Locator::GetParticleManager()->RemoveParticleDefinition(name);
	}

	///-------------------------------------------/// 
	/// パーティクルの操作
	///-------------------------------------------///
	// パーティクルを発生
	MiiEngine::ParticleGroup* Particle::Emit(const std::string& name, const Vector3& translate) {
		return Locator::GetParticleManager()->Emit(name, translate);
	}

	// テクスチャを設定
	void Particle::SetTexture(const std::string& name, const std::string& textureName) {
		Locator::GetParticleManager()->SetTexture(name, textureName);
	}

	// パラメータを設定（型安全版）
	void Particle::SetParameter(const std::string& name, MiiEngine::ParticleParameter param, float value) {
		Locator::GetParticleManager()->SetParameter(name, param, value);
	}

	// パーティクルを停止
	void Particle::StopParticle(const std::string& name) {
		Locator::GetParticleManager()->StopParticle(name);
	}

	// 全パーティクルを削除
	void Particle::RemoveAllParticles() {
		Locator::GetParticleManager()->RemoveAllParticles();
	}

	///-------------------------------------------/// 
	/// 情報取得
	///-------------------------------------------///
	// 定義を取得
	const MiiEngine::ParticleDefinition* Particle::GetDefinition(const std::string& name) {
		return Locator::GetParticleManager()->GetDefinition(name);
	}

	// 定義が存在するか確認
	bool Particle::HasDefinition(const std::string& name) {
		return Locator::GetParticleManager()->HasDefinition(name);
	}

	// アクティブなパーティクル数を取得
	uint32_t Particle::GetActiveParticleCount(const std::string& name) {
		return Locator::GetParticleManager()->GetActiveParticleCount(name);
	}

	// 定義名の一覧を取得
	std::vector<std::string> Particle::GetDefinitionNames() {
		return Locator::GetParticleManager()->GetDefinitionNames();
	}

	// アクティブなグループ数を取得
	size_t Particle::GetActiveGroupCount(const std::string& name) {
		return Locator::GetParticleManager()->GetActiveGroupCount(name);
	}
}