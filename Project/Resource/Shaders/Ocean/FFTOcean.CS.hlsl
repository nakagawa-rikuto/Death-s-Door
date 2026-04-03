/// ================================================================
/// FFT Ocean - Compute Shader
/// ================================================================

/// ================================================================
/// 定数バッファ
/// ================================================================
cbuffer OceanParams : register(b0)
{
    uint N;              // グリッドサイズ（例: 256）
    float L;             // タイル実空間サイズ（例: 1000.0）
    float Time;          // 経過時間
    float WindSpeed;     // 風速 [m/s]
    float2 WindDir;      // 風向き（正規化済み）
    float Amplitude;     // Phillips 振幅係数 A
    float Lambda;        // Choppy Wave 強度（0.0 〜 1.5）
    float FoamThreshold; // 泡判定しきい値（0.0 〜 1.0）
    float3 Padding;
}

cbuffer ButterflyParams : register(b1)
{
    uint Stage;         // バタフライステージ（0 〜 log2(N)-1）
    uint PingPong;      // 0=Ping→Pong, 1=Pong→Ping
    uint Direction;     // 0=水平方向, 1=垂直方向
    uint InputSource;   // 0=Hkt, 1=Dx, 2=Dz
    uint BfPad;
}

/// ================================================================
/// テクスチャ（UAV）
/// ================================================================
RWTexture2D<float4> H0Texture : register(u0);       // 初期スペクトル
RWTexture2D<float4> HktTexture : register(u1);      // 時間更新スペクトル（高さ）
RWTexture2D<float4> DxTexture : register(u2);       // 水平変位スペクトル X
RWTexture2D<float4> DzTexture : register(u3);       // 水平変位スペクトル Z
RWTexture2D<float4> PingTexture : register(u4);     // IFFT Ping バッファ
RWTexture2D<float4> PongTexture : register(u5);     // IFFT Pong バッファ
RWTexture2D<float4> DisplaceMap : register(u6);     // 最終変位マップ (Dx, height, Dz)
RWTexture2D<float4> NormalFoamMap : register(u7);   // 法線 (rgb) + 泡強度 (a)

/// ================================================================
/// 複素数演算ユーティリティ
/// ================================================================

/// 複素数の乗算: (a + bi)(c + di) = (ac - bd) + (ad + bc)i
float2 ComplexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y,
                  a.x * b.y + a.y * b.x);
}

/// 複素共役
float2 ComplexConj(float2 a)
{
    return float2(a.x, -a.y);
}

/// オイラーの公式: e^(iθ) = cos(θ) + i*sin(θ)
float2 EulerExp(float theta)
{
    return float2(cos(theta), sin(theta));
}

/// ================================================================
/// Box-Muller 法（正規分布乱数）
/// ================================================================
float2 BoxMuller(float u1, float u2)
{
    float r = sqrt(-2.0 * log(max(u1, 1e-7)));
    float theta = 6.28318530 * u2;
    return float2(r * cos(theta), r * sin(theta));
}

/// 簡易ハッシュ（シード付き擬似乱数 → [0,1]）
float Hash(uint seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9301u;
    seed ^= seed >> 4u;
    seed *= 0x27d4eb2du;
    seed ^= seed >> 15u;
    return float(seed) / 4294967295.0;
}

/// ================================================================
/// Phillips スペクトル
///   P(k) = A * exp(-1/(kL)^2) / k^4 * (k・w)^2
/// ================================================================
float PhillipsSpectrum(float2 k)
{
    float kLen = length(k);
    if (kLen < 1e-5)
        return 0.0;

    float kLen2 = kLen * kLen;
    float kLen4 = kLen2 * kLen2;

    float g = 9.81;
    float Lk = (WindSpeed * WindSpeed) / g; // 最大波長 L = V^2/g
    float Lk2 = Lk * Lk;

    float phillip = Amplitude
                  * exp(-1.0 / (kLen2 * Lk2))
                  / kLen4;

    // 風向きとの一致度（内積の2乗）
    float2 kNorm = k / kLen;
    float kDotW = dot(kNorm, WindDir);
    phillip *= kDotW * kDotW;

    // 風と逆向きの微細な波を抑制
    phillip *= exp(-kLen2 * 0.0001);

    return max(phillip, 0.0);
}

/// ================================================================
/// Kernel 0: InitSpectrum
///   初期スペクトル h0(k) / h0*(-k) を H0Texture に書き込む
///   ※ 一度だけ実行する
/// ================================================================
[numthreads(16, 16, 1)]
void InitSpectrum(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    float pi2 = 6.28318530;

    float2 k;
    k.x = (id.x - (int) N / 2) * (pi2 / L);
    k.y = (id.y - (int) N / 2) * (pi2 / L);

    float p = sqrt(max(PhillipsSpectrum(k), 0.0) * 0.5);
    float pm = sqrt(max(PhillipsSpectrum(-k), 0.0) * 0.5);

    uint seed = (uint) id.x + (uint) id.y * N;
    float2 g1 = BoxMuller(Hash(seed + 0u), Hash(seed + 1u));
    float2 g2 = BoxMuller(Hash(seed + 2u), Hash(seed + 3u));

    H0Texture[id] = float4(g1 * p, g2 * pm);
}

/// ================================================================
/// Kernel 1: UpdateSpectrum
///   時刻 t の h(k,t)・Dx・Dz スペクトルを計算
/// ================================================================
[numthreads(16, 16, 1)]
void UpdateSpectrum(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    float pi2 = 6.28318530;

    float2 k;
    k.x = (id.x - (int) N / 2) * (pi2 / L);
    k.y = (id.y - (int) N / 2) * (pi2 / L);

    float kLen = length(k);
    float g = 9.81;
    float omega = sqrt(g * max(kLen, 1e-5)); // 分散関係

    float4 h0Data = H0Texture[id];
    float2 h0 = h0Data.xy;
    float2 h0c = h0Data.zw;

    // h(k,t) = h0(k)*e^(iωt) + conj(h0(-k))*e^(-iωt)
    float2 hkt = ComplexMul(h0, EulerExp(omega * Time))
               + ComplexMul(ComplexConj(h0c), EulerExp(-omega * Time));

    HktTexture[id] = float4(hkt, 0, 0);

    // Choppy Wave 変位スペクトル: D(k) = -i*(k/|k|)*h(k,t)
    float2 kNorm = (kLen > 1e-5) ? (k / kLen) : (float2) 0;
    float2 minusI = float2(0, -1);

    DxTexture[id] = float4(ComplexMul(minusI * kNorm.x, hkt), 0, 0);
    DzTexture[id] = float4(ComplexMul(minusI * kNorm.y, hkt), 0, 0);
}

/// ================================================================
/// Kernel 2: BitReverseRows（IFFT 前処理 - 行方向）
/// ================================================================
uint BitReverse(uint v, uint bits)
{
    uint r = 0;
    [unroll(8)]
    for (uint i = 0; i < bits; i++)
    {
        r = (r << 1u) | (v & 1u);
        v >>= 1u;
    }
    return r;
}

[numthreads(16, 16, 1)]
void BitReverseRows(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    uint bits = (uint) round(log2((float) N));
    uint revX = BitReverse((uint) id.x, bits);
    
    float4 src;
    if (InputSource == 0)
        src = HktTexture[int2(revX, id.y)]; // Height
    else if (InputSource == 1)
        src = DxTexture[int2(revX, id.y)]; // Dx
    else
        src = DzTexture[int2(revX, id.y)]; // Dz

    PingTexture[id] = src;
}

/// ================================================================
/// Kernel 3: ButterflyIFFT
///   1スレッド = 1ピクセル。Stage・Direction・PingPong を外から指定
/// ================================================================
[numthreads(16, 16, 1)]
void ButterflyIFFT(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;

    uint span = 1u << Stage;
    uint stride = span << 1u;

    uint idx = (Direction == 0) ? (uint) id.x : (uint) id.y;
    uint groupIdx = idx / stride;
    uint withinGroup = idx % stride;

    uint topIdx = groupIdx * stride + (withinGroup % span);
    uint botIdx = topIdx + span;

    // Twiddle Factor（IFFT は符号が +）
    float angle = 6.28318530 * float(withinGroup % span) / float(stride);
    float2 twiddle = EulerExp(angle);

    int2 topCoord = (Direction == 0) ? int2(topIdx, id.y) : int2(id.x, topIdx);
    int2 botCoord = (Direction == 0) ? int2(botIdx, id.y) : int2(id.x, botIdx);

    float2 topVal, botVal;
    if (PingPong == 0)
    {
        topVal = PingTexture[topCoord].xy;
        botVal = PingTexture[botCoord].xy;
    }
    else
    {
        topVal = PongTexture[topCoord].xy;
        botVal = PongTexture[botCoord].xy;
    }

    float2 twBot = ComplexMul(twiddle, botVal);
    float2 resultT = topVal + twBot;
    float2 resultB = topVal - twBot;

    if (PingPong == 0)
    {
        PongTexture[topCoord] = float4(resultT, 0, 0);
        PongTexture[botCoord] = float4(resultB, 0, 0);
    }
    else
    {
        PingTexture[topCoord] = float4(resultT, 0, 0);
        PingTexture[botCoord] = float4(resultB, 0, 0);
    }
}

/// ================================================================
/// Kernel 4: BitReverseCols（IFFT 前処理 - 列方向）
/// ================================================================
[numthreads(16, 16, 1)]
void BitReverseCols(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    uint bits = (uint) round(log2((float) N));
    uint revY = BitReverse((uint) id.y, bits);
    PingTexture[id] = PongTexture[int2(id.x, revY)];
}

/// ================================================================
/// Kernel 5: AssembleDisplacement
///   height / Dx / Dz の IFFT 結果を DisplaceMap に合成
///   ※ 3 回分の IFFT をそれぞれ別に PingPong し、最後にこの Kernel を呼ぶ
/// ================================================================
Texture2D<float4> HeightIFFT : register(t0);
Texture2D<float4> DxIFFT : register(t1);
Texture2D<float4> DzIFFT : register(t2);
Texture2D<float2> RippleMap : register(t3);

[numthreads(16, 16, 1)]
void AssembleDisplacement(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    float scale = 1.0 / float(N * N);

    // 符号反転（周波数シフトの逆操作）
    float sign = ((id.x + id.y) & 1) ? -1.0 : 1.0;

    float height = HeightIFFT[id].x * scale * sign;
    float dx = DxIFFT[id].x * scale * sign * Lambda;
    float dz = DzIFFT[id].x * scale * sign * Lambda;
    
    // 波紋の高さを加算
    float ripple = RippleMap[id].x;
    height += ripple;

    DisplaceMap[id] = float4(dx, height, dz, 1.0);
}

/// ================================================================
/// Kernel 5: SaveIFFTResult
///   垂直IFFT完了後、PingTexture の結果を
///   InputSource に応じて Dx / Dz に書き戻す
/// ================================================================
[numthreads(16, 16, 1)]
void SaveIFFTResult(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    float4 result = PingTexture[id];

    if (InputSource == 1)
        DxTexture[id] = result; // Dx IFFT結果を保存
    else if (InputSource == 2)
        DzTexture[id] = result; // Dz IFFT結果を保存
}

/// ================================================================
/// Kernel 6: ComputeNormalFoam
///   中心差分で法線計算 + Jacobian 行列式で泡強度を算出
/// ================================================================
[numthreads(16, 16, 1)]
void ComputeNormalFoam(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    uint uN = N;
    float dx = L / float(N); // 実空間上のピクセル間隔

    // トーラス折り返しで隣接ピクセルを取得
    int2 left = int2((id.x - 1 + uN) % uN, id.y);
    int2 right = int2((id.x + 1) % uN, id.y);
    int2 down = int2(id.x, (id.y - 1 + uN) % uN);
    int2 up = int2(id.x, (id.y + 1) % uN);

    float3 dL = DisplaceMap[left].xyz;
    float3 dR = DisplaceMap[right].xyz;
    float3 dD = DisplaceMap[down].xyz;
    float3 dU = DisplaceMap[up].xyz;

    // 実空間座標（変位を加味）
    float3 posL = float3(-dx + dL.x, dL.y, dL.z);
    float3 posR = float3(dx + dR.x, dR.y, dR.z);
    float3 posD = float3(dD.x, dD.y, -dx + dD.z);
    float3 posU = float3(dU.x, dU.y, dx + dU.z);

    float3 tangentX = posR - posL;
    float3 tangentZ = posU - posD;
    float3 normal = normalize(cross(tangentZ, tangentX));

    // --- Jacobian 行列式（泡判定） ---
    // J = (1 + λ*∂Dx/∂x)(1 + λ*∂Dz/∂z) - (λ*∂Dx/∂z)(λ*∂Dz/∂x)
    float invD2 = 1.0 / (2.0 * dx);
    float dDxdx = (dR.x - dL.x) * invD2;
    float dDzdz = (dU.z - dD.z) * invD2;
    float dDxdz = (dU.x - dD.x) * invD2;
    float dDzdx = (dR.z - dL.z) * invD2;

    float J = (1.0 + Lambda * dDxdx) * (1.0 + Lambda * dDzdz)
               - (Lambda * dDxdz) * (Lambda * dDzdx);

    // J が小さい（頂点が密集）ほど泡が強い
    float foam = saturate(FoamThreshold - J);

    // 法線を [0,1] に変換して格納、a チャンネルに泡強度
    NormalFoamMap[id] = float4(normal * 0.5 + 0.5, foam);
}
