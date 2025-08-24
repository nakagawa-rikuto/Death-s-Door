#include "AudioService.h"
#include "Engine/System/Managers/AudioManager.h"
// ServiceLocator
#include "ServiceLocator.h"

///-------------------------------------------/// 
/// Audioの処理
///-------------------------------------------///
// 音声の再生
void AudioService::PlayeSound(const std::string& key, bool loop) {
	ServiceLocator::GetAudioManager()->Play(key, loop);
}
// 音声の停止
void AudioService::StopSound(const std::string& key) {
	ServiceLocator::GetAudioManager()->Stop(key);
}
// 音声のボリュームの調整
void AudioService::VolumeSound(const std::string& key, float volume) {
	ServiceLocator::GetAudioManager()->SetVolume(key, volume);
}
// 音声の再生速度の調整
void AudioService::PitchSound(const std::string& key, float pitch) {
	ServiceLocator::GetAudioManager()->setPitch(key, pitch);
}
