#pragma once
/// ===Include=== ///
// C++
#include <string>

///=====================================================/// 
/// Load関数
///=====================================================///
class Loader {
public:
	// テクスチャの読み込み
	static void LoadTexture(const std::string& key, const std::string& filePath);
	// モデルの読み込み
	static void LoadModel(const std::string& Key, const std::string& filename);
	// Animationの読み込み
	static void LoadAnimation(const std::string& Key, const std::string& filename);
	static void LoadAnimationdifferentModel(const std::string& directorPath, const std::string& ModelFilename, const std::string& AnimationFilename);
	// CSVの読み込み
	static void LoadCSV(const std::string& filename);
	// Levelの読み込み
	static void LoadLevelJson(const std::string& filename);
	static void LoadLevelModel(const std::string& filname);
	// 音声データの読み込み
	static void LoadWave(const std::string& key, const std::string& filename);
	static void LoadMP3(const std::string& key, const std::string& filename);
	// 音声データの解放
	static void UnloadSound(const std::string& key);
	// 音声データの一括開放
	static void AllUnloadSound();
};

