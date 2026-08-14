#pragma once
// Common
#include "Engine/Graphics/3d/Model/ModelCommon.h"
// Data
#include "Engine/DataInfo/AnimationData.h"
#include "Engine/DataInfo/PipelineStateObjectType.h"

namespace MiiEngine {
	///=====================================================/// 
	/// アニメーションモデル
	///=====================================================///
	class AnimationModel : public ModelCommon {
	public:

		AnimationModel() = default;
		~AnimationModel();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="filename">初期化に使用するファイルの名前またはパスを指定します。</param>
		/// <param name="type">使用するライトの種類を示す LightType 型の値です。</param>
		void Initialize(const std::string& filename, LightType type) override;

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

	public: /// ===Setter=== ///
		// Animation
		void SetAnimation(const std::string& animationName, bool isLoop);

	private: /// ===Variables(変数)=== ///

		/// ===Animation=== ///
		float animationTime_;
		std::map<std::string, Animation> animation_;
		std::string animationName_;
		Skeleton skeleton_;
		SkinCluster skinCluster_;
		bool isLoop_;

	private: /// ===Functions(関数)=== ///

		/// <summary>
		/// 任意の時刻を取得する関数(Vector3)
		/// </summary>
		/// <param name="keyframes">評価に使用する KeyframeVector3 の配列（const 参照）。各キーフレームの値を基に、指定時刻で補間して値を算出します。</param>
		/// <param name="time">評価する時刻。キーフレーム間で補間する位置を指定します。</param>
		/// <returns>指定時刻における補間された Vector3 値を返します。</returns>
		Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

		/// <summary>
		/// 任意の時刻を取得する関数(Quaternion)
		/// </summary>
		/// <param name="keyframes">KeyframeQuaternion 型の要素からなる std::vector。各要素は時刻と対応するクォータニオン値を表します。</param>
		/// <param name="time">評価する時刻（浮動小数点）。関数はこの時刻におけるクォータニオン値を返します。</param>
		/// <returns>指定された時刻における（通常は補間された）Quaternion 値を返します。</returns>
		Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

		/// <summary>
		/// Nodeの階層構造からSkeletonの生成処理
		/// </summary>
		/// <param name="rootNode">スケルトン作成の基点となるノード。作成に必要な情報を参照するために const 参照で渡されます。</param>
		/// <returns>rootNode の構造に基づいて初期化された Skeleton オブジェクト。</returns>
		Skeleton CreateSkeleton(const Node& rootNode);

		/// <summary>
		/// NodeからJointの生成処理
		/// </summary>
		/// <param name="node">ジョイントの作成に必要な情報を持つ Node オブジェクト（入力）。</param>
		/// <param name="parent">親ジョイントのインデックス（存在する場合）。親がいない場合は std::nullopt を渡します。</param>
		/// <param name="joints">作成した Joint を格納する std::vector<Joint>（出力／更新される参照）。</param>
		/// <returns>作成されたジョイントのインデックス（joints 内の位置）を示す int32_t を返します。</returns>
		int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

		/// <summary>
		/// Skeletonに対してAnimationの適用を行う関数
		/// </summary>
		/// <param name="skeleton">アニメーションを適用するSkeletonオブジェクトへの参照。関数はこのオブジェクトの状態を更新します。</param>
		/// <param name="animation">適用するアニメーションデータへの参照（const）。アニメーション情報は変更されません。</param>
		/// <param name="animationTime">アニメーションの適用時刻（秒）。アニメーション内のどの時点を適用するかを指定します。</param>
		void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

		/// <summary>
		/// Skeletonの更新処理
		/// </summary>
		/// <param name="skeleton">更新対象の Skeleton オブジェクトへの参照。関数はこの参照を通じてオブジェクトの状態を変更します。</param>
		void SkeletonUpdate(Skeleton& skeleton);

		/// <summary>
		/// SkinClusterの生成処理
		/// </summary>
		/// <param name="device">ID3D12Device への ComPtr の参照。スキン クラスター作成に使用する Direct3D12 デバイスを指定します（入力専用）。</param>
		/// <param name="skeleton">スケルトン情報（ボーン階層、バインドポーズなど）。スキニング用の骨構造を提供します（入力専用）。</param>
		/// <param name="modelData">モデルのジオメトリとウェイト情報を含む ModelData。メッシュ、頂点ウェイト、関連するバッファなど、クラスター生成に必要なデータを提供します（入力専用）。</param>
		/// <returns>作成された SkinCluster オブジェクト。スキニング情報および関連する GPU リソースを表します。</returns>
		SkinCluster CreateSkinCluster(const ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData);

		/// <summary>
		/// SkinClusterの更新処理
		/// </summary>
		/// <param name="skinCluster">更新対象の SkinCluster への参照。</param>
		/// <param name="skeleton">更新に使用される骨格データの読み取り専用参照。ボーン変換や姿勢情報を提供します。</param>
		void SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton);
	};
}

