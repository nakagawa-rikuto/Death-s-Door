#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer ShatterParams : register(b0)
{
    float progress; // 割れる進行度 (0.0 ~ 1.0)
    float impactX; // 衝撃点のX座標
    float impactY; // 衝撃点のY座標
    float crackDensity; // ひび割れの密度
    float dispersion; // 破片の飛散度
    float rotation; // 破片の回転量
    float fadeOut; // フェードアウト効果
    float padding;
};

// ハッシュ関数（ノイズ生成用）
float hash(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.x + p3.y) * p3.z);
}

// 2Dノイズ関数
float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    
    float a = hash(i);
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    
    float2 u = f * f * (3.0 - 2.0 * f);
    
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// Voronoiノイズ（ひび割れパターン生成）
float2 voronoi(float2 p)
{
    float2 n = floor(p);
    float2 f = frac(p);
    
    float minDist = 8.0;
    float2 minPoint = float2(0.0, 0.0);
    
    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            float2 neighbor = float2(float(i), float(j));
            float2 pos = neighbor + float2(hash(n + neighbor), hash(n + neighbor + float2(10.5, 20.3)));
            
            float2 diff = pos - f;
            
            float dist = length(diff);
            
            if (dist < minDist)
            {
                minDist = dist;
                minPoint = n + neighbor + pos;
            }
        }
    }
    
    return float2(minDist, hash(minPoint));
}

// 2D回転行列
float2 rotate(float2 p, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float2(
        p.x * c - p.y * s,
        p.x * s + p.y * c
    );
}

float4 main(VertexShaderOutPut input) : SV_TARGET
{
    float2 uv = input.texcoord;
    float2 impactPoint = float2(impactX, impactY);
    
    // 衝撃点からの距離と方向
    float2 toCenter = uv - impactPoint;
    float dist = length(toCenter);
    float2 dir = normalize(toCenter + float2(0.0001, 0.0001)); // ゼロ除算回避
    
    // Voronoiノイズでひび割れパターンを生成
    float2 voronoiResult = voronoi(uv * crackDensity);
    float cellDist = voronoiResult.x;
    float cellId = voronoiResult.y;
    
    // 衝撃点からの距離に応じて進行度を調整（波紋のように広がる）
    float distanceDelay = dist * 0.4; // 距離による遅延
    float shardProgress = saturate((progress - distanceDelay) * 2.5);
    shardProgress = smoothstep(0.0, 1.0, shardProgress);
    
    // 破片ごとのランダムな動き
    float2 randomOffset = float2(
        hash(float2(cellId, 0.0)) - 0.5,
        hash(float2(cellId, 1.0)) - 0.5
    ) * 2.0;
    
    // 放射状の動き + ランダムな動き + 重力効果
    float gravity = shardProgress * shardProgress * 0.2; // 下方向への重力
    float2 displacement = (dir + randomOffset * 0.5) * shardProgress * dispersion * 0.4;
    displacement.y += gravity; // 重力を追加
    
    // 破片の回転（遠くの破片ほど回転が大きい）
    float rotationAmount = (hash(float2(cellId, 2.0)) - 0.5) * rotation * shardProgress * 3.14159;
    rotationAmount *= (1.0 + dist * 0.5); // 距離に応じて回転量を増加
    
    // 衝撃点を中心に回転
    float2 centeredUV = uv - impactPoint;
    centeredUV = rotate(centeredUV, rotationAmount);
    float2 rotatedUV = centeredUV + impactPoint;
    
    // 最終的なUV座標
    float2 finalUV = rotatedUV + displacement;
    
    // ひび割れのエッジ検出（進行度に応じて変化）
    float crackThreshold = lerp(0.02, 0.01, progress);
    float crackEdge = smoothstep(crackThreshold, crackThreshold + 0.02, cellDist);
    
    // テクスチャサンプリング
    float4 color = gTexture.Sample(gSampler, finalUV);
    
    // ひび割れの暗い線を追加（初期段階で目立つように）
    float crackLine = 1.0 - smoothstep(0.0, 0.03, cellDist);
    float crackVisibility = saturate(1.0 - shardProgress * 1.5);
    color.rgb = lerp(color.rgb, color.rgb * 0.2, crackLine * crackVisibility);
    
    // 破片のエッジを暗くして立体感を出す
    float edgeDarkness = lerp(1.0, 0.6, (1.0 - crackEdge) * shardProgress);
    color.rgb *= edgeDarkness;
    
    // ハイライト効果（ガラスの反射）
    float highlight = pow(1.0 - cellDist, 3.0) * 0.3 * (1.0 - shardProgress * 0.5);
    color.rgb += highlight;
    
    // UV範囲外は透明に
    if (finalUV.x < 0.0 || finalUV.x > 1.0 || finalUV.y < 0.0 || finalUV.y > 1.0)
    {
        color.a *= max(0.0, 1.0 - shardProgress * 2.0);
    }
    
    // フェードアウト効果（滑らかに）
    float fadeAmount = fadeOut * smoothstep(0.3, 1.0, shardProgress);
    color.a *= 1.0 - fadeAmount;
    
    // 破片が飛散した後は急速に透明に
    color.a *= 1.0 - smoothstep(0.75, 1.0, shardProgress);
    
    // アルファが0に近い場合は完全に透明に
    if (color.a < 0.01)
    {
        discard;
    }
    
    return color;
}