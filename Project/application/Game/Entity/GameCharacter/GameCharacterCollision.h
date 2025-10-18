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

    /// <summary>
    /// 球形コライダーを持つ2つのゲームキャラクター間の衝突処理
    /// </summary>
    /// <param name="sphereCharacterA">衝突する1つ目のゲームキャラクター（SphereCollider を持つ）へのポインタ。</param>
    /// <param name="sphereCharacterB">衝突する2つ目のゲームキャラクター（SphereCollider を持つ）へのポインタ。</param>
    /// <param name="pushBackRatio">衝突時の押し戻しに適用する比率。既定値は 1.0f。</param>
    void HandleSphereSphereCollision(GameCharacter<SphereCollider>* sphereCharacterA, GameCharacter<SphereCollider>* sphereCharacterB, float pushBackRatio = 1.0f);

    /// <summary>
    /// 2つのOBBコライダーを持つゲームキャラクター間の衝突処理
    /// </summary>
    /// <param name="obbCharacterA">衝突の対象となる1つ目のOBBコライダーを持つゲームキャラクターへのポインター。</param>
    /// <param name="obbCharacterB">衝突の対象となる2つ目のOBBコライダーを持つゲームキャラクターへのポインター。</param>
    /// <param name="pushBackRatio">衝突時に適用する押し戻しの強さを示す係数（既定値は1.0f）。値が大きいほど強く押し戻します。</param>
    void HandleOBBOBBCollision(GameCharacter<OBBCollider>* obbCharacterA, GameCharacter<OBBCollider>* obbCharacterB, float pushBackRatio = 1.0f);

    /// <summary>
    /// 球とOBB間の衝突処理
    /// </summary>
    /// <typeparam name="TColliderA">characterA が使用するコライダの型。</typeparam>
    /// <typeparam name="TColliderB">characterB が使用するコライダの型。</typeparam>
    /// <param name="characterA">衝突対象の最初のキャラクター。コライダ型 TColliderA を持つ GameCharacter へのポインタ。</param>
    /// <param name="characterB">衝突対象の二番目のキャラクター。コライダ型 TColliderB を持つ GameCharacter へのポインタ。</param>
    /// <param name="pushBackRatio">衝突解決時に適用する押し戻し（push-back）の倍率。デフォルトは 1.0f。値を小さくすると押し戻しが弱くなり、0 にすると押し戻しを行わない。</param>
    template<typename TColliderA, typename TColliderB>
    void HandleSphereOBBCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, float pushBackRatio = 1.0f);

    /// <summary>
    /// 2つのGameCharacter間の衝突処理
    /// </summary>
    /// <typeparam name="TColliderA">characterAに使用されるコライダの型。</typeparam>
    /// <typeparam name="TColliderB">characterBに使用されるコライダの型。</typeparam>
    /// <param name="characterA">衝突する最初のGameCharacterへのポインタ。</param>
    /// <param name="characterB">衝突する2番目のGameCharacterへのポインタ。</param>
    /// <param name="pushBackRatio">衝突時に適用する押し戻し量の比率。既定値は1.0f。</param>
    template<typename TColliderA, typename TColliderB>
    void ProcessCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, float pushBackRatio = 1.0f);

private:

    /// <summary>
    /// GameCharacter からコライダーの種類 (ColliderType) を取得
    /// </summary>
    /// <typeparam name="TCollider">キャラクターが使用するコライダーの型。</typeparam>
    /// <param name="character">TCollider 型のコライダーを持つ GameCharacter へのポインター。対象のキャラクターを指定します。</param>
    /// <returns>指定したキャラクターに関連付けられた ColliderType を返します。</returns>
    template<typename TCollider>
    ColliderType GetColliderTypeFromCharacter(GameCharacter<TCollider>* character);

    /// <summary>
    /// 与えられた点から、OBBコライダーを持つキャラクター上の最も近い点（最接近点）を計算して返す処理
    /// </summary>
    /// <param name="point">基準となる点（入力、参照渡し）。この点から OBB への最短点を求めます。</param>
    /// <param name="obbCharacter">OBBCollider を持つ GameCharacter へのポインター。計算対象となる OBB の位置・向き・半径などの情報を提供します。</param>
    /// <returns>OBB 上の最も近い点を表す Vector3。</returns>
    Vector3 CalculateClosestPointOnOBBFromCharacter(const Vector3& point, GameCharacter<OBBCollider>* obbCharacter);
};