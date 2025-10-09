
#pragma once
/// ===Include=== ///
#include "Engine/Collider/AABBCollider.h"

///=====================================================/// 
/// Ground
///=====================================================///
class Ground : public AABBCollider {
public:
	Ground() = default;
	~Ground();

	// 初期化
	void Initialize()override;
	// 更新
	void Update()override;
	// 描画
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal)override;

public:
	// 衝突時の処理
	void OnCollision(Collider* collider)override;
};
