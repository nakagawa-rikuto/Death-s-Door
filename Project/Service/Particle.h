#pragma once
/// ===include=== ///
// ParticleGroup
#include "Engine/Graphics/Particle/ParticleGroup.h"
// Timeline
#include "Engine/DataInfo/ParticleData.h"
// c++
#include <string>
#include <vector>
// Math
#include "Math/Vector3.h"

/// ===前方宣言=== ///
namespace MiiEngine {
	struct ParticleDefinition;
	enum class ParticleParameter;
}

namespace Service {
	///=====================================================/// 
	/// Particle
	///=====================================================///
	class Particle {
	public:

		/// ===定義の管理=== ///

		/// <summary>
		/// パーティクル定義の追加（JSONファイルから）
		/// </summary>
		/// <param name="jsonPath">JSONファイルのパス（Resource/Json/Particles/以下のパス）</param>
		static void LoadParticleDefinition(const std::string& jsonPath);

		/// <summary>
		/// パーティクル定義の追加（直接指定）
		/// </summary>
		/// <param name="name">パーティクルの名前</param>
		/// <param name="definition">パーティクル定義</param>
		static void AddParticleDefinition(const std::string& name, const MiiEngine::ParticleDefinition& definition);

		/// <summary>
		/// パーティクルの発生処理
		/// </summary>
		/// <param name="name">発生させるパーティクルの名前</param>
		/// <param name="translate">発生させる場所</param>
		/// <returns>生成されたParticleGroupへのポインタ（nullptrの場合は失敗）</returns>
		static MiiEngine::ParticleGroup* Emit(const std::string& name, const Vector3& translate);

		/// <summary>
		/// 指定した名前のアクティブなパーティクルをすべて停止
		/// </summary>
		/// <param name="name">停止するパーティクルの名前</param>
		static void StopParticle(const std::string& name);

		/// <summary>
		/// パーティクル定義の削除
		/// </summary>
		/// <param name="name">削除するパーティクルの名前</param>
		static void RemoveParticleDefinition(const std::string& name);

		/// <summary>
		/// 全てのパーティクルを削除
		/// </summary>
		static void RemoveAllParticles();

	public: /// ===タイムライン=== ///

		/// <summary>
		/// タイムライン定義をJSONファイルから読み込む
		/// </summary>
		/// <param name="jsonPath">JSONファイルのパス（Resource/Json/Particles/以下のパス）</param>
		static void LoadParticleTimeline(const std::string& jsonPath);

		/// <summary>
		/// タイムラインを再生（時間に応じて各パーティクルを発生させる）
		/// </summary>
		/// <param name="timelineName">タイムライン名</param>
		/// <param name="basePosition">基準位置</param>
		static void EmitTimeline(const std::string& timelineName, const Vector3& basePosition);

	public: /// ===設定=== ///
		/// <summary>
		/// パーティクルのテクスチャを設定
		/// </summary>
		/// <param name="name">テクスチャを設定するパーティクルの名前</param>
		/// <param name="textureName">設定するテクスチャの名前</param>
		static void SetTexture(const std::string& name, const std::string& textureName);

		/// <summary>
		/// 実行時パラメータ変更（型安全版）
		/// </summary>
		/// <param name="name">対象パーティクルの名前</param>
		/// <param name="param">変更するパラメータ</param>
		/// <param name="value">設定する値</param>
		static void SetParameter(const std::string& name, MiiEngine::ParticleParameter param, float value);

	public: /// ===取得=== ///
		/// <summary>
		/// パーティクル定義を取得
		/// </summary>
		/// <param name="name">取得するパーティクルの名前</param>
		/// <returns>パーティクル定義へのポインタ（存在しない場合はnullptr）</returns>
		static const MiiEngine::ParticleDefinition* GetDefinition(const std::string& name);

		/// <summary>
		/// パーティクル定義が登録されているか確認
		/// </summary>
		/// <param name="name">確認するパーティクルの名前</param>
		/// <returns>登録されている場合はtrue</returns>
		static bool HasDefinition(const std::string& name);

		/// <summary>
		/// アクティブなパーティクル数を取得
		/// </summary>
		/// <param name="name">対象パーティクルの名前</param>
		/// <returns>アクティブなパーティクルの総数</returns>
		static uint32_t GetActiveParticleCount(const std::string& name);

		/// <summary>
		/// 登録されているパーティクル定義の一覧を取得
		/// </summary>
		/// <returns>定義名のリスト</returns>
		static std::vector<std::string> GetDefinitionNames();

		/// <summary>
		/// アクティブなパーティクルグループ数を取得
		/// </summary>
		/// <param name="name">対象パーティクルの名前</param>
		/// <returns>アクティブなグループ数</returns>
		static size_t GetActiveGroupCount(const std::string& name);
	};
}