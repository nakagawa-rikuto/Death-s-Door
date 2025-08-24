#pragma once
// c++
#include <cstdint>
#include <array>

///-------------------------------------------/// 
/// BlendMode
///-------------------------------------------///
enum BlendMode {
	//!< ブレンドなし
	kBlendModeNone,

	//!< 通常ブレンド。
	KBlendModeNormal,

	//!< 加算
	kBlendModeAdd,

	//!< 減算
	kBlendModeSubtract,

	//!< 乗算
	kBlendModeMultily,

	//!< スクリーン
	kBlendModeScreen,

	// 総数(これは最後にしなければいけない)
	kCountOfBlendMode,
};

constexpr std::array<BlendMode, static_cast<size_t>(BlendMode::kCountOfBlendMode)> AllBlendModes() {
	return { BlendMode::kBlendModeNone, BlendMode::KBlendModeNormal, BlendMode::kBlendModeAdd,
			 BlendMode::kBlendModeSubtract, BlendMode::kBlendModeMultily, BlendMode::kBlendModeScreen };
}