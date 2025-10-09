#pragma once
#include "Engine/DataInfo/CData.h"

/// ===Particleの構造体=== ///
struct ParticleData {
	EulerTransform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};
/// ===PaticleForGPU=== ///
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};
