#pragma once
/// ===Include=== ///
#include "Engine/Collider/Base/Collider.h"

///=====================================================/// 
/// SphereCollider
///=====================================================///
class SphereCollider : public Collider {
public:

	SphereCollider() = default;
	~SphereCollider() = default;

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
	Sphere GetSphere()const;

protected:
	Sphere sphere_;
};

