#ifndef __CHORD_LOG_H__
#define __CHORD_LOG_H__

#include <iostream>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "util.h"
#include "singleton.h"

#define CHORD_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        chord::LogEventWrap(chord::LogEvent::ptr(new chord::LogEvent(logger,\
        level, __FILE__, __LINE__, 0, chord::GetThreadId(), chord::GetFiberId(), \
        time(0)))).getSS()

#define CHORD_LOG_DEBUG(logger)   CHORD_LOG_LEVEL(logger,   chord::LogLevel::DEBUG)
#define CHORD_LOG_INFO(logger)    CHORD_LOG_LEVEL(logger,   chord::LogLevel::INFO)
#define CHORD_LOG_WARN(logger)    CHORD_LOG_LEVEL(logger,   chord::LogLevel::WARN)
#define CHORD_LOG_ERROR(logger)   CHORD_LOG_LEVEL(logger,   chord::LogLevel::ERROR)
#define CHORD_LOG_FATAL(logger)   CHORD_LOG_LEVEL(logger,   chord::LogLevel::FATAL)

#define CHORD_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        chord::LogEventWrap(chord::LogEvent::ptr(new chord::LogEvent(logger, \
        level, __FILE__, __LINE__, 0, chord::GetThreadId(), chord::GetFiberId(), \
        time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define CHORD_LOG_FMT_DEBUG(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_INFO(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::INFO, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_WARN(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::WARN, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_ERROR(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::ERROR, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_FATAL(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::FATAL, fmt, __VA_ARGS__)

#define CHORD_LOG_ROOT()   chord::LoggerMgr::GetInstance()->getRoot()


namespace chord{

class Logger;

//日志级别
class LogLevel{
public:
    enum Level
    {
        UNKNOWN = 0,
        DEBUG  = 1,
        INFO   = 2,
        WARN   = 3,
        ERROR  = 4,
        FATAL  = 5
    };

    static const char* ToString(LogLevel::Level level);
};

//日志事件
class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, 
            uint32_t thread_id, uint32_t fiber_id, uint64_t time);
    const char *getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapsed() const { return m_elapsed; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    std::string getContent() const { return m_ss.str(); }
    std::stringstream &getSS() { return m_ss; }
    std::shared_ptr<Logger> getLogger() const  {return m_logger;}
    LogLevel::Level getLevel() const { return m_level; }
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list all);

private:
    const char *m_file = nullptr; //文件名
    int32_t m_line = 0;           //行号
    uint32_t m_elapsed = 0;       //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;      //线程id
    uint32_t m_fiberId = 0;       //协程id
    uint64_t m_time;              //时间戳
    std::stringstream m_ss;

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr event);
    ~LogEventWrap();
    LogEvent::ptr getEvent() const {return m_event; }
    std::stringstream& getSS();

private:
    LogEvent::ptr m_event;
};


    //%m  -- message body
    //%p  -- level
    //%r  -- the time aft setup
    //%t  -- thread id
    //%n  -- enter change line
    //%d  -- time
    //%f  -- filename
    //%l  -- line number 

//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    //%t   %theread_id%m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem
    {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem(const std::string& fmt = "") {}
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
private:
    void init();

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};
   
//日志输出器
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0; //子类必须要实现这个方法
    void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
    LogFormatter::ptr getFormatter() const { return m_formatter; }
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level level) {m_level = level;}
protected:
    LogFormatter::ptr m_formatter;
    LogLevel::Level m_level = LogLevel::DEBUG;
private:
    
};




//日志器
class Logger :public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);


    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);
    void error(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level val) { m_level = val; }

    const std::string& getName() const {return m_name; }

private:
    std::string m_name;                         //日志名称
    LogLevel::Level m_level;                    //日志级别
    LogFormatter::ptr m_formatter;              //formatter集合
    std::list<LogAppender::ptr> m_appenders;    //appener集合
};


//输出到控制台的Appender
class StdOutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdOutLogAppender> ptr;
    virtual void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override; //子类必须要实现这个方法
private:
};

//输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    virtual void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override; //子类必须要实现这个方法

    //重新打开 返回成功True
    bool reopen();

private:
    std::string m_filename;
    std::ofstream m_filestream;
};



class LoggerManager{
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);
    void init();
    Logger::ptr getRoot() const {return m_root; }
private:
    std::map<std::string, Logger::ptr> m_logger;
    Logger::ptr m_root;
};
typedef chord::Singleton<LoggerManager> LoggerMgr;
typedef chord::SingletonPtr<LoggerManager> LoggerMgrPtr;

} // namespace chord

#endif
