#include "EasingMath.h"
#define _USE_MATH_DEFINES
#include <cmath>

#include "sMath.h"

///-------------------------------------------/// 
/// Lerp関数
///-------------------------------------------///
// float
float Math::Lerp(float start, float end, float t) {
    return start * (1.0f - t) + end * t;
}
// Vector3
Vector3 Math::Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;

	result.x = (1.0f - t) * start.x + t * end.x;
	result.y = (1.0f - t) * start.y + t * end.y;
	result.z = (1.0f - t) * start.z + t * end.z;

	return result;
}
// Quaternion
Quaternion Math::Lerp(const Quaternion& start, const Quaternion& end, float t) {
	Quaternion result;

	result.x = (1.0f - t) * start.x + t * end.x;
	result.y = (1.0f - t) * start.y + t * end.y;
	result.z = (1.0f - t) * start.z + t * end.z;
	result.w = (1.0f - t) * start.w + t * end.w;

	return result;
}

///-------------------------------------------/// 
/// SLerp関数
///-------------------------------------------///
// Vector3
Vector3 Math::SLerp(const Vector3& start, const Vector3& end, float t) {
	// なす角の計算
	float angle = std::cosf(Dot(start, end));

	// 線形補間を計算する
	float scalestart = std::sinf((1.0f - t) * angle) / std::sinf(angle);

	float scaleend = std::sinf(t * angle) / std::sinf(angle);

	Vector3 result;

	result.x = scalestart * start.x + scaleend * end.x;
	result.y = scalestart * start.y + scaleend * end.y;
	result.z = scalestart * start.z + scaleend * end.z;

	return result;
}
// Quaternion
Quaternion Math::SLerp(const Quaternion& start, const Quaternion& end, float t) {
	Quaternion q2Modified = end;
	float dot = Dot(start, end);

	// 逆方向補間を防ぐために符号を反転
	if (dot < 0.0f) {
		q2Modified = Conjugate(q2Modified);
		dot = -dot;
	}

	// クォータニオン補間
	if (dot > 0.9995f) {
		// 角度が小さい場合は Lerp で近似
		return Normalize(start + (q2Modified - start) * t);
	}

	float theta_0 = acosf(dot); // 初期角度
	float theta = theta_0 * t;  // 補間後の角度

	Quaternion q3 = Normalize(q2Modified - start * dot); // 直交成分
	return start * cosf(theta) + q3 * sinf(theta);
}
