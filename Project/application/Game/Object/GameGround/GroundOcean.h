#pragma once
/// ===Include=== ///
// Collider
#include "application/Game/Object/GameObject/GameObject.h"
// Ocean
#include "Engine/Graphics/Ocean/FFT/FFTOceanGenerator.h"

///=====================================================/// 
/// GroundOcean
///=====================================================///
class GroundOcean : public GameObject<MiiEngine::AABBCollider> {
public: 

	GroundOcean() = default;
	~GroundOcean();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">BlendModeの適用</param>
	void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void ShowImGui();

	/// <summary>
	/// 波紋を追加します。
	/// </summary>
	/// <param name="pos">波紋の位置。</param>
	/// <param name="radius">波紋の半径。</param>
	/// <param name="strength">波紋の強度。</param>
	void AddRipple(const Vector3& pos, float radius, float strength);

public: /// ===衝突処理=== ///

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collider">衝突先のCollider</param>
	void OnCollision(MiiEngine::Collider* collider)override;

private:

	/// ===オーシャン=== ///
	std::unique_ptr<MiiEngine::FFTOceanGenerator> fftOcean_;
};

