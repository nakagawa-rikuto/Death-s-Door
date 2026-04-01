#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace MiiEngine {
    ///=====================================================/// 
    /// Ocean Shader用データ構造
    ///=====================================================///

    const int kWaveCount_ = 3; // 使用する波の数

    /// <summary>
    /// 波情報構造体（ゲルストナー波用）
    /// </summary>
    struct OceanShaderInfo {
        Vector3 distance;   // 波の方向ベクトル
        float amplitude;    // 波の振幅
        float length;       // 波の波長
        float speed;        // 波の速度
        float time;         // 時間
        float padding;      // アライメント用（16バイト境界）
    };

    /// <summary>
    /// 波紋情報構造体
    /// </summary>
    struct RippleInfo {
        Vector2 position;   // 波紋の中心位置(XZ平面)
        float startTime;    // 波紋の開始時間
        float intensity;    // 波紋の強度
        float duration;     // 波紋の持続時間
        float maxRadius;    // 波紋の最大半径
        float speed;        // 波紋の拡散速度
        float padding;      // アライメント用
    };

    /// <summary>
    /// 波紋データ（CPU側管理用）
    /// </summary>
    struct RippleData {
        Vector2 position;
        float startTime;
        float intensity;
        float duration;
        float maxRadius;
        float speed;
        int priority;       // 優先度（新しいほど高い）
        bool isActive;
    };

    /// <summary>
    /// 波紋バッファ（GPU送信用）
    /// </summary>
    struct RippleBufferForGPU {
        RippleInfo ripples[8];  // 最大8つの波紋
        int activeCount;        // アクティブな波紋の数
        float currentTime;      // 現在時刻
        float rippleSpeed;      // デフォルトの拡散速度
        float rippleDecay;      // デフォルトの減衰率
    };

    /// <summary>
    /// 海の色情報
    /// </summary>
    struct OceanColorInfo {
        Vector3 seaBase;        // 基本の海の色
        float padding1;
        Vector3 seaShallow;     // 浅瀬の色
        float padding2;
        Vector3 sky;            // 空の色
        float padding3;
        Vector3 deepWater;      // 深海の色
        float baseStrength;     // 色の強度
        float heightOffset;     // 波の高さに基づいた色のオフセット
        float waterClarity;     // 水の透明度
        float foamThreshold;    // 泡の閾値
    };

    /// <summary>
    /// Compute Shader出力用頂点データ
    /// </summary>
    struct WaveVertexData {
        Vector3 position;   // 波の影響を受けた位置
        Vector3 normal;     // 計算された法線
        Vector2 worldXZ;    // ワールド座標のXZ
        float padding;      // アライメント用
    };
}


namespace MiiEngine {
    ///=====================================================/// 
	/// CS用の定数バッファ構造体
    ///=====================================================///
    
    ///-------------------------------------------/// 
    /// OceanParams
    ///-------------------------------------------///
    struct OceanParams {
		uint32_t gridSize;       // グリッドのサイズ
		float gridWidth;         // グリッドの幅
		float time;              // 経過時間
		float windowSpeed;       // ウィンドウの速度
		Vector2 windDirection;   // 風の方向
		float amplitude;         // 波の振幅
		float lambda;            // 波長
		float foamThreshold;     // 泡の閾値
        Vector3 padding;
    };

    ///-------------------------------------------/// 
    /// ButterflyParams
    ///-------------------------------------------///
    struct ButterflyParams {
		uint32_t stages;         // バタフライアルゴリズムのステージ数
		uint32_t pingPong;       // ピンポンバッファの切り替えフラグ
		uint32_t direction;      // バタフライアルゴリズムの方向（0: 水平, 1: 垂直）
		uint32_t inputSource;    // インプットソース（0: 初期スペクトル, 1: 前のステージの出力）
        uint32_t bfPad;
    };

    ///=====================================================/// 
    /// PS用定数バッファ構造体
    ///=====================================================///
    
    struct OceanRenderCB {
        Vector3 sunDirection; // 太陽の方向
        float sunPower; // 太陽の強度
        Vector3 sunColor; // 太陽の色
        float fresnelBias; // Fresnelの最小反射率
        Vector3 deepColor; // 深海の色
        float roughness; // 海面の粗さ
        Vector3 shallowColor; // 浅瀬・波頂点の色
        float sssStrength; // SSS強度
        Vector3 foamColor; // 泡の色
        float foamSoftness; // 泡のエッジの柔らかさ
        Vector3 skyColorHorizon; // 地平線の空の色
        float pad0;
        Vector3 skyColorZenith; // 天頂の空の色
        float pad1;
    };

    ///=====================================================/// 
    /// VS用定数バッファ構造体
    ///=====================================================///
    
    struct TransformCB {
        Matrix4x4 world;
        Matrix4x4 view;
        Matrix4x4 projection;
        Vector3 cameraPos;
        float size;
        float tileScale;
        Vector3 cbPad;
    };

    ///-------------------------------------------/// 
    /// OceanGridVertex
    ///-------------------------------------------///
    struct OceanGridVertex {
        Vector3 position; // POSITION (x, 0, z)
        Vector2 texCoord; // TEXCOORD0
    };

    ///-------------------------------------------/// 
	/// RippleVertex
    ///-------------------------------------------///
    struct RippleParams {
		uint32_t gridSize; // グリッドのサイズ
        float cellSize;    // セルサイズ
        float waveSpeed;   // 速度
        float deltaTime;   // デルタタイム
        float damping;     // 減衰率
		uint32_t pingPong;    // ピンポンバッファの切り替えフラグ
		float padding[2];
    };

    ///-------------------------------------------///   
    /// RippleInjection
    ///-------------------------------------------///
    struct RippleInjection {
        Vector2 uv;     // 位置
        float radius;   // 半径
        float strength; // 強さ
    };
}