#pragma once
/// ===Include=== ///
#include "Engine/Collider/Base/Collider.h"
#include "Engine/DataInfo/ColliderData.h"
#include "Math/sMath.h"

/// ===前方宣言=== ///
template<typename TCollider>
class GameCharacter;
class SphereCollider;
class OBBCollider;

///=====================================================/// 
/// GameCharacterCollision - GameCharacter専用の衝突処理クラス
/// GameCharacterクラスのOnCollision内で使用される
///=====================================================///
class GameCharacterCollision {
public:
    GameCharacterCollision() = default;
    ~GameCharacterCollision() = default;

	//NOTE:pushBackRatioは押し戻しの比率を調整するパラメータで、0.0fから1.0fの範囲で設定します。0.0はCharacterBのみ押し戻し、1.0はCharacterAのみ押し戻し、0.5は両方均等に押し戻します。

    /// ===GameCharacter用衝突処理=== ///
    // 球体GameCharacter同士の衝突処理
    void HandleSphereSphereCollision(GameCharacter<SphereCollider>* sphereCharacterA, GameCharacter<SphereCollider>* sphereCharacterB, float pushBackRatio = 1.0f);

    // OBBGameCharacter同士の衝突処理
    void HandleOBBOBBCollision(GameCharacter<OBBCollider>* obbCharacterA, GameCharacter<OBBCollider>* obbCharacterB, float pushBackRatio = 1.0f);

    // 球体とOBBのGameCharacter衝突処理
    template<typename TColliderA, typename TColliderB>
    void HandleSphereOBBCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, float pushBackRatio = 1.0f);

    /// ===汎用衝突処理（OnCollision内で呼び出し用）=== ///
    // GameCharacter同士の衝突を自動判定して処理
    template<typename TColliderA, typename TColliderB>
    void ProcessCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, float pushBackRatio = 1.0f);

private:

    // コライダー型の判定
    template<typename TCollider>
    ColliderType GetColliderTypeFromCharacter(GameCharacter<TCollider>* character);

    // OBB上の最近点計算
    Vector3 CalculateClosestPointOnOBBFromCharacter(const Vector3& point, GameCharacter<OBBCollider>* obbCharacter);
};