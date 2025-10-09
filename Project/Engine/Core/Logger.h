#pragma once
/// ===include=== ///
// Windows
#include <Windows.h>

// C++
#include <string>
#include <format>

/// <summary>
/// 出力ウィンドウの文字表示
/// </summary>
/// <param name="message"></param>
void Log(const std::string& message);

/// <summary>
/// 出力ウィンドウの文字表示
/// </summary>
/// <param name="message"></param>
void Log(const std::wstring& message);

/// <summary>
/// string->wstringに変換
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::wstring ConvertString(const std::string& str);

/// <summary>
/// wstring->stringに変換
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::string ConvertString(const std::wstring& str);