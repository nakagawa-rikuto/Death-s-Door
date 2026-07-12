#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

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
