#pragma once
/// ===include=== ///
// ParticleGroup
#include "Engine/Graphics/Particle/ParticleGroup.h"
#include "Engine/Graphics/Particle/ParticleDefinition.h"
#include "Engine/Graphics/Particle/ParticleParameter.h"
// Timeline
#include "Engine/Graphics/Particle/ParticleTimeline.h"
// c++
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace MiiEngine {
	///=====================================================/// 
	/// ParticleManager
	///=====================================================///
	class ParticleManager {
	public:
		ParticleManager() = default;
		~ParticleManager();

		/// <summary>
		/// パーティクル定義の追加（JSONファイルから）
		/// </summary>
		/// <param name="jsonPath">JSONファイルのパス</param>
		/// <returns>読み込みに成功したかどうか</returns>
		void LoadParticleDefinition(const std::string& jsonPath);

		/// <summary>
		/// パーティクル定義の追加（直接指定）
		/// </summary>
		/// <param name="name">パーティクルの名前</param>
		/// <param name="definition">パーティクル定義</param>
		void AddParticleDefinition(const std::string& name, const ParticleDefinition& definition);

		/// <summary>
		/// パーティクルの発生処理
		/// </summary>
		/// <param name="name">発生させるパーティクルの名前</param>
		/// <param name="translate">発生させる場所</param>
		/// <returns>生成されたParticleGroupへのポインタ（nullptrの場合は失敗）</returns>
		ParticleGroup* Emit(const std::string& name, const Vector3& translate);

		/// <summary>
		/// パーティクル全体の更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// パーティクル全体の描画処理
		/// </summary>
		/// <param name="mode">ブレンドモード</param>
		void Draw(BlendMode mode = BlendMode::kBlendModeAdd);

		/// <summary>
		/// 指定した名前のアクティブなパーティクルをすべて停止
		/// </summary>
		/// <param name="name">停止するパーティクルの名前</param>
		void StopParticle(const std::string& name);

		/// <summary>
		/// パーティクル定義の削除
		/// </summary>
		/// <param name="name">削除するパーティクルの名前</param>
		void RemoveParticleDefinition(const std::string& name);

		/// <summary>
		/// すべてのアクティブなパーティクルを停止
		/// </summary>
		void RemoveAllParticles();

	public: /// ===タイムライン=== ///

		/// <summary>
		/// タイムライン定義をJSONファイルから読み込む
		/// </summary>
		/// <param name="jsonPath">JSONファイルのパス</param>
		void LoadParticleTimeline(const std::string& jsonPath);

		/// <summary>
		/// タイムライン定義を直接追加
		/// </summary>
		/// <param name="name">タイムライン名</param>
		/// <param name="timeline">タイムライン定義</param>
		void AddParticleTimeline(const std::string& name, const ParticleTimeline& timeline);

		/// <summary>
		/// タイムラインを再生（時間に応じて各パーティクルを発生させる）
		/// </summary>
		/// <param name="timelineName">タイムライン名</param>
		/// <param name="basePosition">基準位置</param>
		void EmitTimeline(const std::string& timelineName, const Vector3& basePosition);

		/// <summary>
		/// タイムライン定義が登録されているか確認
		/// </summary>
		bool HasTimeline(const std::string& name) const;

	public: /// ===設定=== ///

		/// <summary>
		/// パーティクルのテクスチャを設定
		/// </summary>
		/// <param name="name">テクスチャを設定するパーティクルの名前</param>
		/// <param name="textureName">設定するテクスチャの名前</param>
		void SetTexture(const std::string& name, const std::string& textureName);

		/// <summary>
		/// 実行時パラメータ変更（型安全版）
		/// </summary>
		/// <param name="name">対象パーティクルの名前</param>
		/// <param name="param">変更するパラメータ</param>
		/// <param name="value">設定する値</param>
		void SetParameter(const std::string& name, ParticleParameter param, float value);

	public: /// ===情報取得=== ///

		/// <summary>
		/// パーティクル定義を取得
		/// </summary>
		/// <param name="name">取得するパーティクルの名前</param>
		/// <returns>パーティクル定義へのポインタ（存在しない場合はnullptr）</returns>
		const ParticleDefinition* GetDefinition(const std::string& name) const;

		/// <summary>
		/// パーティクル定義が登録されているか確認
		/// </summary>
		/// <param name="name">確認するパーティクルの名前</param>
		/// <returns>登録されている場合はtrue</returns>
		bool HasDefinition(const std::string& name) const;

		/// <summary>
		/// アクティブなパーティクル数を取得
		/// </summary>
		/// <param name="name">対象パーティクルの名前</param>
		/// <returns>アクティブなパーティクルの総数</returns>
		uint32_t GetActiveParticleCount(const std::string& name) const;

		/// <summary>
		/// 登録されているパーティクル定義の一覧を取得
		/// </summary>
		/// <returns>定義名のリスト</returns>
		std::vector<std::string> GetDefinitionNames() const;

		/// <summary>
		/// アクティブなパーティクルグループ数を取得
		/// </summary>
		/// <param name="name">対象パーティクルの名前</param>
		/// <returns>アクティブなグループ数</returns>
		size_t GetActiveGroupCount(const std::string& name) const;

	private:
		/// ===アクティブタイムライン管理用内部構造=== ///
		struct ActiveTimeline {
			ParticleTimeline timeline;        // タイムライン定義
			Vector3 basePosition;             // 基準位置
			float currentTime = 0.0f;         // 現在の経過時間
			std::vector<bool> emitted;        // 各エントリが発生済みかどうか
		};

		// 定義ベース
		std::map<std::string, ParticleDefinition> definitions_;

		// タイムライン定義
		std::map<std::string, ParticleTimeline> timelines_;

		// アクティブなタイムラインインスタンス
		std::vector<ActiveTimeline> activeTimelines_;

		// アクティブなパーティクル
		std::vector<std::unique_ptr<ParticleGroup>> activeParticles_;
	};
}