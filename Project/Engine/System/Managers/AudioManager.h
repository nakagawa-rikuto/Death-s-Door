#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
// C++
#include <memory>
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

namespace MiiEngine {
	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		std::unique_ptr<BYTE[]> pBuffer;
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

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();

	public:/// ===関数=== ///

		/// <summary>
		/// 音声データの読み込み処理
		/// </summary>
		/// <param name="key">読み込むリソースを識別するためのキー。</param>
		/// <param name="filename">読み込むファイルのパスまたは名前。</param>
		/// <param name="isMP3">ファイルがMP3形式であるかを示すフラグ。trueの場合はMP3として処理されます。</param>
		void Load(const std::string& key, const std::string& filename, bool isMP3);

		/// <summary>
		/// 音声データの解放処理
		/// </summary>
		/// <param name="key">アンロードする対象を識別する文字列キー。</param>
		void Unload(const std::string& key);

		/// <summary>
		/// 音声データの一括解放処理
		/// </summary>
		void UnloadAll();

		/// <summary>
		/// サウンドの再生処理
		/// </summary>
		/// <param name="key">再生対象を識別する文字列キー。</param>
		/// <param name="loop">再生をループするかどうかを示すフラグ。true の場合は繰り返し再生し、false（既定値）は一度だけ再生します。</param>
		void Play(const std::string& key, bool loop = false);
	
		/// <summary>
		/// サウンドの停止処理
		/// </summary>
		/// <param name="key">停止対象を識別する文字列キー。</param>
		void Stop(const std::string& key);

		/// <summary>
		/// 全てのサウンドを停止処理
		/// </summary>
		void StopAll();

		/// <summary>
		/// サウンドの音量設定処理
		/// </summary>
		/// <param name="key">音量を設定する対象を識別するキー文字列。</param>
		/// <param name="volume">設定する音量値。通常は 0.0（無音）から 1.0（最大）などの範囲で指定します。</param>
		void SetVolume(const std::string& key, float volume);

		/// <summary>
		/// サウンド全体の音量設定処理
		/// </summary>
		/// <param name="volume"></param>
		void SetVolumeAll(float volume);

		/// <summary>
		/// 再生速度の設定処理
		/// </summary>
		/// <param name="key">ピッチを設定する対象を識別する文字列キー。</param>
		/// <param name="pitch">設定するピッチ値（float）。</param>
		void SetPitch(const std::string& key, float pitch);

	private: /// ===変数=== ///
		ComPtr<IXAudio2> xAudio2_;
		IXAudio2MasteringVoice* masterVoice_ = nullptr;
		std::unordered_map<std::string, IXAudio2SourceVoice*> sourceVoices_;

		// 音声データ
		std::unordered_map<std::string, SoundData> soundDates_;

	private: /// ===関数=== ///

		/// <summary>
		/// WAVファイルの読み込み処理
		/// </summary>
		/// <param name="filename">読み込む WAV ファイルのパスまたはファイル名。</param>
		/// <returns>読み込まれた音声データを格納した SoundData。サンプル、チャンネル数、サンプリングレートなどを含みます。</returns>
		SoundData LoadWave(const std::string& filename);

		/// <summary>
		/// MP3ファイルの読み込み処理
		/// </summary>
		/// <param name="filename読み込むMP3ファイルのパスまたはファイル名。e"></param>
		/// <returns>読み込まれた音声データを表すSoundData。読み込みやデコードに失敗した場合の挙動は実装依存です。ns></returns>
		SoundData LoadMP3(const std::string& filename);

		/// <summary>
		/// サウンドのアンロード処理
		/// </summary>
		/// <param name="data">アンロードするサウンドデータへの参照。関数はこの参照に関連するリソースを解放します。</param>
		void UnloadSoundData(SoundData& data);
	};
}