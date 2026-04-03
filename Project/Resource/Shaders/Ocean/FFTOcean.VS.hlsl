
/// ================================================================
/// 定数バッファ
/// ================================================================
cbuffer TransformCB : register(b0)
{
    matrix World;       // ワールド行列
    matrix View;        // ビュー行列
    matrix Projection;  // プロジェクション行列
    float3 CameraPos;   // カメラ世界座標
    float OceanSize;    // タイル実空間サイズ（CS の L と一致）
    float TileScale;    // テクスチャタイリング倍率
    float3 CbPad;
}

/// ================================================================
/// テクスチャ
/// ================================================================
Texture2D<float4> DisplaceMap : register(t0);   // (Dx, height, Dz, 1)
Texture2D<float4> NormalFoamMap : register(t1); // (nx, ny, nz packed, foam)
Texture2D<float2> RippleMap : register(t2); // (height, 0)
SamplerState LinearWrap : register(s0);
SamplerState LinearClamp : register(s1);

/// ================================================================
/// 入出力構造体
/// ================================================================
struct VSInput
{
    float3 Position : POSITION;     // グリッドの XZ 平面上の頂点（Y=0）
    float2 TexCoord : TEXCOORD0;    // グリッド UV [0,1]
};

struct VSOutput
{
    float4 SvPos : SV_POSITION;     // クリップ空間座標
    float3 WorldPos : TEXCOORD0;    // 変位後の世界座標
    float3 Normal : TEXCOORD1;      // 世界空間法線
    float2 UV : TEXCOORD2;          // テクスチャ UV
    float Foam : TEXCOORD3;         // 泡強度
    float3 ViewDir : TEXCOORD4;     // 視線ベクトル（頂点 → カメラ）
};

/// ================================================================
/// メイン
/// ================================================================
VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    // タイリング UV（テクスチャをリピートして広大な海を表現）
    float2 uv = input.TexCoord * TileScale;

    // Displacement Map をサンプリング（Vertex Shader でのサンプリング）
    float4 displace = DisplaceMap.SampleLevel(LinearWrap, uv, 0);
    float4 normalFoam = NormalFoamMap.SampleLevel(LinearWrap, uv, 0);

    // 変位を適用（dx=水平X, height=垂直Y, dz=水平Z）
    float3 displaced = input.Position;
    displaced.x += displace.x;
    displaced.y += displace.y;
    displaced.z += displace.z;

    // 世界空間へ変換
    float4 worldPos = mul(float4(displaced, 1.0), World);
    output.WorldPos = worldPos.xyz;

    // ビュー・プロジェクション変換
    float4 viewPos = mul(worldPos, View);
    output.SvPos = mul(viewPos, Projection);

    // 法線をアンパック（[0,1] → [-1,1]）
    float3 normal = normalFoam.rgb * 2.0 - 1.0;
    // ワールド空間法線（法線変換は World の逆転置だが、スケールなし前提でシンプルに）
    output.Normal = normalize(mul(normal, (float3x3) World));

    // 泡強度
    output.Foam = normalFoam.a;

    // テクスチャ UV（タイリングなし版もPSに渡す）
    output.UV = input.TexCoord;

    // 視線ベクトル
    output.ViewDir = normalize(CameraPos - output.WorldPos);

    return output;
}
