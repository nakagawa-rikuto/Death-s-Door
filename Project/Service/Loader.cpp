#include "Loader.h"
// Manager
#include "Engine/System/Managers/TextureManager.h"
#include "Engine/System/Managers/ModelManager.h"
#include "Engine/System/Managers/AudioManager.h"
#include "Engine/System/Managers/CSVManager.h"
#include "Engine/System/Managers/AnimationManager.h"
#include "Engine/system/Managers/LevelManager.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// テクスチャ
	///-------------------------------------------///
	void Loader::LoadTexture(const std::string& key, const std::string& filePath) {
		// ベースのディレクトリパス
		const std::string& baseDirectorPath = "./Resource/Textures";
		Locator::GetTextureManager()->LoadTexture(key, baseDirectorPath + "/" + filePath);
	}

	///-------------------------------------------/// 
	/// モデル
	///-------------------------------------------///
	void Loader::LoadModel(const std::string& Key, const std::string& filename) {
		// ベースのディレクトリパス
		const std::string& baseDirectorPath = "./Resource/Models";
		Locator::GetModelManager()->Load(baseDirectorPath, Key, filename);
	}

	///-------------------------------------------/// 
	/// csv
	///-------------------------------------------///
	void Loader::LoadCSV(const std::string& filename) {
		const std::string& baseDirectorPath = "./Resource/CSV";
		Locator::GetCSVManager()->Load(baseDirectorPath + "/" + filename);
	}

	///-------------------------------------------/// 
	/// Json
	///-------------------------------------------///
	void Loader::LoadLevelJson(const std::string& filename) {
		// ベースのディレクトリパス
		const std::string& baseDirectorPath = "./Resource/Json";
		Locator::GetLevelManager()->LoadLevelJson(baseDirectorPath, filename);
	}

	///-------------------------------------------/// 
	/// Animation
	///-------------------------------------------///
	void Loader::LoadAnimation(const std::string& key, const std::string& filename) {
		// ベースのディレクトリパス
		const std::string& baseDirectorPath = "./Resource/Animations";
		Locator::GetModelManager()->Load(baseDirectorPath, key, filename);
		Locator::GetAnimationManager()->Load(baseDirectorPath, key, filename);
	}
	void Loader::LoadAnimationdifferentModel(const std::string& key, const std::string& ModelFilename, const std::string& AnimationFilename) {
		// ベースのディレクトリパス
		const std::string& baseDirectorPath = "./Resource/Animations";
		Locator::GetModelManager()->Load(baseDirectorPath, key, ModelFilename);
		Locator::GetAnimationManager()->Load(baseDirectorPath, key, AnimationFilename);
	}

	///-------------------------------------------/// 
	/// WAVE
	///-------------------------------------------///
	void Loader::LoadWave(const std::string& key, const std::string& filename) {
		const std::string& directorPath = "./Resource/BGM";
		Locator::GetAudioManager()->Load(key, directorPath + "/" + filename, false);
	}

	///-------------------------------------------/// 
	/// MP3
	///-------------------------------------------///
	void Loader::LoadMP3(const std::string& key, const std::string& filename) {
		const std::string& directorPath = "./Resource/BGM";
		Locator::GetAudioManager()->Load(key, directorPath + "/" + filename, true);
	}

	///-------------------------------------------/// 
	/// 音声データの解放
	///-------------------------------------------///
	void Loader::UnloadSound(const std::string& key) {
		Locator::GetAudioManager()->Unload(key);
	}

	///-------------------------------------------/// 
	/// 音声データの一括開放
	///-------------------------------------------///
	void Loader::AllUnloadSound() {
		Locator::GetAudioManager()->UnloadAll();
	}
}