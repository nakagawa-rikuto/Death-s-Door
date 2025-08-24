#include "Matrix3x3.h"

///=====================================================///
///行列の加法
///=====================================================///
Matrix3x3 Add(const Matrix3x3& m1, const Matrix3x3& m2) {

	Matrix3x3 result;

	result.m[0][0] = m1.m[0][0] + m2.m[0][0];
	result.m[0][1] = m1.m[0][1] + m2.m[0][1];
	result.m[0][2] = m1.m[0][2] + m2.m[0][2];
	result.m[1][0] = m1.m[1][0] + m2.m[1][0];
	result.m[1][1] = m1.m[1][1] + m2.m[1][1];
	result.m[1][2] = m1.m[1][2] + m2.m[1][2];
	result.m[2][0] = m1.m[2][0] + m2.m[2][0];
	result.m[2][1] = m1.m[2][1] + m2.m[2][1];
	result.m[2][2] = m1.m[2][2] + m2.m[2][2];

	return result;
}

///=====================================================///
///行列の減法
///=====================================================///
Matrix3x3 Subject(const Matrix3x3& m1, const Matrix3x3& m2) {

	Matrix3x3 result;

	result.m[0][0] = m1.m[0][0] - m2.m[0][0];
	result.m[0][1] = m1.m[0][1] - m2.m[0][1];
	result.m[0][2] = m1.m[0][2] - m2.m[0][2];
	result.m[1][0] = m1.m[1][0] - m2.m[1][0];
	result.m[1][1] = m1.m[1][1] - m2.m[1][1];
	result.m[1][2] = m1.m[1][2] - m2.m[1][2];
	result.m[2][0] = m1.m[2][0] - m2.m[2][0];
	result.m[2][1] = m1.m[2][1] - m2.m[2][1];
	result.m[2][2] = m1.m[2][2] - m2.m[2][2];

	return result;
}

///=====================================================///
///行列の積
///=====================================================///
Matrix3x3 Multiply(const Matrix3x3& m1, const Matrix3x3& m2) {

	Matrix3x3 answer = {};
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {

			answer.m[x][y] = 0;
			for (int z = 0; z < 3; ++z) {

				answer.m[x][y] += m1.m[x][z] * m2.m[z][y];
			}
		}
	}

	return answer;
}