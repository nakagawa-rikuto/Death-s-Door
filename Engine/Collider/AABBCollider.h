#pragma once
/// ===Include=== ///
#include "Engine/Collider/Base/Collider.h"

///=====================================================/// 
/// AABB
///=====================================================///
class AABBCollider : public Collider {
public:

	AABBCollider() = default;
	~AABBCollider() = default;

	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw(BlendMode mode) override;
	// 情報
	virtual void Information()override;

public: /// ===衝突=== ///
	virtual void OnCollision(Collider* collider) override = 0;

public: /// ===Getter=== ///
	AABB GetAABB() const;

protected:
	AABB aabb_;
};

