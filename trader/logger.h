#pragma once

// Define macro to output file name and line number
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_TRACE_ON
// #define SPDLOG_DEBUG_ON

#include "spdlog/spdlog.h"
#include <string>
using namespace std;

class BaseLog
{
private:
    BaseLog() = default;

public:
    static BaseLog *getInstance();

    void init(const string &level);

    std::shared_ptr<spdlog::logger> logger();

private:
    std::shared_ptr<spdlog::logger> logPtr;
};

#define LogInit(level) BaseLog::getInstance()->init(level)

// See SPDLOG_LOGGER_CALL
#define SPDLOG_BASE(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, level, __VA_ARGS__)
#define LogTrace(...) SPDLOG_LOGGER_CALL(BaseLog::getInstance()->logger(), spdlog::level::trace, __VA_ARGS__)
#define LogDebug(...) SPDLOG_LOGGER_CALL(BaseLog::getInstance()->logger(), spdlog::level::debug, __VA_ARGS__)
#define LogInfo(...) SPDLOG_LOGGER_CALL(BaseLog::getInstance()->logger(), spdlog::level::info, __VA_ARGS__)
#define LogWarn(...) SPDLOG_LOGGER_CALL(BaseLog::getInstance()->logger(), spdlog::level::warn, __VA_ARGS__)
#define LogError(...) SPDLOG_LOGGER_CALL(BaseLog::getInstance()->logger(), spdlog::level::err, __VA_ARGS__)
#define LogCirtical(...) SPDLOG_LOGGER_CALL(BaseLog::getInstance()->logger(), spdlog::level::critical, __VA_ARGS__)
