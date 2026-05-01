#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

namespace Math {
	
	/// <summary>
	/// 円周率（π）を返す
	/// </summary>
	/// <returns>円周率 π を float 型で返します。</returns>
	float Pi();

	/// <summary>
	/// 角度を標準的な範囲に正規化
	/// </summary>
	/// <param name="angle">正規化する角度。通常はラジアンで表されます。</param>
	/// <returns>正規化された角度（入力と同じ単位）。通常は -π〜π または 0〜2π の範囲に収まるように変換されます。</returns>
	float NormalizeAngle(float angle);

	/// <summary>
	/// 2つの3次元ベクトルの外積（クロス積）を計算して返す
	/// </summary>
	/// <param name="v1">外積の左オペランドとなるベクトル（const 参照で渡され、関数内では変更されません）。</param>
	/// <param name="v2">外積の右オペランドとなるベクトル（const 参照で渡され、関数内では変更されません）。</param>
	/// <returns>v1 × v2 を表す Vector3。外積は順序に依存します。</returns>
	Vector3 Cross(const Vector3& v1, const Vector3& v2);
	
	/// <summary>
	/// Quaternionのノルム（大きさ）を計算して返す
	/// </summary>
	/// <param name="quaternion">計算対象の四元数。const参照で渡されます。</param>
	/// <returns>四元数のノルム（長さ）。通常は √(w² + x² + y² + z²) の値を返します。</returns>
	float Norm(const Quaternion& quaternion);

	/// <summary>
	/// Quaternionのノルムの二乗を計算して返す
	/// </summary>
	/// <param name="quaternion">計算対象の四元数。</param>
	/// <returns>四元数のノルムの二乗を float 型で返します。</returns>
	float NormSquared(const Quaternion& quaternion);

	/// <summary>
	/// 恒等（単位）クォータニオンを返す
	/// </summary>
	/// <returns>恒等（単位）クォータニオン。通常は (x=0, y=0, z=0, w=1) を表し、回転を表さないクォータニオンです。</returns>
	Quaternion IdentityQuaternion();

	/// <summary>
	/// Quaternionの共役を計算する
	/// </summary>
	/// <param name="quaternion">共役を求める入力の四元数。const 参照で渡されます。</param>
	/// <returns>与えられた四元数の共役。スカラー成分はそのままで、ベクトル成分（i, j, k）は符号が反転します。</returns>
	Quaternion Conjugate(const Quaternion& quaternion);

	/// <summary>
	/// 与えられたQuaternionの逆（乗法逆元）を返す
	/// </summary>
	/// <param name="quaternion">逆を求める四元数への参照。関数内で変更されません。</param>
	/// <returns>入力の四元数の乗法逆元を表す四元数。入力がゼロ四元数の場合は逆が定義されないことがあります。</returns>
	Quaternion Inverse(const Quaternion& quaternion);

	/// <summary>
	/// 指定した軸と角度から回転を表すQuaternionを作成する
	/// </summary>
	/// <param name="axis">回転軸を表すベクトル。通常は正規化された（単位）ベクトルを渡します。</param>
	/// <param name="angle">軸まわりの回転角。通常はラジアン単位で指定します。</param>
	/// <returns>指定した軸と角度を表す回転を表現する四元数（Quaternion）。</returns>
	Quaternion MakeRotateAxisAngle(const Vector3& axis, float angle);

	/// <summary>
	/// クォータニオンを使ってベクトルを回転させる
	/// </summary>
	/// <param name="vector">回転対象の3次元ベクトル。</param>
	/// <param name="quaternion">適用する回転を表すクォータニオン。</param>
	/// <returns>回転を適用した後の3次元ベクトル。</returns>
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

	/// <summary>
	/// クォータニオンからX軸周りの回転角を取得
	/// </summary>
	/// <param name="quaternion">回転を表すクォータニオン。X軸周りの角度を計算するために使用されます。</param>
	/// <returns>X軸周りの角度を表すfloat値。角度の単位（ラジアンまたは度）は実装仕様によります。</returns>
	float GetXAngle(const Quaternion& quaternion);

	/// <summary>
	/// QuaternionからY軸周りの角度を取得
	/// </summary>
	/// <param name="quaternion">回転を表す四元数。Y軸周りの角度を計算するために使用されます。</param>
	/// <returns>Y軸周りの回転角をfloat型で返します（角度の単位は実装に依存します）。</returns>
	float GetYAngle(const Quaternion& quaternion);

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
	Quaternion QuaternionFromVector(const Vector3& vector);

	/// <summary>
	/// 指定した前方ベクトルと上方向ベクトルに基づいて向きを表すQuaternionを生成
	/// </summary>
	/// <param name="forward">対象が向く前方方向を表すベクトル。通常は正規化されたベクトルを渡し、ゼロベクトルは避けてください。</param>
	/// <param name="up">上方向を定義するベクトル。world上方向やオブジェクトの上方向を指定します。forwardとほぼ平行にならないようにしてください。</param>
	/// <returns>forwardを前方、upを上向きとする回転を表す四元数。forwardとupが不適切（ゼロまたは同一直線上）な場合は結果が不安定になるか、実装依存の振る舞いになります。</returns>
	Quaternion LookRotation(Vector3 forward, Vector3 up);

	/// <summary>
	/// X軸まわりの回転を表すQuaternionを生成
	/// </summary>
	/// <param name="angle">X軸まわりの回転角（ラジアン単位）。</param>
	/// <returns>指定した角度のX軸回転を表すQuaternionを返します。</returns>
	Quaternion RotateX(float angle);

	/// <summary>
	/// Y軸周りの回転を表すQuaternionを生成
	/// </summary>
	/// <param name="angle">Y軸回転の角度（関数やライブラリで使用される単位に従います。通常はラジアン）。</param>
	/// <returns>指定した角度でY軸周りに回転する四元数。</returns>
	Quaternion RotateY(float angle);

	/// <summary>
	/// 指定した角度でZ軸まわりの回転を表すQuaternionを返
	/// </summary>
	/// <param name="angle">Z軸まわりの回転角（通常はラジアン）。</param>
	/// <returns>指定角度のZ軸回転を表す Quaternion（四元数）。</returns>
	Quaternion RotateZ(float angle);

	/// <summary>
	/// 与えられた現在のY回転にピッチ回転（X軸周り）を適用した新しい四元数を生成して返します。
	/// </summary>
	/// <param name="currentYRotation">現在のY回転を表す四元数（const 参照）。この四元数にピッチを適用して結果を作成します。</param>
	/// <param name="pitchAngleDegrees">適用するピッチ角（度単位）。正の角度の向きは座標系の定義に依存します。</param>
	/// <returns>ピッチが適用された新しい Quaternion。元の四元数は変更されません。</returns>
	Quaternion ApplyPitchToCurrentRotation(const Quaternion& currentYRotation, float pitchAngleDegrees);

#pragma region Lerp・SLerp
	/// <summary>
	/// start と end の間を t に応じて線形補間してその値を返す
	/// </summary>
	/// <param name="start">補間の開始値。t が 0 のときに返される値。</param>
	/// <param name="end">補間の終了値。t が 1 のときに返される値。</param>
	/// <param name="t">補間係数（通常 0〜1）。0 で start、1 で end を返します。範囲外の値はそのまま外挿として扱われます。</param>
	/// <returns>指定した t に対応する線形補間（float）結果。</returns>
	float Lerp(float start, float end, float t);

	/// <summary>
	/// 2つのベクトルを線形補間して、指定した係数 t に対応する位置のベクトルを返す
	/// </summary>
	/// <param name="start">補間の開始ベクトル（const 参照）。</param>
	/// <param name="end">補間の終了ベクトル（const 参照）。</param>
	/// <param name="t">補間係数。0 は start、1 は end を表します。範囲外の値では外挿が行われます。</param>
	/// <returns>start と end を線形補間した結果の Vector3（t=0 で start、t=1 で end）。</returns>
	Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

	/// <summary>
	/// 2つのQuaternionを線形補間する関数
	/// </summary>
	/// <param name="start">補間の開始となるクォータニオン（入力）。</param>
	/// <param name="end">補間の終了となるクォータニオン（入力）。</param>
	/// <param name="t">補間係数。0.0でstart、1.0でendを表す。通常は0.0〜1.0の範囲で使用する。</param>
	/// <returns>指定した補間係数に基づいて得られる補間済みのクォータニオン。</returns>
	Quaternion Lerp(const Quaternion& start, const Quaternion& end, float t);

	/// <summary>
	/// 2つのVector4を線形補間する関数
	/// </summary>
	/// <param name="start">補間の開始となるベクトル（入力）。</param>
	/// <param name="end">補間の終了となるベクトル（入力）。</param>
	/// <param name="t">補間係数。0.0でstart、1.0でendを表す。通常は0.0〜1.0の範囲で使用する。</param>
	/// <returns>指定した補間係数に基づいて得られる補間済みのベクトル。</returns>
	Vector4 Lerp(const Vector4& start, const Vector4& end, float t);

	/// <summary>
	/// 2つのベクトル間を球面線形補間（SLerp）で補間
	/// </summary>
	/// <param name="start">補間の開始ベクトル（参照）。</param>
	/// <param name="end">補間の終了ベクトル（参照）。</param>
	/// <param name="t">補間係数。通常は 0.0 が start、1.0 が end を表す。</param>
	/// <returns>指定した t に対応する補間後のベクトル。入力ベクトルが正規化されている場合、結果も正規化されます。</returns>
	Vector3 SLerp(const Vector3& start, const Vector3& end, float t);

	/// <summary>
	/// 指定した補間係数に基づき、2つのクォータニオン間を球面線形補間（Slerp）して結果のクォータニオンを返す
	/// </summary>
	/// <param name="start">補間の開始となるクォータニオン（参照）。</param>
	/// <param name="end">補間の終了となるクォータニオン（参照）。</param>
	/// <param name="t">補間係数。通常は 0.0（start）から 1.0（end）の範囲で指定します。範囲外の値は外挿となる場合があります。</param>
	/// <returns>指定した係数に対応する補間後のクォータニオン。通常は正規化されたクォータニオンが返されます。</returns>
	Quaternion SLerp(const Quaternion& start, const Quaternion& end, float t);

	/// <summary>
	/// 2つのVector4を球面線形補間（SLerp）する関数
	/// </summary>
	/// <param name="start">補間の開始となるベクトル（入力）。</param>
	/// <param name="end">補間の終了となるベクトル（入力）。</param>
	/// <param name="t">補間係数。0.0でstart、1.0でendを表す。通常は0.0〜1.0の範囲で使用する。</param>
	/// <returns>指定した補間係数に基づいて得られる補間済みのベクトル。</returns>
	Vector4 SLerp(const Vector4& start, const Vector4& end, float t);
#pragma endregion
}


