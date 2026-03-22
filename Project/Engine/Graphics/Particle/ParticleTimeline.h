#pragma once
/// ===Include=== ///
// Engine
#include "Engine/DataInfo/ParticleData.h"
// c++
#include <string>
// JSON
#include <json.hpp>

namespace MiiEngine {
    ///=====================================================/// 
    /// パーティクルタイムラインのJSON変換クラス
    ///=====================================================///
    class ParticleTimelineSerializer {
    public:
        /// <summary>
        /// ParticleTimelineをJSON形式に変換
        /// </summary>
        static nlohmann::json ToJson(const ParticleTimeline& timeline);

        /// <summary>
        /// JSONからParticleTimelineを生成
        /// </summary>
        static ParticleTimeline FromJson(const nlohmann::json& json);

    private:
        // Vector3のJSON変換ヘルパー
        static nlohmann::json Vector3ToJson(const Vector3& vec);
        static Vector3 JsonToVector3(const nlohmann::json& json);
    };
}
