#pragma once
#include "Vector3.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

namespace Math {
	/// ===平行移動行列=== ///
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
	/// ===拡縮行列=== ///
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	/// ===回転行列=== ///
	// X軸
	Matrix4x4 MakeRotateXMatrix(float radian);
	// Y軸
	Matrix4x4 MakeRotateYMatrix(float radian);
	// Z軸
	Matrix4x4 MakeRotateZMatrix(float radian);
	// Quaternion
	Matrix4x4 MakeRotateQuaternionMatrix(const Quaternion q);
	/// ===三次元アフィン変換=== ///
	// EulerTransform(Vector3, Vector3, Vector3)
	Matrix4x4 MakeAffineEulerMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	// QuaternionTransform(Vector3, Quaternion, Vector3)
	Matrix4x4 MakeAffineQuaternionMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
	/// ===単位行列=== ///
	Matrix3x3 MakeIdentity3x3();
	Matrix4x4 MakeIdentity4x4();
	/// ==転置行列==== ///
	Matrix4x4 TransposeMatrix(const Matrix4x4& m);
	/// ===ビューポート変換行列=== ///
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
	/// ===透視影行列=== ///
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	/// ===正射影行列=== ///
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	/// ===逆行列=== ///
	Matrix3x3 Inverse3x3(const Matrix3x3& matrix);
	Matrix4x4 Inverse4x4(const Matrix4x4& matrix);


	/// ===座標変換=== ///
	// 座標変換
	Vector3 TransformCoordinates(const Vector3& translate, const Matrix4x4& matrix);
	// ベクトル変換
	Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix);
	// 法線ベクトルの変換
	Vector3 TransformNormal(const Vector3& normal, const Matrix4x4& matrix);

	/// ===行列を変換する為の関数=== ///
	Quaternion MatrixToQuaternion(const Matrix4x4& matrix);
}



