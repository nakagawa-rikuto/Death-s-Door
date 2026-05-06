#include "ParticleHistory.h"


namespace MiiEngine {
	///-------------------------------------------/// 
	/// 状態の保存
	///-------------------------------------------///
	void ParticleHistory::PushState(const ParticleDefinition& state) {
		// もし過去の状態に戻っている途中で新しい操作をしたら、それ以降の未来（Redo履歴）を消す
		if (currentIndex_ < static_cast<int>(history_.size()) - 1) {
			history_.erase(history_.begin() + currentIndex_ + 1, history_.end());
		}

		// 履歴に追加して、インデックスを進める
		history_.push_back(state);
		currentIndex_ = static_cast<int>(history_.size()) - 1;

		// メモリの無駄を防ぐため、履歴の上限（例：50回）を超えたら古いものを消す処理を入れても良いです
		if (history_.size() > 50) {
			history_.erase(history_.begin());
			currentIndex_--;
		}
	}

	///-------------------------------------------/// 
	/// 元に戻すことができるか
	///-------------------------------------------///
	bool ParticleHistory::CanUndo() const { return currentIndex_ > 0; }

	///-------------------------------------------/// 
	/// やり直すことができるか
	///-------------------------------------------///
	bool ParticleHistory::CanRedo() const { return currentIndex_ < static_cast<int>(history_.size()) - 1; }

	///-------------------------------------------/// 
	/// Undoした後の状態を返す
	///-------------------------------------------///
	ParticleDefinition ParticleHistory::Undo() {
		if (CanUndo()) {
			currentIndex_--;
		}
		return history_[currentIndex_];
	}

	///-------------------------------------------/// 
	/// Redoした後の状態を返す
	///-------------------------------------------///
	ParticleDefinition ParticleHistory::Redo() {
		if (CanRedo()) {
			currentIndex_++;
		}
		return history_[currentIndex_];
	}

	///-------------------------------------------/// 
	/// 履歴をリセット
	///-------------------------------------------///
	void ParticleHistory::ClearHistory() {
		history_.clear();
		currentIndex_ = -1;
	}
}
