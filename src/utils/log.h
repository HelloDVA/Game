#include<mutex>
#include<string>
#include<sstream>
#include<fstream>

enum    LOG_LEVEL
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NONE
};
enum    LOG_TYPE
{
    LOG_TYPE_CONSOLE,
    LOG_TYPE_FILE,
    LOG_TYPE_NONE
};

class Log{
    public:
        static Log* getlog();
        void WriteLog(LOG_LEVEL log_level, const char* file_name, const char* function, int line_number, const char* message);
        void CloseLog();

    private:
        // In singleton partten the constrctor is private
        Log();
        void init(LOG_LEVEL log_level, LOG_TYPE log_type);

        ~Log();

		Log(const Log &log) = delete;
		Log& operator=(const Log &log) = delete;

        // tools
        std::string GetCurrentTime();
        const char* GetLevelToString(LOG_LEVEL log_level);

        LOG_LEVEL log_level_;
        LOG_TYPE log_type_;

        static Log *log;
        static std::mutex log_mutex;
        static std::string log_buffer;

};
