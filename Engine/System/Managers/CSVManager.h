#pragma once
/// ===Inclde=== ///
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

///=====================================================/// 
/// CSVManager
///=====================================================///
class CSVManager {
public:

	CSVManager() = default;
	~CSVManager() = default;

public: /// ===Load関数=== ///
	// CSV
	void Load(const std::string& file_path);

private:

	std::mutex mtx_;
	std::condition_variable cv_;
	bool data_ready_ = false;
	std::vector<std::vector<int>> map_data_;
};

