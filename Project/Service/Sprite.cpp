#include "Sprite.h"
#include "Engine/System/Managers/SpriteManager.h"
// CameraCommon
#include "Engine/Camera/Base/CameraCommon.h"
// Locator
#include "Locator.h"
#include "GraphicsResourceGetter.h"
#include "Camera.h"
// Math
#include <Math/MatrixMath.h>

namespace Service {
	///-------------------------------------------/// 
	/// スプライトの追加
	///-------------------------------------------///
	void Sprite::AddSprite(MiiEngine::SpriteCommon* sprite) {
		Locator::GetSpriteManager()->AddSprite(sprite);
	}

	///-------------------------------------------/// 
	/// スプライトのクリア
	///-------------------------------------------///
	void Sprite::ClearSprites() {
		Locator::GetSpriteManager()->ClearSprites();
	}

	///-------------------------------------------/// 
	///	スプライトの削除
	///-------------------------------------------///
	void Sprite::RemoveSprite(MiiEngine::SpriteCommon* sprite) {
		Locator::GetSpriteManager()->RemoveSprite(sprite);
	}

	///-------------------------------------------/// 
	/// 基準スケールの取得 
	///-------------------------------------------///
	Vector2 Sprite::GetBaseScale() {
		// 基準解像度
		constexpr float BASE_WIDTH = 1920.0f;
		constexpr float BASE_HEIGHT = 1080.0f;

		// ウィンドウサイズの取得
		Vector2 windowSize = {
			static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()),
			static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight())
		};
		// スケール計算
		Vector2 scale = { windowSize.x / BASE_WIDTH, windowSize.y / BASE_HEIGHT };
		return scale;
	}
}
