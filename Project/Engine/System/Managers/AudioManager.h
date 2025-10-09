#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
// waveの読み込み
#include <xaudio2.h> 
// mp3の読み込み
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mfreadwrite.h>
// c++
#include <string>
#include <unordered_map>
// リンク
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

///=====================================================/// 
/// オーディオ
///=====================================================///
class AudioManager {
public:
	AudioManager() = default;
	~AudioManager();

	// 初期化
	void Initialze();

public:/// ===関数=== ///

	// 音声データの読み込み
	void Load(const std::string& key, const std::string& filename, bool isMP3);
	// 音声データの解放
	void Unload(const std::string& key);
	// 音声データの一括解放
	void UnloadAll();
	// サウンドの再生
	void Play(const std::string& key, bool loop = false);
	// サウンドの停止
	void Stop(const std::string& key);
	// 全てのサウンドを停止
	void StopAll();
	// 音量の設定
	void SetVolume(const std::string& key, float volume);
	// 再生速度の設定
	void setPitch(const std::string& key, float pitch);
	// 音の反響効果
	//void InitializeReverbEffect();

private: /// ===変数=== ///
	ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	std::unordered_map<std::string, IXAudio2SourceVoice*> sourceVoices_;

	// 音声データ
	std::unordered_map<std::string, SoundData> soundDatas_;

private: /// ===関数=== ///

	// 音声データの読み込み
	SoundData LoadWave(const std::string& filename);
	SoundData LoadMP3(const std::string& filename);
	void UnloadSoundData(SoundData& data);
};


