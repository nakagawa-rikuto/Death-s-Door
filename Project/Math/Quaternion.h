#pragma once
/// <summary>
/// Quaternionの構造体
/// </summary>
struct Quaternion final {
	float x;
	float y;
	float z;
	float w;

	/// ===加法=== ///
	// Quaternion | const
	Quaternion operator+(const float& v);
	Quaternion& operator+=(const float& v);
	Quaternion operator+(const Quaternion& v);
	Quaternion& operator+=(const Quaternion& v);
	// const | const
	Quaternion operator+(const float& v)const;
	Quaternion operator+(const Quaternion& v)const;

	/// ===減法=== ///
	// Quaternion | const
	Quaternion operator-(const float& v);
	Quaternion& operator-=(const float& v);
	Quaternion operator-(const Quaternion& v);
	Quaternion& operator-=(const Quaternion& v);

	// const | const
	Quaternion operator-(const float& v)const;
	Quaternion operator-(const Quaternion& v)const;

	/// ===積=== ///
	// Quaternion | const
	Quaternion operator*(const float& v);
	Quaternion& operator*=(const float& v);
	Quaternion operator*(const Quaternion& v);
	Quaternion& operator*=(const Quaternion& v);
	// const | const
	Quaternion operator*(const float& v)const;
	Quaternion operator*(const Quaternion& v)const;

	/// ===除法=== ///
	// Quaternion | const
	Quaternion operator/(const float& v);
	Quaternion& operator/=(const float& v);
	Quaternion operator/(const Quaternion& v);
	Quaternion& operator/=(const Quaternion& v);
	// const | const
	Quaternion operator/(const float& v)const;
	Quaternion operator/(const Quaternion& v)const;
};

/// ===内積の計算=== ///
float Dot(const Quaternion& q);
float Dot(const Quaternion& q1, const Quaternion& q2);

/// ===長さの計算=== ///
float Length(const Quaternion& q);
float Length(const Quaternion& q1, const Quaternion& q2);

/// ===Quaternionの積=== ///
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

/// ===正規化したQuaternionを返す=== ///
Quaternion Normalize(const Quaternion& quaternion);