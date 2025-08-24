#pragma once

/// <summary>
/// 3x3行列
/// </summary>
struct Matrix3x3 final {
	float m[3][3];
};

/// <summary>
/// 行列の加法
/// </summary>
/// <param name="m1"></param>
/// <param name="m2"></param>
/// <returns></returns>
// Matrix3x3
Matrix3x3 Add(const Matrix3x3& m1, const Matrix3x3& m2);

/// <summary>
/// 行列の減法
/// </summary>
/// <param name="m1"></param>
/// <param name="m2"></param>
/// <returns></returns>
// Matrix3x3
Matrix3x3 Subject(const Matrix3x3& m1, const Matrix3x3& m2);

/// <summary>
/// 行列の積
/// </summary>
/// <param name="m1"></param>
/// <param name="m2"></param>
/// <returns></returns>
// Matrix3x3
Matrix3x3 Multiply(const Matrix3x3& m1, const Matrix3x3& m2);