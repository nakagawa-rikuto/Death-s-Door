#pragma once
/// ===Include=== ///
#include <string>

///=====================================================/// 
/// AudioService
///=====================================================///
class AudioService {
public:
	// サウンドの再生
	static void PlayeSound(const std::string& key, bool loop);
	// サウンドの停止
	static void StopSound(const std::string& key);
	// 音量の設定
	static void VolumeSound(const std::string& key, float volume);
	// 再生速度の設定
	static void PitchSound(const std::string& key, float pitch);
};

