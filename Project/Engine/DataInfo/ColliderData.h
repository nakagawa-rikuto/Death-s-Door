#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace MiiEngine {
    ///-------------------------------------------/// 
    /// ColliderType
    ///-------------------------------------------///
    enum class ColliderType {
        AABB,      // 軸整合バウンディングボックス（矩形）
        Sphere,    // 球体
        OBB,       // 任意方向のバウンディングボックス
        Count      // 配列サイズ用
    };

    ///-------------------------------------------/// 
    /// ColliderName
    ///-------------------------------------------///
    enum class ColliderName {
        Player,    // プレイヤー
        Enemy,     // 敵
        PlayerBullet, // プレイヤーの弾
        PlayerWeapon, // プレイヤーの武器
        EnemyBullet,  // 敵の弾
		EnemyWeapon,  // 敵の武器
        Object,      // 壁
        Ground,     // 床
        Ceiling,   // 天井
        None       // 無し
    };

    ///-------------------------------------------/// 
    /// AABBの構造体
    ///-------------------------------------------///
    struct AABB {
        Vector3 min; //!<最小点
        Vector3 max; //!<最大点
    };

    ///-------------------------------------------/// 
    /// OBBの構造体
    ///-------------------------------------------///
    struct OBB {
        Vector3 center;          // !<中心
        Vector3 axis[3];         // !<座標軸。正規化・直交必須
        Vector3 halfSize;        // !<座標軸方向の長さの半分。中心空面までの距離
    };

    ///-------------------------------------------/// 
    /// Sphereの構造体
    ///-------------------------------------------///
    struct Sphere {
        Vector3 center;  // 中心
        float radius;    // 半径
    };
}

