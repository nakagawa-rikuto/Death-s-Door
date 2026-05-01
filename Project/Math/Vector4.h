#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 final {
	float x;
	float y;
	float z;
	float w;

	/// ===加法=== ///
	// Vector4 | const
	Vector4 operator+(const float& v);
	Vector4& operator+=(const float& v);
	Vector4 operator+(const Vector4& v);
	Vector4& operator+=(const Vector4& v);
	// const | const
	Vector4 operator+(const float& v)const;
	Vector4 operator+(const Vector4& v)const;

	/// ===減法=== ///
	// Vector4 | const
	Vector4 operator-(const float& v);
	Vector4& operator-=(const float& v);
	Vector4 operator-(const Vector4& v);
	Vector4& operator-=(const Vector4& v);

	// const | const
	Vector4 operator-(const float& v)const;
	Vector4 operator-(const Vector4& v)const;

	/// ===積=== ///
	// Vector4 | const
	Vector4 operator*(const float& v);
	Vector4& operator*=(const float& v);
	Vector4 operator*(const Vector4& v);
	Vector4& operator*=(const Vector4& v);
	// const | const
	Vector4 operator*(const float& v)const;
	Vector4 operator*(const Vector4& v)const;

	/// ===除法=== ///
	// Vector4 | const
	Vector4 operator/(const float& v);
	Vector4& operator/=(const float& v);
	Vector4 operator/(const Vector4& v);
	Vector4& operator/=(const Vector4& v);
	// const | const
	Vector4 operator/(const float& v)const;
	Vector4 operator/(const Vector4& v)const;
};

/// ===内積の計算=== ///
float Dot(const Vector4& q);
float Dot(const Vector4& q1, const Vector4& q2);

/// ===長さの計算=== ///
float Length(const Vector4& q);
float Length(const Vector4& q1, const Vector4& q2);

/// ===正規化=== ///
Vector4 Normalize(const Vector4& quaternion);