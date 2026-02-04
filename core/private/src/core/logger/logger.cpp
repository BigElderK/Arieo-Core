#include "base/prerequisites.h"
#include "core/core.h"
#include <sstream>
#include <iostream>

#define ELPP_THREADING_ENABLED 
#define ELPP_ASYNC_LOGGING 
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

#if defined(ARIEO_PLATFORM_ANDROID)
#include <android/log.h>
#define LOG_TAG "ArieoEngine"

class LogDispatchCallback : public el::LogDispatchCallback
{
    protected:
        void handle(const el::LogDispatchData* data) override
        {
            // This callback is called for every log message
            // You can customize log handling here if needed
            // For example, you could forward logs to another system
            const el::LogMessage* log_msg = data->logMessage();
            if (log_msg != nullptr) 
            {
                // data->dispatchAction() = el::base::DispatchAction::None; // Prevent default handling
                // Example: Print log message to standard output
                switch(log_msg->level()) 
                {
                    case el::Level::Trace:
                    case el::Level::Debug:
                        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", log_msg->message().c_str());
                        break;
                    case el::Level::Info:
                        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", log_msg->message().c_str());
                        break;
                    case el::Level::Warning:
                        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s", log_msg->message().c_str());
                        break;
                    case el::Level::Error:
                        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", log_msg->message().c_str());
                        break;
                    case el::Level::Fatal:
                        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", log_msg->message().c_str());
                        break;
                    default:
                        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", log_msg->message().c_str());
                        break;
                }
            }
        }
};
#endif

namespace Arieo::Core
{
    static el::Logger* g_default_logger = nullptr; //el::Loggers::getLogger("default");
    
    //TODO: do not use mutex, it is slow
    static std::mutex g_logger_mutex;

    void Logger::setDefaultLogger(const std::string&& logger_name)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::tm time_info = {};
#ifdef _WIN32
        localtime_s(&time_info, &in_time_t);
#else
        localtime_r(&in_time_t, &time_info);
#endif

        std::stringstream log_file_name_stream;
        log_file_name_stream 
            << SystemUtility::FileSystem::getFormalizedPath("${EXE_DIR}/logs/") 
            << std::put_time(&time_info, "%Y-%m-%d_%H-%M-%S")
            << ".log";

        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger = el::Loggers::getLogger(logger_name);

        // Configure the default logger location
        el::Loggers::reconfigureAllLoggers(
            el::ConfigurationType::Filename, 
            log_file_name_stream.str()
        );
#if defined(ARIEO_PLATFORM_ANDROID)
        el::Helpers::installLogDispatchCallback<LogDispatchCallback>("LogDispatchCallback");
        LogDispatchCallback* dispatcher = el::Helpers::logDispatchCallback<LogDispatchCallback>("LogDispatchCallback");
        dispatcher->setEnabled(true);
#endif
    }

    void Logger::verbose(const std::uint8_t level, const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->verbose(level, expression);
    }

    void Logger::trace(const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->trace(expression);
    }

    void Logger::debug(const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->debug(expression);
    }

    void Logger::info(const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->info(expression);
    }

    void Logger::warn(const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->warn(expression);
    }

    void Logger::error(const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->error(expression);
    }

    void Logger::fatal(const std::string& expression)
    {
        std::unique_lock<std::mutex> lock(g_logger_mutex);
        g_default_logger->fatal(expression);
    }
}
