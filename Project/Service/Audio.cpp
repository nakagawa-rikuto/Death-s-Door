#include "Audio.h"
#include "Engine/System/Managers/AudioManager.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// 音声の再生
	///-------------------------------------------///
	void Audio::StartSound(const std::string& key, bool loop) {
		Locator::GetAudioManager()->Play(key, loop);
	}
	
	///-------------------------------------------/// 
	/// 音声の停止
	///-------------------------------------------///
	void Audio::StopSound(const std::string& key) {
		Locator::GetAudioManager()->Stop(key);
	}
	///-------------------------------------------/// 
	/// 音声のボリュームの調整
	///-------------------------------------------///
	void Audio::VolumeSound(const std::string& key, float volume) {
		Locator::GetAudioManager()->SetVolume(key, volume);
	}
	///-------------------------------------------/// 
	/// 全ての音声のボリュームの調整
	///-------------------------------------------///
	void Audio::AllVolumeSound(float volume) {
		Locator::GetAudioManager()->SetVolumeAll(volume);
	}
	///-------------------------------------------/// 
	/// 音声の再生速度の調整
	///-------------------------------------------///
	void Audio::PitchSound(const std::string& key, float pitch) {
		Locator::GetAudioManager()->SetPitch(key, pitch);
	}
}
