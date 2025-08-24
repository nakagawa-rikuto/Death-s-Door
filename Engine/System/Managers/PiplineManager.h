#pragma once
/// ===Include=== ///
// Engine
#include "Engine/DataInfo/PipelineStateObjectType.h"
#include "Engine/Graphics/Pipeline/PipelineStateObjectCommon.h"
#include "Engine/Graphics/Pipeline/Compiler.h"
// c++
#include <unordered_map>
#include <memory>
#include <utility> // std::pair
#include <functional> // std::hash

/// ===前方宣言=== ///
class DXCommon;

///-------------------------------------------/// 
/// std::pair 用のハッシュ関数を特殊化
///-------------------------------------------///
namespace std {
    template <>
    struct hash<std::pair<PipelineType, BlendMode>> {
        size_t operator()(const std::pair<PipelineType, BlendMode>& p) const noexcept {
            size_t h1 = std::hash<int>()(static_cast<int>(p.first));   // PipelineType のハッシュ値
            size_t h2 = std::hash<int>()(static_cast<int>(p.second)); // BlendMode のハッシュ値
            return h1 ^ (h2 << 1); // ハッシュ値を組み合わせる（XOR とシフト）
        }
    };
}

class PipelineManager {
public:
    
    PipelineManager();
    ~PipelineManager();

    // 初期化
    void Initialize(DXCommon* dxCommon);
    // PSOの設定
    void SetPipeline(ID3D12GraphicsCommandList* commandList, PipelineType type, BlendMode mode, D3D12_PRIMITIVE_TOPOLOGY topology);

private:/// ===変数=== ///
    /// パイプラインの管理
    std::unordered_map<std::pair<PipelineType, BlendMode>, std::unique_ptr<PipelineStateObjectCommon>> pipelines_;
    std::unordered_map<PipelineType, std::unique_ptr<Compiler>> compiler_;

private:/// ===関数=== ///
    /// 指定されたパイプラインを取得
    PipelineStateObjectCommon* GetPipeline(PipelineType type, BlendMode mode);
};