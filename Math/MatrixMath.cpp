#include "MatrixMath.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>

///=====================================================///
/// 平行移動行列
///=====================================================///
Matrix4x4 Math::MakeTranslateMatrix(const Vector3& translate) {
	// 単位行列を初期化
	Matrix4x4 result = MakeIdentity4x4();

	// 平行移動成分をセット
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

///=====================================================///
/// 拡大縮小行列
///=====================================================///
Matrix4x4 Math::MakeScaleMatrix(const Vector3& scale) {
	// 単位行列を初期化
	Matrix4x4 result = MakeIdentity4x4();

	// 拡大縮小成分をセット
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

///=====================================================///
/// 回転行列
///=====================================================///
// X軸
Matrix4x4 Math::MakeRotateXMatrix(float radian) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	// 行列の計算
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}
// Y軸
Matrix4x4 Math::MakeRotateYMatrix(float radian) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	// 行列の計算
	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}
// Z軸
Matrix4x4 Math::MakeRotateZMatrix(float radian) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	// 行列の計算
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
}
// Quaternion
Matrix4x4 Math::MakeRotateQuaternionMatrix(const Quaternion q) {
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	Matrix4x4 result;
	result.m[0][0] = 1.0f - 2.0f * (yy + zz);
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = 1.0f - 2.0f * (xx + zz);
	result.m[1][2] = 2.0f * (yz + wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = 1.0f - 2.0f * (xx + yy);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

///=====================================================///
/// 三次元アフィン変換
///=====================================================///
// EulerTransform
Matrix4x4 Math::MakeAffineEulerMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	// 拡大縮小行列
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	// 回転行列
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	// 平行移動行列
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	// 合成（スケール -> 回転 -> 平行移動）
	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
}
// QuaternionTransform
Matrix4x4 Math::MakeAffineQuaternionMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	// 拡大縮小行列
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	// 回転行列
	Matrix4x4 rotateMatrix = MakeRotateQuaternionMatrix(rotate);

	// 平行移動行列
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	// 合成（スケール -> 回転 -> 平行移動）
	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
}



///=====================================================///
/// 単位行列
///=====================================================///
// Matrix3x3
Matrix3x3 Math::MakeIdentity3x3() {
	// ゼロ初期化
	Matrix3x3 result = {};

	// 対角成分を1に設定
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;

	return result;
}
// Matrix4x4
Matrix4x4 Math::MakeIdentity4x4() {
	// ゼロ初期化
	Matrix4x4 result = {};

	// 対角成分を1に設定
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
}

///=====================================================///
/// 転置行列
///=====================================================///
Matrix4x4 Math::TransposeMatrix(const Matrix4x4& matrix) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = matrix.m[j][i];
		}
	}
	return result;
}

///=====================================================///
/// ビューポート行列
///=====================================================///
Matrix4x4 Math::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	float scaleX = width / 2.0f;
	float scaleY = height / 2.0f;
	float scaleZ = (maxDepth - minDepth);
	float offsetX = left + scaleX;
	float offsetY = top + scaleY;
	float offsetZ = minDepth;

	result = {
		{{scaleX, 0, 0, 0},
		 {0, -scaleY, 0, 0}, // - scaleYは、ビューポートのY軸が反転しているため。
		 {0, 0, scaleZ, 0},
		 {offsetX, offsetY, offsetZ, 1}}
	};

	return result;
}

///=====================================================///
/// 透視影行列
///=====================================================///
Matrix4x4 Math::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {

	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	float tanHalfFovY = tan(fovY * 0.5f);
	float scaleX = 1.0f / (aspectRatio * tanHalfFovY);
	float scaleY = 1.0f / tanHalfFovY;
	float nearMinusFar = farClip - nearClip;

	result = {
		{{scaleX, 0, 0, 0}, {0, scaleY, 0, 0}, {0, 0, farClip / nearMinusFar, 1}, {0, 0, (-farClip * nearClip) / nearMinusFar, 0}}
	};

	return result;
}

///=====================================================///
/// 正射影行列
///=====================================================///
Matrix4x4 Math::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();

	// 正射影平面の範囲から正射影行列を構築する
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = -2.0f / (farClip - nearClip);
	result.m[3][0] = -(right + left) / (right - left);
	result.m[3][1] = -(top + bottom) / (top - bottom);
	result.m[3][2] = -(farClip + nearClip) / (farClip - nearClip);

	return result;
}



///=====================================================///
/// 逆行列
///=====================================================///
// Matrix3x3
Matrix3x3 Math::Inverse3x3(const Matrix3x3& matrix) {
	Matrix3x3 result = MakeIdentity3x3();

	// 行列式を計算
	float det =
		matrix.m[0][0] * (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) -
		matrix.m[0][1] * (matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]) +
		matrix.m[0][2] * (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]);

	// 行列式がゼロならば逆行列は存在しない
	if (det == 0.0f) {
		return result; // 単位行列を返す（またはエラー処理）
	}

	float invDet = 1.0f / det;

	// 余因子行列を計算し転置
	result.m[0][0] = invDet * (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]);
	result.m[0][1] = invDet * (matrix.m[0][2] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][2]);
	result.m[0][2] = invDet * (matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1]);

	result.m[1][0] = invDet * (matrix.m[1][2] * matrix.m[2][0] - matrix.m[1][0] * matrix.m[2][2]);
	result.m[1][1] = invDet * (matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0]);
	result.m[1][2] = invDet * (matrix.m[0][2] * matrix.m[1][0] - matrix.m[0][0] * matrix.m[1][2]);

	result.m[2][0] = invDet * (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]);
	result.m[2][1] = invDet * (matrix.m[0][1] * matrix.m[2][0] - matrix.m[0][0] * matrix.m[2][1]);
	result.m[2][2] = invDet * (matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0]);

	return result;
}
// Matrix4x4
Matrix4x4 Math::Inverse4x4(const Matrix4x4& matrix) {
	// 単位行列で初期化
	Matrix4x4 result = MakeIdentity4x4();
	float det;

	// 行列の行列式を計算
	det =
		matrix.m[0][0] *
		(matrix.m[1][1] * (matrix.m[2][2] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][2]) - matrix.m[1][2] * (matrix.m[2][1] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][1]) + matrix.m[1][3] * (matrix.m[2][1] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][1])) -
		matrix.m[0][1] *
		(matrix.m[1][0] * (matrix.m[2][2] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][2]) - matrix.m[1][2] * (matrix.m[2][0] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][0]) + matrix.m[1][3] * (matrix.m[2][0] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][0])) +
		matrix.m[0][2] *
		(matrix.m[1][0] * (matrix.m[2][1] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][1]) - matrix.m[1][1] * (matrix.m[2][0] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][0]) + matrix.m[1][3] * (matrix.m[2][0] * matrix.m[3][1] - matrix.m[2][1] * matrix.m[3][0])) -
		matrix.m[0][3] *
		(matrix.m[1][0] * (matrix.m[2][1] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][1]) - matrix.m[1][1] * (matrix.m[2][0] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][0]) + matrix.m[1][2] * (matrix.m[2][0] * matrix.m[3][1] - matrix.m[2][1] * matrix.m[3][0]));

	// 行列式がゼロに近い場合は特異行列と判断（逆行列を計算できない）
	if (/*fabs(det) < 1e-6f*/ det == 0) {
		// 特異行列の場合の処理（例えば、単位行列を返す、またはエラー処理）
		return result; // 単位行列またはエラー値
	}

	float invDet = 1.0f / det;

	// 余因子行列を計算
	result.m[0][0] = invDet * (matrix.m[1][1] * (matrix.m[2][2] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][2]) - matrix.m[1][2] * (matrix.m[2][1] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][1]) +
		matrix.m[1][3] * (matrix.m[2][1] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][1]));
	result.m[0][1] = -invDet * (matrix.m[0][1] * (matrix.m[2][2] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][2]) - matrix.m[0][2] * (matrix.m[2][1] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][1]) +
		matrix.m[0][3] * (matrix.m[2][1] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][1]));
	result.m[0][2] = invDet * (matrix.m[0][1] * (matrix.m[1][2] * matrix.m[3][3] - matrix.m[1][3] * matrix.m[3][2]) - matrix.m[0][2] * (matrix.m[1][1] * matrix.m[3][3] - matrix.m[1][3] * matrix.m[3][1]) +
		matrix.m[0][3] * (matrix.m[1][1] * matrix.m[3][2] - matrix.m[1][2] * matrix.m[3][1]));
	result.m[0][3] = -invDet * (matrix.m[0][1] * (matrix.m[1][2] * matrix.m[2][3] - matrix.m[1][3] * matrix.m[2][2]) - matrix.m[0][2] * (matrix.m[1][1] * matrix.m[2][3] - matrix.m[1][3] * matrix.m[2][1]) +
		matrix.m[0][3] * (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]));

	result.m[1][0] = -invDet * (matrix.m[1][0] * (matrix.m[2][2] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][2]) - matrix.m[1][2] * (matrix.m[2][0] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][0]) +
		matrix.m[1][3] * (matrix.m[2][0] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][0]));
	result.m[1][1] = invDet * (matrix.m[0][0] * (matrix.m[2][2] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][2]) - matrix.m[0][2] * (matrix.m[2][0] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][0]) +
		matrix.m[0][3] * (matrix.m[2][0] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][0]));
	result.m[1][2] = -invDet * (matrix.m[0][0] * (matrix.m[1][2] * matrix.m[3][3] - matrix.m[1][3] * matrix.m[3][2]) - matrix.m[0][2] * (matrix.m[1][0] * matrix.m[3][3] - matrix.m[1][3] * matrix.m[3][0]) +
		matrix.m[0][3] * (matrix.m[1][0] * matrix.m[3][2] - matrix.m[1][2] * matrix.m[3][0]));
	result.m[1][3] = invDet * (matrix.m[0][0] * (matrix.m[1][2] * matrix.m[2][3] - matrix.m[1][3] * matrix.m[2][2]) - matrix.m[0][2] * (matrix.m[1][0] * matrix.m[2][3] - matrix.m[1][3] * matrix.m[2][0]) +
		matrix.m[0][3] * (matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]));

	result.m[2][0] = invDet * (matrix.m[1][0] * (matrix.m[2][1] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][1]) - matrix.m[1][1] * (matrix.m[2][0] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][0]) +
		matrix.m[1][3] * (matrix.m[2][0] * matrix.m[3][1] - matrix.m[2][1] * matrix.m[3][0]));
	result.m[2][1] = -invDet * (matrix.m[0][0] * (matrix.m[2][1] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][1]) - matrix.m[0][1] * (matrix.m[2][0] * matrix.m[3][3] - matrix.m[2][3] * matrix.m[3][0]) +
		matrix.m[0][3] * (matrix.m[2][0] * matrix.m[3][1] - matrix.m[2][1] * matrix.m[3][0]));
	result.m[2][2] = invDet * (matrix.m[0][0] * (matrix.m[1][1] * matrix.m[3][3] - matrix.m[1][3] * matrix.m[3][1]) - matrix.m[0][1] * (matrix.m[1][0] * matrix.m[3][3] - matrix.m[1][3] * matrix.m[3][0]) +
		matrix.m[0][3] * (matrix.m[1][0] * matrix.m[3][1] - matrix.m[1][1] * matrix.m[3][0]));
	result.m[2][3] = -invDet * (matrix.m[0][0] * (matrix.m[1][1] * matrix.m[2][3] - matrix.m[1][3] * matrix.m[2][1]) - matrix.m[0][1] * (matrix.m[1][0] * matrix.m[2][3] - matrix.m[1][3] * matrix.m[2][0]) +
		matrix.m[0][3] * (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]));

	result.m[3][0] = -invDet * (matrix.m[1][0] * (matrix.m[2][1] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][1]) - matrix.m[1][1] * (matrix.m[2][0] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][0]) +
		matrix.m[1][2] * (matrix.m[2][0] * matrix.m[3][1] - matrix.m[2][1] * matrix.m[3][0]));
	result.m[3][1] = invDet * (matrix.m[0][0] * (matrix.m[2][1] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][1]) - matrix.m[0][1] * (matrix.m[2][0] * matrix.m[3][2] - matrix.m[2][2] * matrix.m[3][0]) +
		matrix.m[0][2] * (matrix.m[2][0] * matrix.m[3][1] - matrix.m[2][1] * matrix.m[3][0]));
	result.m[3][2] = -invDet * (matrix.m[0][0] * (matrix.m[1][1] * matrix.m[3][2] - matrix.m[1][2] * matrix.m[3][1]) - matrix.m[0][1] * (matrix.m[1][0] * matrix.m[3][2] - matrix.m[1][2] * matrix.m[3][0]) +
		matrix.m[0][2] * (matrix.m[1][0] * matrix.m[3][1] - matrix.m[1][1] * matrix.m[3][0]));
	result.m[3][3] = invDet * (matrix.m[0][0] * (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) - matrix.m[0][1] * (matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]) +
		matrix.m[0][2] * (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]));

	return result;
}

///=====================================================///
/// 座標変換
///=====================================================///
// 座標変換（平行移動を加味する）
Vector3 Math::TransformCoordinates(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f);

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
// ベクトル変換（平行移動を加味しない）
Vector3 Math::TransformVector(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];
	return result;
}
// 法線ベクトルの変換（逆転置行列を使用）
Vector3 Math::TransformNormal(const Vector3& normal, const Matrix4x4& matrix) {
	// 逆転置行列を使用
	Matrix4x4 inverseTransposeMatrix = TransposeMatrix(Inverse4x4(matrix));
	// ベクトル変換
	return TransformVector(normal, inverseTransposeMatrix);
}

///=====================================================/// 
/// 行列を変換する為の関数
///=====================================================///
// Quaternion
Quaternion Math::MatrixToQuaternion(const Matrix4x4& m) {
	Quaternion q;
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if (trace > 0.0f) {
		float s = std::sqrt(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
	} else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) {
		float s = std::sqrt(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;
		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	} else if (m.m[1][1] > m.m[2][2]) {
		float s = std::sqrt(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;
		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	} else {
		float s = std::sqrt(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;
		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25f * s;
	}

	return q;
}
