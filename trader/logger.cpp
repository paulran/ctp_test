#include "logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <algorithm>

BaseLog *BaseLog::getInstance()
{
    static BaseLog instance;
    return &instance;
}

void BaseLog::init(const string &level)
{
    string lowerLevel = level;
    std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), ::tolower);
    spdlog::level::level_enum logLevel = spdlog::level::from_str(lowerLevel);
    if (logLevel == spdlog::level::off)
    {
        logLevel = spdlog::level::trace;
    }

    logPtr = spdlog::stdout_color_mt("console");
    logPtr->set_level(logLevel);

    // See doc: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    logPtr->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s %!:%#] %v");

    // Set flush level to err to avoid performance drop on debug mode
    logPtr->flush_on(spdlog::level::trace);
    logPtr->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, spdlog::level::info,
                "Logger level is set to {}", spdlog::level::to_string_view(logLevel));
}

std::shared_ptr<spdlog::logger> BaseLog::logger()
{
    return logPtr;
}
