#include "ParticleService.h"
// Manager
#include "Engine/System/Managers/ParticleManager.h"
// ServiceLocator
#include "ServiceLocator.h"

// 追加
void ParticleService::AddParticle(const std::string& name, std::unique_ptr<ParticleGroup> particle) {
	ServiceLocator::GetParticleManager()->AddParticle(name, std::move(particle));
}
// 削除
void ParticleService::RemoveParticle(const std::string& name) {
	ServiceLocator::GetParticleManager()->RemoveParticle(name);
}
// エミっと
void ParticleService::Emit(const std::string& name, const Vector3& translate) {
	ServiceLocator::GetParticleManager()->Emit(name, translate);
}
// テクスチャの設定
void ParticleService::SetTexture(const std::string& name, const std::string& textureName) {
	ServiceLocator::GetParticleManager()->SetTexture(name, textureName);
}