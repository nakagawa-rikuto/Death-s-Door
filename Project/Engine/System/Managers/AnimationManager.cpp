#include "AnimationManager.h"
// c++
#include <fstream>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
AnimationManager::~AnimationManager() {
	animationDatas_.clear();
}

///-------------------------------------------/// 
/// ファイル読み込み
///-------------------------------------------///
void AnimationManager::Load(const std::string& baseDirectoryPath, const std::string& Key, const std::string& filename) {
	// 読み込み済みアニメーションを検索
	if (animationDatas_.contains(filename)) {
		// 読み込み済みなら早期return
		return;
	}

	// Dataの宣言
	std::map<std::string, Animation> animationData;
	// アニメーションの読み込み
	animationData = LoadAnimation(baseDirectoryPath + "/" + Key, filename);

	// アニメーションをMapコンテナに格納
	animationDatas_[Key] = animationData;
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
std::map<std::string, Animation> AnimationManager::GetAnimation(const std::string& directorPath) {
	assert(animationDatas_.contains(directorPath));
	return animationDatas_.at(directorPath);
}

///-------------------------------------------/// 
/// アニメーションファイル読み込み
///-------------------------------------------///
std::map<std::string, Animation> AnimationManager::LoadAnimation(const std::string& directorPath, const std::string& filename) {
	/// ===LoadAnimationを実装する=== ///
	std::map<std::string, Animation> animations;
	Assimp::Importer importer;
	std::string filePath = directorPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0); // アニメーションがない
	
	// アニメーションの数だけ回す
	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		aiAnimation* animationAssimp = scene->mAnimations[animationIndex];
		Animation animation; // 今回作るアニメーション
		animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換
		
		/// ===NodeAnimationを解析する=== ///
		// assimpでは個々のNodeのAnimationをchnnelと読んでいるのでchannelを回してNodeAnimationの情報を取ってくる
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
			// Translate（キーフレーム）
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
				keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}
			// Rotate (キーフレーム) 
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // y, z 反転
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}
			// Scale (キーフレーム) 
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // スケールは変換不要
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

		// アニメーションの名前を登録
		std::string animName = animationAssimp->mName.C_Str();

		// アニメーションをMapコンテナに格納
		animations[animName] = animation;
		
	}
	// 解析完了
	return animations;
}
