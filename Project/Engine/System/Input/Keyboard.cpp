#include "Keyboard.h"
// Engine
#include "Engine/Core/WinApp.h"
#include "InputCommon.h"
// c++
#include <cassert>

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	Keyboard::~Keyboard() {
		if (keyboard_) {
			keyboard_->Unacquire();
		}
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void Keyboard::Initialize(WinApp* winApp, IDirectInput8* input) {
		HRESULT hr;

		// キーボードデバイスの生成
		hr = input->CreateDevice(GUID_SysKeyboard, &keyboard_, nullptr);
		assert(SUCCEEDED(hr));
		// 入力データ形式のセット
		hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(hr));
		// 排他的制御レベルのセット
		hr = keyboard_->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void Keyboard::Update() {
		HRESULT hr;

		// 前回の入力を保持
		memcpy(preKey_, key_, sizeof(key_));

		// 情報の取得を開始
		hr = keyboard_->Acquire();
		if (SUCCEEDED(hr)) {
			keyboard_->GetDeviceState(sizeof(key_), key_);
		}
	}

	///-------------------------------------------/// 
	/// キーの押下をチェック
	///-------------------------------------------///
	bool Keyboard::PushKey(BYTE keyNum) {
		// 指定キーを押して入ればTrueを返す
		if (key_[keyNum]) {
			return true;
		}
		// そうでなければFalseを返す
		return false;
	}

	///-------------------------------------------/// 
	/// キーのトリガーをチェック
	///-------------------------------------------///
	bool Keyboard::TriggerKey(BYTE keyNum) {
		// 指定キーがトリガーされていればtrueを返す
		if (key_[keyNum] && !preKey_[keyNum]) {
			return true;
		}
		// そうでなければFalseを返す
		return false;
	}

	///-------------------------------------------/// 
	/// キーのリリースをチェック
	///-------------------------------------------///
	bool Keyboard::ReleaseKey(BYTE keyNum)	{
		// 指定キーがリリースされていればtrueを返す
		if (!key_[keyNum] && preKey_[keyNum]) {
			return true;
		}
		// そうでなければFalseを返す
		return false;
	}

	///-------------------------------------------/// 
	/// いずれかのキー入力があるかどうかをチェック
	///-------------------------------------------///
	bool Keyboard::HasAnyKeyInput() const {
		for (int i = 0; i < 256; i++) {
			if (key_[i]) {
				return true;
			}
		}
		return false;
	}

	///-------------------------------------------/// 
	/// WASDキーの入力値を取得 
	///-------------------------------------------///
	Vector2 Keyboard::GetKeybordWASDValue() {
		Vector2 input = { 0.0f, 0.0f };

		if (PushKey(DIK_D)) {
			input.x += 1.0f;
		}
		if (PushKey(DIK_A)) {
			input.x -= 1.0f;
		}
		if (PushKey(DIK_W)) {
			input.y += 1.0f;
		}
		if (PushKey(DIK_S)) {
			input.y -= 1.0f;
		}

		// 斜め移動時に速度が大きくなりすぎないよう正規化
		if (Length(Vector2{ input.x, input.y }) > 1.0f) {
			Vector2 normalized = Normalize(Vector2{ input.x, input.y });
			input.x = normalized.x;
			input.y = normalized.y;
		}

		return input;
	}

	///-------------------------------------------/// 
	/// 矢印キーの入力値を取得
	///-------------------------------------------///
	Vector2 Keyboard::GetKeybordArrowValue() {
		Vector2 input = { 0.0f, 0.0f };

		if (PushKey(DIK_RIGHT)) {
			input.x += 1.0f;
		}
		if (PushKey(DIK_LEFT)) {
			input.x -= 1.0f;
		}
		if (PushKey(DIK_UP)) {
			input.y += 1.0f;
		}
		if (PushKey(DIK_DOWN)) {
			input.y -= 1.0f;
		}

		// 斜め移動時に速度が大きくなりすぎないよう正規化
		if (Length(Vector2{ input.x, input.y }) > 1.0f) {
			Vector2 normalized = Normalize(Vector2{ input.x, input.y });
			input.x = normalized.x;
			input.y = normalized.y;
		}

		return input;
	}
}
