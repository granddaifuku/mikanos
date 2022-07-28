#pragma once

enum LogLevel { kError = 3, kWarn = 4, kInfo = 6, kDebug = 7 };

// グローバルなログ優先度の閾値を変更する
void SetLogLevel(LogLevel level);

// 指定された閾値以上のログを記録する
int Log(LogLevel level, const char* format, ...);