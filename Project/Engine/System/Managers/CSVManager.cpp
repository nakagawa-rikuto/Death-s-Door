#include "CSVManager.h"
// c++
#include <thread>
#include <cassert>

///-------------------------------------------/// 
/// CSVの読み込み関数
///-------------------------------------------///
void CSVManager::Load(const std::string & file_path) {
    std::vector<std::vector<int>> temp_data;
    std::ifstream file(file_path);
    std::string line;

    // ファイルが開けなかった場合のエラーメッセージ
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    // ファイルを1行ずつ読み込む
    while (std::getline(file, line)) {
        std::vector<int> row; // 1行分のデータを格納するベクター
        size_t pos = 0;

        // カンマで分割して整数に変換し、行データに追加
        while ((pos = line.find(",")) != std::string::npos) {
            row.push_back(std::stoi(line.substr(0, pos))); // カンマまでの文字列を数値に変換して追加
            line.erase(0, pos + 1); // 処理済みの部分を削除
        }
        row.push_back(std::stoi(line)); // 最後の要素を追加
        temp_data.push_back(row); // 行データを一時データに追加
    }

    file.close(); // ファイルを閉じる

    // スレッド間で共有するデータを更新
    {
        std::lock_guard<std::mutex> lock(mtx_);
        map_data_ = temp_data;
        data_ready_ = true; // データ準備完了フラグを設定
    }

    cv_.notify_all(); // 他のスレッドに通知
}

