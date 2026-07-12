#pragma once
/// ===Include=== ///
#include <Math/Vector2.h>
#include <Math/Vector3.h>

/// ===前方宣言=== ///
namespace MiiEngine {
	class SpriteCommon;
}

namespace Service {
	///=====================================================/// 
	/// Sprite Service
	///=====================================================///
	class Sprite {
	public:

		/// <summary>
		/// スプライトを追加します。
		/// </summary>
		/// <param name="sprite">追加するスプライト。</param>
		static void AddSprite(MiiEngine::SpriteCommon* sprite);

		/// <summary>
		/// すべてのスプライトをクリアします。
		/// </summary>
		static void ClearSprites();

		/// <summary>
		/// スプライトを削除します。
		/// </summary>
		/// <param name="sprite">削除するスプライトへのポインタ。</param>
		static void RemoveSprite(MiiEngine::SpriteCommon* sprite);

		/// <summary>
		/// 基本スケールを取得します。
		/// </summary>
		/// <returns>基本スケールのベクトル。</returns>
		static Vector2 GetBaseScale();
	};

}

