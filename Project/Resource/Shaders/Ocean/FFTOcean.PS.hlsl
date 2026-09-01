
/// ================================================================
/// 定数バッファ
/// ================================================================
cbuffer OceanRenderCB : register(b1)
{
    float3 SunDirection;        // 太陽方向（正規化済み、光源→シーン方向の逆）
    float SunPower;             // 太陽強度
    float3 SunColor;            // 太陽色
    float FresnelBias;          // Fresnel 最小反射率

    float3 OceanDeepColor;      // 深海の色（暗い青）
    float OceanRoughness;       // 海面の粗さ（スペキュラの広がり）

    float3 OceanShallowColor;   // 浅瀬・波頂点の色（青緑）
    float SSSStrength;          // SSS 強度

    float3 FoamColor;           // 泡の色（白）
    float FoamSoftness;         // 泡のエッジ柔らかさ

    float3 SkyColorHorizon;     // 地平線の空の色
    float Pad0;

    float3 SkyColorZenith;      // 天頂の空の色
    float OceanOpacity;         // 海面の基本不透明度
}

/// ================================================================
/// テクスチャ
/// ================================================================
Texture2D<float4> FoamTexture : register(t3); // 泡ノイズテクスチャ
SamplerState LinearWrap : register(s0);

/// ================================================================
/// 入力（VS からの出力）
/// ================================================================
struct PSInput
{
    float4 SvPos : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 UV : TEXCOORD2;
    float Foam : TEXCOORD3;
    float3 ViewDir : TEXCOORD4;
};

/// ================================================================
/// ユーティリティ
/// ================================================================

/// Schlick の Fresnel 近似
float FresnelSchlick(float cosTheta, float F0)
{
    return F0 + (1.0 - F0) * pow(saturate(1.0 - cosTheta), 5.0);
}

/// GGX/Trowbridge-Reitz 法線分布
float GGX_NDF(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float d = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (3.14159265 * d * d + 1e-7);
}

/// スペキュラ用幾何学減衰
float SchlickGGX(float NdotV, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k + 1e-7);
}

/// 簡易スカイカラー
float3 GetSkyColor(float3 reflectDir)
{
    float t = saturate(reflectDir.y * 0.5 + 0.5);
    return lerp(SkyColorHorizon, SkyColorZenith, t * t);
}

/// ================================================================
/// メイン
/// ================================================================
float4 main(PSInput input) : SV_TARGET
{
    float3 N = normalize(input.Normal);
    float3 V = normalize(input.ViewDir);
    float3 L = normalize(SunDirection);
    float3 H = normalize(V + L); // ハーフベクトル

    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));

    // --------------------------------------------------------
    //    Fresnel 反射
    //    水の屈折率 ~1.33 → F0 = ((1.33-1)/(1.33+1))^2 ≈ 0.02
    // --------------------------------------------------------
    float F0 = max(FresnelBias, 0.02);
    float fresnel = FresnelSchlick(NdotV, F0);

    // 環境マップ（スカイボックス相当をシンプルに近似）
    float3 reflectDir = reflect(-V, N);
    float3 reflectionColor = GetSkyColor(reflectDir);

    // 屈折色（水の色）
    float3 refractionColor = lerp(OceanDeepColor, OceanShallowColor, fresnel);

    // 反射と屈折を Fresnel で合成
    float3 baseColor = lerp(refractionColor, reflectionColor, fresnel);

    // --------------------------------------------------------
    //    スペキュラ
    //    サン・グリント：複雑な波面で太陽が無数にキラキラ反射する
    // --------------------------------------------------------
    float D = GGX_NDF(NdotH, OceanRoughness);
    float G = SchlickGGX(NdotV, OceanRoughness)
            * SchlickGGX(NdotL, OceanRoughness);
    float F = FresnelSchlick(VdotH, F0);

    // Cook-Torrance BRDF
    float denom = 4.0 * NdotV * NdotL + 1e-7;
    float3 specular = (D * G * F / denom) * SunColor * SunPower * NdotL;

    // --------------------------------------------------------
    //    SSS
    //    波の頂点が逆光で透き通る青緑の輝き
    // --------------------------------------------------------
    // 視線が太陽の逆方向を見ていて、かつ波が高い部分に適用
    float sssView = saturate(dot(-V, L)); // 逆光方向への傾き
    float sssHeight = saturate(input.WorldPos.y * 0.3 + 0.5); // 高さで強くなる
    float sssFactor = pow(sssView, 3.0) * sssHeight * SSSStrength;
    float3 sssColor = OceanShallowColor * sssFactor * SunColor;

    // --------------------------------------------------------
    //    泡
    //    Jacobian ベースの泡強度に泡テクスチャを乗算してマスク
    // --------------------------------------------------------
    // 泡ノイズテクスチャをスクロールしてリアルに見せる
    float2 foamUV1 = input.UV * 4.0;
    float2 foamUV2 = input.UV * 2.5 + float2(0.1, 0.05); // 少しずらして2枚合わせ

    float foamNoise = FoamTexture.Sample(LinearWrap, foamUV1).r
                    * FoamTexture.Sample(LinearWrap, foamUV2).g;

    // Jacobian 泡強度にノイズを掛けてマスク
    float foamMask = saturate(input.Foam / max(FoamSoftness, 0.01));
    float foamIntensity = foamMask * foamNoise;

    // --------------------------------------------------------
    //    全要素を合成
    // --------------------------------------------------------
    float3 finalColor = baseColor + specular + sssColor;

    // 泡を上乗せ（白い泡色で上書き）
    finalColor = lerp(finalColor, FoamColor, foamIntensity);

    // 最低限のアンビエント（真っ暗になるのを防ぐ）
    finalColor += OceanDeepColor * 0.03;

    // --------------------------------------------------------
    // 透明度
    // --------------------------------------------------------
    float alpha = saturate(OceanOpacity + fresnel * (1.0 - OceanOpacity));
    alpha = max(alpha, foamIntensity);

    return float4(finalColor, alpha);
}
