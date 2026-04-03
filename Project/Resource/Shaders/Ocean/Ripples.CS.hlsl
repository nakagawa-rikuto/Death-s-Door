/// ================================================================
/// Ripple Simulator - Compute Shader
/// 波動方程式: ∂²h/∂t² = c² * ∇²h
/// Ping-Pong バッファで前フレーム・現フレームを交互に使用
/// ================================================================

// Rippleシミュレーションのパラメータ
cbuffer RippleParams : register(b0)
{
    uint GridSize; // グリッドサイズ（OceanのNと一致）
    float CellSize; // 実空間上のセルサイズ (gridWidth / GridSize)
    float WaveSpeed; // 波の伝播速度 [m/s]
    float DeltaTime; // フレーム時間
    float Damping; // 減衰係数（0.98〜0.999）
    uint PingPong; // 0=Ping読みPong書き, 1=Pong読みPing書き
    float2 RipplePad;
}

// 波紋バッファ（Ping/Pong）
RWTexture2D<float2> RipplePing : register(u0); // .x=現在高さ, .y=前フレーム高さ
RWTexture2D<float2> RipplePong : register(u1);

// ----------------------------------------------------------------
// Kernel 0: SimulateRipple
//   波動方程式を1ステップ前進させる
// ----------------------------------------------------------------
[numthreads(16, 16, 1)]
void SimulateRipple(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    uint N = GridSize;

    // トーラス（ラップアラウンド）対応の隣接インデックス
    int2 idL = int2((id.x - 1 + N) % N, id.y);
    int2 idR = int2((id.x + 1) % N, id.y);
    int2 idD = int2(id.x, (id.y - 1 + N) % N);
    int2 idU = int2(id.x, (id.y + 1) % N);

    // 隣接4セルの現在高さを取得
    float2 cur, left, right, up, down;
    if (PingPong == 0)
    {
        cur = RipplePing[id];
        left = RipplePing[idL];
        right = RipplePing[idR];
        up = RipplePing[idU];
        down = RipplePing[idD];
    }
    else
    {
        cur = RipplePong[id];
        left = RipplePong[idL];
        right = RipplePong[idR];
        up = RipplePong[idU];
        down = RipplePong[idD];
    }

    float h = cur.x; // 現在
    float hPrev = cur.y; // 前フレーム

    // ラプラシアン（有限差分）
    float laplacian = left.x + right.x + up.x + down.x - 4.0 * h;

    // c² * Δt² / Δx²（CFL条件: この値が1未満である必要がある）
    float c = WaveSpeed;
    float dt = DeltaTime;
    float dx = CellSize;
    float k = (c * dt / dx) * (c * dt / dx);
    k = clamp(k, 0.0, 0.49); // 安定性のためクランプ

    // 次ステップの高さ
    float hNext = (2.0 * h - hPrev + k * laplacian) * Damping;

    // 書き込み（新=hNext, 旧=h）
    if (PingPong == 0)
        RipplePong[id] = float2(hNext, h);
    else
        RipplePing[id] = float2(hNext, h);
}

// ----------------------------------------------------------------
// Kernel 1: AddRipplePoint
//   CPU から指定された点に波紋を注入する
// ----------------------------------------------------------------
cbuffer RippleInjection : register(b1)
{
    float2 RippleUV; // 注入位置 UV [0,1]
    float RippleRadius; // 注入半径（グリッド単位）
    float RippleStrength; // 注入強度
}

[numthreads(16, 16, 1)]
void AddRipplePoint(uint3 DTid : SV_DispatchThreadID)
{
    int2 id = (int2) DTid.xy;
    uint N = GridSize;

    // UV化
    float2 uv = (float2) id / N;
    
    // ラップ考慮（UV版）
    float2 diff = abs(uv - RippleUV);
    diff = min(diff, 1.0 - diff); // ← ここが修正ポイント

    float dist = length(diff);

    if (dist > RippleRadius)
        return;

    // コサインプロファイル
    float t = dist / RippleRadius;
    float bump = RippleStrength * cos(t * 3.14159265 * 0.5);
    bump = max(bump, 0.0);

    // 加算
    if (PingPong == 0)
    {
        float2 v = RipplePing[id];
        v.x += bump;
        RipplePing[id] = v;
    }
    else
    {
        float2 v = RipplePong[id];
        v.x += bump;
        RipplePong[id] = v;
    }
}