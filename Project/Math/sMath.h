#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

namespace Math {
	/// ===π=== ///
	float Pi();

	/// ===クロス積=== ///
	Vector3 Cross(const Vector3& v1, const Vector3& v2);

	/// ===Quaternion=== ///
	// Quaternionのnormを返す
	float Norm(const Quaternion& quaternion);
	// Quaternionのnormの２乗を返す
	float NormSquared(const Quaternion& quaternion);
	// 単位Quaternionを返す
	Quaternion IdentityQuaternion();
	// 共役Quaternionを返す
	Quaternion Conjugate(const Quaternion& quaternion);
	// 逆Quaternionを返す
	Quaternion Inverse(const Quaternion& quaternion);
	// 任意軸回転Quaternionを返す
	Quaternion MakeRotateAxisAngle(const Vector3& axis, float angle);
	// Quaternionの回転結果をベクトルで返す
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	// QuaternionからX軸の角度を取る関数
	float GetXAngle(const Quaternion& quaternion);
	// QuaternionからY軸の角度を取る関数
	float GetYAngle(const Quaternion& quaternion);
	// QuaternionをEulerAngles(Vector3)に変換する関数
	Vector3 QuaternionToEuler(const Quaternion& quaternion);
	// Vector3からQuaternionに変換する関数
	Quaternion QuaternionFromVector(const Vector3& vector);
	// ある方向（forward）を向くクォータニオン（回転）を作る
	Quaternion LookRotation(Vector3 forward, Vector3 up);
	// 
	Quaternion RotateX(float angle);
	Quaternion RotateY(float angle);
	Quaternion RotateZ(float angle);

	/// ====== ///
	// 
	float NormalizeAngle(float angle);
}


