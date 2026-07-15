#pragma once
/// ===Include=== ///
// Collider
#include "Engine/Collider/OBBCollider.h"
#include "Engine/Collider/SphereCollider.h"
#include "Engine/Collider/ColliderConcept.h"

/// ===using宣言=== ///
using MiiEngine::IsCollider;

/// ===前方宣言=== ///
template<typename TCollider> requires IsCollider<TCollider>
class GameCharacter;

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
    void HandleSphereSphereCollision(GameCharacter<MiiEngine::SphereCollider>* sphereCharacterA, GameCharacter<MiiEngine::SphereCollider>* sphereCharacterB, const float pushBackRatio = 1.0f);

    /// <summary>
    /// 2つのOBBコライダーを持つゲームキャラクター間の衝突処理
    /// </summary>
    /// <param name="obbCharacterA">衝突の対象となる1つ目のOBBコライダーを持つゲームキャラクターへのポインター。</param>
    /// <param name="obbCharacterB">衝突の対象となる2つ目のOBBコライダーを持つゲームキャラクターへのポインター。</param>
    /// <param name="pushBackRatio">衝突時に適用する押し戻しの強さを示す係数（既定値は1.0f）。値が大きいほど強く押し戻します。</param>
    void HandleOBBOBBCollision(GameCharacter<MiiEngine::OBBCollider>* obbCharacterA, GameCharacter<MiiEngine::OBBCollider>* obbCharacterB, const float pushBackRatio = 1.0f);

    /// <summary>
    /// 球とOBB間の衝突処理
    /// </summary>
    /// <typeparam name="TColliderA">characterA が使用するコライダの型。</typeparam>
    /// <typeparam name="TColliderB">characterB が使用するコライダの型。</typeparam>
    /// <param name="characterA">衝突対象の最初のキャラクター。コライダ型 TColliderA を持つ GameCharacter へのポインタ。</param>
    /// <param name="characterB">衝突対象の二番目のキャラクター。コライダ型 TColliderB を持つ GameCharacter へのポインタ。</param>
    /// <param name="pushBackRatio">衝突解決時に適用する押し戻し（push-back）の倍率。デフォルトは 1.0f。値を小さくすると押し戻しが弱くなり、0 にすると押し戻しを行わない。</param>
    template<typename TColliderA, typename TColliderB> requires IsCollider<TColliderA>&& IsCollider<TColliderB>
    void HandleSphereOBBCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, const float pushBackRatio = 1.0f);

    /// <summary>
    /// 2つのGameCharacter間の衝突処理
    /// </summary>
    /// <typeparam name="TColliderA">characterAに使用されるコライダの型。</typeparam>
    /// <typeparam name="TColliderB">characterBに使用されるコライダの型。</typeparam>
    /// <param name="characterA">衝突する最初のGameCharacterへのポインタ。</param>
    /// <param name="characterB">衝突する2番目のGameCharacterへのポインタ。</param>
    /// <param name="pushBackRatio">衝突時に適用する押し戻し量の比率。既定値は1.0f。</param>
	template<typename TColliderA, typename TColliderB> requires IsCollider<TColliderA>&& IsCollider<TColliderB>
    void ProcessCollision(GameCharacter<TColliderA>* characterA, GameCharacter<TColliderB>* characterB, const float pushBackRatio = 1.0f);

private:

    // 定数定義
	const float EPSILON_ = 1e-6f;

    /// <summary>
    /// 2点間の方向ベクトルを計算する共通処理。
    /// 距離が epsilon 未満の場合は fallbackDirection を採用する。
    /// </summary>
    /// <param name="from">基準となる点（この点から to への方向を求める）。</param>
    /// <param name="to">対象となる点。</param>
    /// <param name="fallbackDirection">距離がepsilon未満の場合に使用する方向。</param>
    /// <param name="epsilon">距離0とみなす閾値。</param>
    /// <param name="outDistance">計算された距離（epsilon未満の場合はepsilonにクランプされる）。</param>
    /// <returns>正規化された方向ベクトル（from - to 方向）。</returns>
    Vector3 CalculatePushBackDirection(const Vector3& from, const Vector3& to, const Vector3& fallbackDirection, float epsilon, float& outDistance) const;

    /// <summary>
    /// 重なり量に応じて2つの位置へ押し戻しを適用する共通処理。
    /// overlapが正の場合のみ適用し、pushBackRatioが1.0でposA側、0.0でposB側のみが押し戻される。
    /// </summary>
    /// <param name="posA">押し戻し対象の位置A（参照渡しで更新される）。</param>
    /// <param name="posB">押し戻し対象の位置B（参照渡しで更新される）。</param>
    /// <param name="direction">押し戻し方向（posA側が+方向）。</param>
    /// <param name="overlap">重なり量。</param>
    /// <param name="pushBackRatio">押し戻しの比率。</param>
    /// <returns>overlapが正で実際に押し戻しを適用した場合はtrue。</returns>
    bool ApplyPushBack(Vector3& posA, Vector3& posB, const Vector3& direction, float overlap, float pushBackRatio) const;

    /// <summary>
    /// GameCharacter からコライダーの種類 (ColliderType) を取得
    /// </summary>
    /// <typeparam name="TCollider">キャラクターが使用するコライダーの型。</typeparam>
    /// <param name="character">TCollider 型のコライダーを持つ GameCharacter へのポインター。対象のキャラクターを指定します。</param>
    /// <returns>指定したキャラクターに関連付けられた ColliderType を返します。</returns>
    template<typename TCollider> requires IsCollider<TCollider>
    MiiEngine::ColliderType GetColliderTypeFromCharacter(GameCharacter<TCollider>* character) const;

    /// <summary>
    /// 与えられた点から、OBBコライダーを持つキャラクター上の最も近い点（最接近点）を計算して返す処理
    /// </summary>
    /// <param name="point">基準となる点（入力、参照渡し）。この点から OBB への最短点を求めます。</param>
    /// <param name="obbCharacter">OBBCollider を持つ GameCharacter へのポインター。計算対象となる OBB の位置・向き・半径などの情報を提供します。</param>
    /// <returns>OBB 上の最も近い点を表す Vector3。</returns>
    Vector3 CalculateClosestPointOnOBBFromCharacter(const Vector3& point, GameCharacter<MiiEngine::OBBCollider>* obbCharacter) const;

    /// <summary>
    /// 与えられた点からOBB上の最も近い点を計算
    /// </summary>
    /// <param name="point">基準となる点。</param>
    /// <param name="obb">計算対象のOBB。</param>
    /// <returns>OBB上の最も近い点を表すVector3。</returns>
    Vector3 CalculateClosestPointOnOBB(const Vector3& point, const MiiEngine::OBB& obb) const;

    /// <summary>
    /// OBBの指定軸上での重なり量を計算
    /// </summary>
    /// <param name="obb1">1つ目のOBB。</param>
    /// <param name="obb2">2つ目のOBB。</param>
    /// <param name="axis">分離軸。</param>
    /// <returns>重なり量（正の値なら重なっている）。</returns>
    float CalculateOverlapOnAxis(const MiiEngine::OBB& obb1, const MiiEngine::OBB& obb2, const Vector3& axis) const;
};