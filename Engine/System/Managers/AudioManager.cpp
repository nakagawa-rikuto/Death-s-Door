#include "AudioManager.h"

#include <cassert>
#include <fstream>
#include <vector>

// チャンクヘッダ
struct ChunkHeader {
	char id[4];    // チャック毎のID
	int32_t size;  // チャンクサイズ 
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk; // "RIFF"
	char type[4];      // "WAVE"
};

// FMIチャンク
struct FormatChunk {
	ChunkHeader chunk;  // "fmt"
	WAVEFORMATEX fmt;   // 波形フォーマット
};


///-------------------------------------------/// 
///
///-------------------------------------------///
AudioManager::~AudioManager() {
	sourceVoices_.clear();
	masterVoice_ = nullptr;
	xAudio2_.Reset();
}

///-------------------------------------------/// 
///
///-------------------------------------------///
void AudioManager::Initialze() {
	HRESULT result;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));
}

///-------------------------------------------/// 
/// 音声データの読み込み
///-------------------------------------------///
void AudioManager::Load(const std::string& key, const std::string& filename, bool isMP3) {
	// 既に存在する場合はスキップ
	if (soundDatas_.find(key) != soundDatas_.end()) {
		return; // 何もしない
	}
	if (isMP3) {
		soundDatas_[key] = LoadMP3(filename);
	} else {
		soundDatas_[key] = LoadWave(filename);
	}
}

///-------------------------------------------/// 
/// 音声データの解放
///-------------------------------------------///
void AudioManager::Unload(const std::string& key) {
	auto it = soundDatas_.find(key);
	if (it != soundDatas_.end()) {
		UnloadSoundData(it->second);
		soundDatas_.erase(it);
	}
}

///-------------------------------------------/// 
/// 音声データの一括開放
///-------------------------------------------///
void AudioManager::UnloadAll() {
	for (auto& pair : soundDatas_) {
		UnloadSoundData(pair.second);
	}
	soundDatas_.clear(); // コンテナをクリア
}

///-------------------------------------------/// 
/// サウンドの再生
///-------------------------------------------///
void AudioManager::Play(const std::string& key, bool loop) {
	auto it = soundDatas_.find(key); // 探したkeyを代入
	
	assert(it != soundDatas_.end());
	HRESULT result;

	// 再生中の音声がある場合は処理をスキップ
	if (sourceVoices_[key]) {
		XAUDIO2_VOICE_STATE state;
		sourceVoices_[key]->GetState(&state);
		if (state.BuffersQueued > 0) {
			return; // 既存の音声が再生中なので新しい音声を再生しない
		}
		// 既存のSourceVoiceを停止・破棄
		sourceVoices_[key]->Stop(0);                // 再生を停止
		sourceVoices_[key]->FlushSourceBuffers();  // バッファをクリア
		sourceVoices_[key]->DestroyVoice();        // ボイスを解放
		sourceVoices_[key] = nullptr;              // ポインタを無効化
	}

	// 波形フォーマットを基にSourceVoiceの生成
	result = xAudio2_->CreateSourceVoice(&sourceVoices_[key], &it->second.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = it->second.pBuffer;
	buf.AudioBytes = it->second.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// ループ再生の設定
	if (loop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE; // 無限ループ
	} else {
		buf.LoopCount = 0; // ループなし
	}

	// 波形データの再生
	result = sourceVoices_[key]->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));
	result = sourceVoices_[key]->Start();
	assert(SUCCEEDED(result));
}

///-------------------------------------------/// 
/// サウンドの停止
///-------------------------------------------///
void AudioManager::Stop(const std::string& key) {

	if (sourceVoices_[key]) {
		sourceVoices_[key]->Stop();
		sourceVoices_[key]->FlushSourceBuffers();
		sourceVoices_[key]->DestroyVoice();        // ボイスを解放
		sourceVoices_[key] = nullptr;              // ポインタを無効化
	}
}

///-------------------------------------------/// 
/// 全てのサウンドを停止
///-------------------------------------------///
void AudioManager::StopAll() {
	for (auto& pair : sourceVoices_) {
		if (pair.second) {
			pair.second->Stop();
			pair.second->FlushSourceBuffers();
			pair.second->DestroyVoice();
			pair.second = nullptr;
		}
	}
	sourceVoices_.clear();
}

///-------------------------------------------/// 
/// 音量の設定
///-------------------------------------------///
void AudioManager::SetVolume(const std::string& key, float volume) {
	if (sourceVoices_[key]) {
		sourceVoices_[key]->SetVolume(volume);
	}
}

///-------------------------------------------/// 
/// 再生速度の設定
///-------------------------------------------///
void AudioManager::setPitch(const std::string& key, float pitch) {
	if (sourceVoices_[key]) {
		sourceVoices_[key]->SetFrequencyRatio(pitch);
	}
}


///-------------------------------------------/// 
/// 音声データの読み込み(Wave)
///-------------------------------------------///
SoundData AudioManager::LoadWave(const std::string& filename) {
	/// ===ファイルオープン=== ///
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	/// ===.wavデータ読み込み=== ///
	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプAがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Dataチャンクの読み込み
	ChunkHeader data;
	while (true) {
		file.read((char*)&data, sizeof(data));
		// "data" チャンクならループを抜ける
		if (strncmp(data.id, "data", 4) == 0) {
			break;
		}
		// 読み取り位置をチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
	}
	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	// Waveファイルを閉じる
	file.close();

	/// ====読み込んだ音声データをreturn== ///
	// retrunする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

///-------------------------------------------/// 
/// 音声データの読み込み(MP3)
///-------------------------------------------///
SoundData AudioManager::LoadMP3(const std::string& filename) {

	//// 通ってない
	//// === キャッシュの確認と返却 ===
	//if (auto it = soundDatas_.find(filename); it != soundDatas_.end()) {
	//	return it->second; // 既にロード済みならキャッシュを返す
	//}

	/// ===ファイルオープン=== ///
   // ファイル名を wide 文字列に変換
	int wideSize = MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, nullptr, 0);
	std::wstring wideFilename(wideSize, 0);
	MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, &wideFilename[0], wideSize);

	// Media Foundation の初期化
	HRESULT result = MFStartup(MF_VERSION);
	assert(SUCCEEDED(result));

	// SourceReader を作成
	IMFSourceReader* sourceReader = nullptr;
	result = MFCreateSourceReaderFromURL(wideFilename.c_str(), nullptr, &sourceReader);
	assert(SUCCEEDED(result));

	/// ===音声データ読み込み=== ///
	// 出力形式を PCM 形式に設定
	IMFMediaType* audioType = nullptr;
	result = MFCreateMediaType(&audioType);
	assert(SUCCEEDED(result));
	result = audioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	assert(SUCCEEDED(result));
	result = audioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	assert(SUCCEEDED(result));
	result = sourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, audioType);
	assert(SUCCEEDED(result));

	// PCM 形式のフォーマットを取得
	IMFMediaType* outputType = nullptr;
	result = sourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &outputType);
	assert(SUCCEEDED(result));

	WAVEFORMATEX* pWaveFormat = nullptr;
	UINT32 cbFormat = 0;
	result = MFCreateWaveFormatExFromMFMediaType(outputType, &pWaveFormat, &cbFormat);
	assert(SUCCEEDED(result));

	// フォーマットを保存
	FormatChunk format = {};
	memcpy(&format.fmt, pWaveFormat, sizeof(WAVEFORMATEX));
	CoTaskMemFree(pWaveFormat);
	outputType->Release();

	/// ===音声データをバッファに読み込み=== ///
	ChunkHeader data = {};
	std::vector<BYTE> buffer;

	while (true) {
		IMFSample* sample = nullptr;
		DWORD flags = 0;
		result = sourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample);
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break; // データの終端に到達
		}
		assert(SUCCEEDED(result));

		if (sample) {
			IMFMediaBuffer* mediaBuffer = nullptr;
			result = sample->ConvertToContiguousBuffer(&mediaBuffer);
			assert(SUCCEEDED(result));

			BYTE* audioData = nullptr;
			DWORD audioDataLength = 0;
			result = mediaBuffer->Lock(&audioData, nullptr, &audioDataLength);
			assert(SUCCEEDED(result));

			// データをバッファに追加
			buffer.insert(buffer.end(), audioData, audioData + audioDataLength);

			result = mediaBuffer->Unlock();
			assert(SUCCEEDED(result));
			mediaBuffer->Release();
			sample->Release();
		}
	}

	// Data チャンクのデータ部をセット
	data.size = static_cast<uint32_t>(buffer.size());
	char* pBuffer = new char[data.size];
	memcpy(pBuffer, buffer.data(), data.size);

	// リソース解放
	sourceReader->Release();
	audioType->Release();
	MFShutdown();

	/// ====読み込んだ音声データを return ==== ///
	SoundData soundData = {};
	soundData.wfex = format.fmt; // フォーマットを設定
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

///-------------------------------------------/// 
/// 音声データの解放
///-------------------------------------------///
void AudioManager::UnloadSoundData(SoundData& soundData) {
	// バッファのメモリを解放
	delete[] soundData.pBuffer;
	soundData.pBuffer = 0;
	soundData.bufferSize = 0;
	soundData.wfex = {};
}
