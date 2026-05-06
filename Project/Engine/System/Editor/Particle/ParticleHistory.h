#pragma once
#include "Engine/Graphics/Particle/ParticleDefinition.h"
#include <vector>

namespace MiiEngine {
	///=====================================================/// 
	/// パーティクルの履歴を管理するクラス
	///=====================================================///
	class ParticleHistory {
	public:
		ParticleHistory() = default;
		~ParticleHistory() = default;

		/// <summary>
		/// 状態を保存する
		/// </summary>
		/// <param name="state">状態</param>
		void PushState(const ParticleDefinition& state);

		/// <summary>
		/// Undoが可能か
		/// </summary>
		bool CanUndo() const;

		/// <summary>
		/// Redoが可能か
		/// </summary>
		bool CanRedo() const;

		/// <summary>
		/// Undoした後の状態を返す
		/// </summary>
		ParticleDefinition Undo();

		/// <summary>
		/// Redoした後の状態を返す
		/// </summary>
		ParticleDefinition Redo();

		/// <summary>
		/// 履歴をリセット
		/// </summary>
		void ClearHistory();

	private:
		// 履歴のリスト
		std::vector<ParticleDefinition> history_; 
		// 現在の履歴のインデックス
		int currentIndex_ = -1;	
	};
}


