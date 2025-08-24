#pragma once
/// ===Includ=== ///
#include "Engine/Collider/Base/Collider.h"

///-------------------------------------------/// 
/// OBBCollider
///-------------------------------------------///
class OBBCollider : public Collider {
public:

	OBBCollider() = default;
	~OBBCollider() = default;

    // 初期化
    virtual void Initialize() override;
    // 更新
    virtual void Update() override;
    // 描画
    virtual void Draw(BlendMode mode) override;

    // 情報
    void Info();

public: /// ===衝突=== ///
    virtual void OnCollision(Collider* collider) override = 0;

public: /// ===Getter=== ///
    OBB GetOBB() const;

protected:
    OBB obb_;

private:
	// OBBのAxisをQuaternionから設定
    void SetOBBAxisFromQuaternion(OBB& obb, Quaternion rotate);
};

