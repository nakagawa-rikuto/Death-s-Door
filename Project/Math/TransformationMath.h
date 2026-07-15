#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <optional>

namespace Math {

	/// <summary>
	/// ワールド座標をスクリーン座標に変換します。
	/// </summary>
	/// <param name="worldPos">変換するワールド座標。</param>
	/// <returns>変換されたスクリーン座標。</returns>
	Vector2 WorldToScreen(const Vector3& worldPos);

	/// <summary>
	/// スクリーン座標をワールド座標に変換します。
	/// </summary>
	/// <param name="screenPos">変換するスクリーン座標。</param>
	/// <returns>変換されたワールド座標。</returns>
	Vector3 ScreenToWorld(const Vector2& screenPos);

	/// <summary>
	/// スクリーン座標から、指定したY平面とレイの交点をワールド座標で取得します。
	/// (Y軸を使用しないゲーム向けに、XZ平面上の狙い位置を求めるための関数)
	/// </summary>
	/// <param name="screenPos">変換するスクリーン座標。</param>
	/// <param name="planeY">交差させる水平面のY座標。</param>
	/// <returns>交点のワールド座標（該当する交点が存在しない場合はstd::nullopt）。</returns>
	std::optional<Vector3> ScreenToWorldOnPlane(const Vector2& screenPos, float planeY);

	/// <summary>
	/// Quaternionを対応するオイラー角に変換
	/// </summary>
	/// <param name="quaternion">変換対象の四元数。回転を表します。</param>
	/// <returns>対応するオイラー角を格納した Vector3。各成分は軸周りの回転角（順序や角度の単位は実装に依存します）。</returns>
	Vector3 QuaternionToEuler(const Quaternion& quaternion);

	/// <summary>
	/// 3次元ベクトルからQuaternionを生成
	/// </summary>
	/// <param name="vector">四元数に変換する3次元ベクトル（const参照）。</param>
	/// <returns>入力ベクトルに基づいて生成された四元数。</returns>
	Quaternion EulerToQuaternion(const Vector3& euler);
}
