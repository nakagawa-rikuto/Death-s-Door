#pragma once
/// ===include=== ///
// Common
#include "Engine/Graphics/3d/Model/ModelCommon.h"

namespace MiiEngine {
	///=====================================================/// 
	/// モデル
	///=====================================================///
	class Model : public ModelCommon {
	public: /// ===基本的な関数=== ///

		Model();
		~Model();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="filename">初期化に使用するファイルのパスまたは名前（const std::string&）。</param>
		/// <param name="type">使用するライトの種類。省略時は LightType::None が使用されます（デフォルト引数）。</param>
		void Initialize(const std::string& filename, LightType type = LightType::None) override;

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;

		/// <summary>
		/// 描画処理
		/// </summary>
		/// <param name="mode">描画に使用する合成（ブレンド）モードを指定します。</param>
		void Draw(BlendMode mode) override;

	public: /// ===Joint=== ///
		// ジョイントのワールド行列を取得
		Matrix4x4 GetJointWorldMatrix(const std::string& jointName) const override;
	};
}

