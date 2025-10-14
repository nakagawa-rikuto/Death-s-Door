#pragma once
#include "Engine/Graphics/OffScreen/Effect/ShatterGlassEffect.h"

enum class FadeState {
    None,
    FadeIn,   // 真っ暗→通常
    FadeOut,  // 通常→真っ暗
    Finished
};

///=====================================================/// 
/// SceneTransition
///=====================================================///
class SceneTransition {
public: 

    SceneTransition() = default;
    ~SceneTransition() = default;

    // フェード開始
    void StartFadeIn(float duration = 1.2f);   // 真っ暗→通常
    void StartFadeOut(float duration = 1.0f);  // 通常→真っ暗

    // 更新
    void Update();

    // フェード中か
    bool IsFading() const;

    // 完了したか
    bool IsFinished() const;

    // 現在の状態
    FadeState GetState() const;

private: 

    // Data
    ShatterGlassData data_;

    // アニメーション制御
    bool isPlaying_ = false;
    bool isFinished_ = false;
    float currentTime_ = 0.0f;
    float duration_ = 1.2f; // エフェクトの総時間（秒）

    FadeState currentState_ = FadeState::None;

    // イージング関数
    float EaseOutCubic(float t);
    float EaseInOutQuad(float t);
};

