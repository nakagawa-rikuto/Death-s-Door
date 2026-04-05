#pragma once
/// ===Include=== ///
#include "Data/AttackData.h"
#include <string>

///=====================================================/// 
/// 攻撃データのJson保存・読み込みクラス
///=====================================================///
class AttackDataSerializer {
public:

    /// <summary>
    /// 攻撃データをJSONファイルに保存
    /// </summary>
    /// <param name="data">保存する攻撃データへの const 参照</param>
    /// <param name="filepath">保存先のファイルパス</param>
    /// <returns>保存に成功したら true、失敗したら false</returns>
	static bool SaveToJson(const AttackData& data, const std::string& filepath);

    /// <summary>
    /// JSONファイルから攻撃データを読み込み
    /// </summary>
    /// <param name="data">読み込んだデータを格納する AttackData への参照</param>
    /// <param name="filepath">読み込み元のファイルパス</param>
    /// <returns>読み込みに成功したら true、失敗したら false</returns>
    static bool LoadFromJson(AttackData& data, const std::string& filepath);
};

