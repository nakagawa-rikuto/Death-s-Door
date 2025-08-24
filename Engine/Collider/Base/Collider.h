#pragma once
/// ===Include=== ///
// map
#include <map>
#include <string>
// Object3D
#include "application/Drawing/3d/Object3d.h"
// Math
#include "Engine/DataInfo/ColliderData.h"
// Line
#include "application/Drawing/3d/Line.h"

// ColliderType
enum class ColliderType {
    AABB,      // 軸整合バウンディングボックス（矩形）
    Sphere,    // 球体
    OBB,       // 任意方向のバウンディングボックス
    Count      // 配列サイズ用
};

enum class ColliderName {
    Player,    // プレイヤー
    Enemy,     // 敵
	PlayerBullet, // プレイヤーの弾
    EnemyBullet,  // 敵の弾
    Wall,      // 壁
    Floor,     // 床
    Ceiling,   // 天井
	None       // 無し
};

/// ===前方宣言=== ///
class Camera;

///=====================================================/// 
/// コライダー
///=====================================================///
class Collider {
public:

    Collider() = default;
    virtual ~Collider() = default;

    // 初期化
    virtual void Initialize();
    // 更新処理
    virtual void Update();
	// 描画処理
    virtual void Draw(BlendMode mode);

public: /// ===衝突=== ///
    // 衝突時の応答処理
    virtual void OnCollision(Collider* collider) = 0;

public: /// ===Getter=== ///
    // Type
    ColliderType GetColliderType();
    // CollsisionName
    ColliderName GetColliderName();

public: /// ===Setter=== ///
    // Light
    void SetLight(LightType type);
    void SetLightData(LightInfo light);

    /// ===isCollision=== ///
    void SetIsCollisison(bool flag);

protected:
    // Type
    ColliderType type_;
    // ColliderName
    ColliderName name_;

	// Object3D
	std::unique_ptr<Object3d> object3d_;

    // 衝突フラグ
    bool isCollision_ = false;

    /// ===Debug用=== ///
#ifdef _DEBUG
    // Line
    std::unique_ptr<Line> line_;
    Vector4 lineColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
#endif // _DEBUG
};

