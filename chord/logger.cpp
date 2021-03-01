#include <functional>
#include <map>
#include <iostream>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"
#include "singleton.h"
#include "config.h"

namespace chord{


LogLevel::Level LogLevel::FromString(const std::string& str){
#define XX(level, v) \
    if(str == #v) \
    { \
        return LogLevel::level;  \
    }
    
    XX(DEBUG, debug);
    XX(INFO , info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO , INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::Level::UNKNOWN;
#undef XX
}

const char* LogLevel::ToString(LogLevel::Level level){
    switch(level)
    {
#define XX(name)         \
    case LogLevel::name: \
        return #name; \
        break;

        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
    default:
        return "unknown";
    }
}
void LogAppender::setFormatter(LogFormatter::ptr val)
{
    MutexType::Lock lock(m_mutex); //其实利用类的构造和析构来完成加锁和解锁
    m_formatter = val;
    if(m_formatter) 
    {
        m_has_Formatter = true;
    }
    else
    {
        m_has_Formatter = false;
    }
}

LogFormatter::ptr LogAppender::getFormatter()
{
    MutexType::Lock lock(m_mutex);;
    return m_formatter;
}

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    ElapseFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getElapsed();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        //os << logger->getName();
        os << event->getLogger()->getName(); //原始的logger
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getThreadId();
    }
};


class FiberIdFormatItem : public LogFormatter::FormatItem{
public:
    FiberIdFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
        }

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};


class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getLine();
    }
};


class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string & str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string& str)
        :FormatItem(str), m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string& str = ""){}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << "\t";
    }
private:
    std::string m_string;
};

LogEvent::LogEvent(Logger::ptr logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, 
    uint32_t thread_id, uint32_t fiber_id, uint64_t time)
    :m_file(file)
    ,m_line(line)
    ,m_elapsed(elapse)
    ,m_threadId(thread_id)
    ,m_fiberId(fiber_id)
    ,m_time(time)
    ,m_logger(logger)
    ,m_level(level)
    {
}

LogEventWrap::LogEventWrap(LogEvent::ptr event)
    :m_event(event)
{
}

LogEventWrap::~LogEventWrap()
{
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrap::getSS() {
    return m_event->getSS();
}

void LogEvent::format(const char* fmt, ...)
{
    va_list a1;
    va_start(a1, fmt);
    format(fmt, a1);
    va_end(a1);
}

void LogEvent::format(const char* fmt, va_list a1)
{
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, a1);
    if(len != -1)
    {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

Logger::Logger(const std::string& name)
    :m_name(name), m_level(LogLevel::DEBUG)
{

    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%f:%l%T%m%n"));
}           


void Logger::setFormatter(LogFormatter::ptr val)
{
    MutexType::Lock lock(m_mutex);
    m_formatter = val;

    for(auto& i : m_appenders)
    {
        MutexType::Lock ll(i->m_mutex); //更新formmater时需要加锁
        if(!i->m_has_Formatter)//如果appender没有formatter
        //也需要更新
        {
            i->m_formatter = m_formatter;
        }
    }
}

LogFormatter::ptr Logger::getFormatter() 
{
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

void Logger::setFormatter(const std::string& val)
{
    chord::LogFormatter::ptr new_val (new chord::LogFormatter(val)); //通过init()来更新 new_val obj中的error 判断是否合法 不合法就没有必要做修改
    if(new_val->isError())
    {
        std::cout << "Logger setFormatter name=" << m_name 
                  << "value=" << val << " invalid formatter" 
                  << std::endl;
        return;
    } 
    setFormatter(new_val);//转而调用 formatter的方式来实现设置
}

void Logger::addAppender(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
    if(!appender->getFormatter())
    {
        MutexType::Lock ll(appender->m_mutex);
        //appender->setFormatter(m_formatter);
        appender->m_formatter = m_formatter;//logger自己添加appender时 不会走到setFormatter的流程 即这个
        //appender还是认为自己是没有formatter的
    }
    m_appenders.push_back(appender);
}

void Logger::clearAppenders()
{
    MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}

void Logger::delAppender(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
    for(auto it = m_appenders.begin(); it != m_appenders.end();++it)
    {
        if(*it == appender)
        {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        auto self = shared_from_this();
        MutexType::Lock lock(m_mutex);
        if(!m_appenders.empty())
        {
            for(auto &i : m_appenders)
            {
                i->log(self, level, event);
            }
        }
        else if (m_root != nullptr)
        {
            m_root->log(level, event);
        }

    }
}

void Logger::debug(LogEvent::ptr event)
{
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event)
{
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event)
{
    log(LogLevel::WARN, event);
}
void Logger::fatal(LogEvent::ptr event)
{
    log(LogLevel::FATAL, event);
}
void Logger::error(LogEvent::ptr event)
{
    log(LogLevel::ERROR, event);
}

std::string Logger::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOWN)
    {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_formatter != nullptr) //智能指针非空
    {
        node["formatter"] = m_formatter->getPattern();
    }
    for(auto& i :m_appenders)
    {
        node["appenders"].push_back(YAML::Load(i->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

FileLogAppender::FileLogAppender(const std::string &filename)
    :m_filename(filename)
{
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        uint64_t now = time(0);
        if(now != m_lastTime + 3)
        {
            reopen();
            m_lastTime = now;
        }
        MutexType::Lock lock(m_mutex);
        if(!(m_filestream << m_formatter->format(logger, level, event)))
        {
            std::cout << "file_appender log error" << std::endl;
        }
    }
}


std::string FileLogAppender::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if(m_level != LogLevel::UNKNOWN)
    {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_has_Formatter && m_formatter)
    {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

bool FileLogAppender::reopen()
{
    MutexType::Lock lock(m_mutex);
    if(m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !m_filestream;
}

void StdOutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        std::cout << m_formatter->format(logger, level, event);
    }
}

std::string StdOutLogAppender::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if(m_level != LogLevel::UNKNOWN)
    {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_has_Formatter && m_formatter) //确保有自己的fomratter
    {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}


LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern)
{
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for(auto& i : m_items)
    {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}


// %xxx    %xxx{xxx}   %%
void LogFormatter::init()
{
    // str, format, type 三元组
    std::vector<std::tuple<std::string, std::string, int>> vec;

    std::string nstr;
    for (size_t i = 0; i < m_pattern.size(); i++)
    {
        if(m_pattern[i] != '%')
        {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size())
        {
            if(m_pattern[i + 1] == '%')
            {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size())
        {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}'))
            {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0)
            {
                if(m_pattern[n] == '{')
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }
            else if(fmt_status == 1)
            {
                if(m_pattern[n] == '}')
                {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size())
            {
                if(str.empty())
                {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0)
        {
            if(!nstr.empty())
            {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            //str = m_pattern.substr(i + 1, n - i - 1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
        else if(fmt_status == 1)
        {
            std::cout << "Pattern parse error : " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
        else if(fmt_status == 2)
        {
            if(!nstr.empty())
            {
                vec.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }

    }
    if(!nstr.empty())
    {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(F, FiberIdFormatItem),           //F:协程id
        XX(T, TabFormatItem),
#undef XX
    };

    for(auto& i : vec)
    {
        if(std::get<2>(i) == 0)
        {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else
        {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end())
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            }
            else
            {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::endl;
}


LoggerManager::LoggerManager()//Root是单例
{
    m_root.reset(new Logger);//rest函数是C++自带函数
    m_root->addAppender(LogAppender::ptr(new StdOutLogAppender));

    m_logger[m_root->m_name] = m_root;//把root自己也放到map中
    init();
}
Logger::ptr LoggerManager::getLogger(const std::string& name)
{
    MutexType::Lock lock(m_mutex);
    auto it = m_logger.find(name);
    if(it != m_logger.end())//找到这个name的log
    {
        return it->second;
    }
    //not find
    Logger::ptr logger(new Logger(name)); //定义一个logger
    logger->m_root = m_root;
    m_logger[name] = logger;
    return logger;
}

struct LogAppenderDefine{
    int type = 0; // 1 File, 2 Stdout
    LogLevel::Level level = LogLevel::UNKNOWN;
    std::string formatter; //specific formatter
    std::string file;
    bool operator==(const LogAppenderDefine& rhs) const
    {
        return type == rhs.type && level == rhs.level && formatter == rhs.formatter && file == rhs.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOWN;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& rhs) const 
    {
        return name == rhs.name && level == rhs.level && formatter == rhs.formatter && appenders == rhs.appenders;
    }

    bool operator<(const LogDefine & rhs) const
    {
        return name < rhs.name;
    }
    
};


template<>//偏特化 针对 LogDefine
class LexicalCast <std::string, std::set<LogDefine>> {
public:
    std::set<LogDefine> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);
        std::set<LogDefine> vec; 
        for(size_t i = 0; i < node.size(); ++i)
        {   
            const auto& n = node[i];
            if(!n["name"].IsDefined())//config is error
            {
                std::cout << "log config error: name is null " << n 
                          << std::endl;
                continue;
                //to prevent self-loop
            }  
            LogDefine ld;
            ld.name = n["name"].as<std::string>();
            ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
            if(n["formatter"].IsDefined())
            {
                ld.formatter = n["formatter"].as<std::string>();
            }

            if(n["appenders"].IsDefined())
            {
                for(size_t x = 0; x < n["appenders"].size(); ++x)
                {
                    auto a = n["appenders"][x];
                    if(!a["type"].IsDefined())
                    {
                        std::cout << "log config error: appender is null " << a 
                          << std::endl;
                        continue;
                    }
                    std::string type = a["type"].as<std::string>();
                    LogAppenderDefine lad;
                    if(type == "FileLogAppender")
                    {
                        lad.type = 1;
                        if(!a["file"].IsDefined())
                        {
                            std::cout << "log config error: file appender is null " << a 
                            << std::endl;
                            continue;
                        }
                        lad.file = a["file"].as<std::string>();
                        if(a["formatter"].IsDefined())
                        {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    }
                    else if(type == "StdoutLogAppender")
                    {
                        lad.type = 2;
                    }
                    else
                    {
                        std::cout << "log config error: appender is null " << a 
                          << std::endl;
                    }
                    ld.appenders.push_back(lad);
                }
            }
            vec.insert(ld);
        }
        return vec;
    }
};


template<>//偏特化 针对
class LexicalCast <std::set<LogDefine>, std::string> {
public:
    std::string operator() (const std::set<LogDefine> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            YAML::Node n;
            n["name"] = i.name;
            if(i.level == LogLevel::UNKNOWN)
            {
                n["level"] = LogLevel::ToString(i.level);   
            }
            if(!i.formatter.empty())
            {
                n["formatter"] = i.formatter;
            }

            for(auto& a : i.appenders)
            {
                YAML::Node na;
                if(a.type == 1) //file 
                {
                    na["type"] = "FileLogAppender";
                    na["file"] = a.file;
                }
                else if (a.type == 2)
                {
                    na["type"] = "StdoutLogAppender";
                }
                if(a.level != LogLevel::UNKNOWN)
                {
                    na["level"] = LogLevel::ToString(a.level);
                }
                

                if(!a.formatter.empty())
                {
                    na["formatter"] = a.formatter;
                }
                n["appenders"].push_back(na);
            }
            node.push_back(n);
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};


chord::ConfigVar<std::set<LogDefine>>::ptr g_log_defines = 
    chord::Config::Lookup("logs", std::set<LogDefine>(), "logs config"); //偏特化如何考虑?


//注册事件
struct LogIniter {
    LogIniter() {
        g_log_defines->addListener(0xF1E231, [](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value) 
        {

            CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i : new_value)
            {
                auto it = old_value.find(i);
                chord::Logger::ptr logger;
                if(it == old_value.end()) //new logger
                {
                    logger = CHORD_LOG_NAME(i.name);
                }
                else  // mod
                {
                    if(!(i == *it))//not equal :need to modify
                    {
                        logger = CHORD_LOG_NAME(i.name); // first find this logger
                    }
                }
                logger->setLevel(i.level);
                if(!i.formatter.empty())
                {
                    logger->setFormatter(i.formatter);
                }
                logger->clearAppenders();
                for(auto& a : i.appenders)
                {
                    chord::LogAppender::ptr ap;
                    if(a.type == 1) //file
                    {
                        ap.reset(new FileLogAppender(a.file));
                    }
                    else if(a.type == 2) //stdout
                    {
                        ap.reset(new StdOutLogAppender);
                    }
                    ap->setLevel(a.level);
                    if(!a.formatter.empty())//这里是加入appenders 的formatter 必不可少 
                    //否则无法解析yaml文件中appenders里的formatter
                    {
                        LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                        if(!fmt->isError())
                        {
                            ap->setFormatter(fmt);
                        }
                        else
                        {
                            std::cout << "log name="<< i.name << "appender type=" << a.type << "formatter" << a.formatter << "invalid" << std::endl;
                        }
                    }
                    logger->addAppender(ap);
                }
            }
            for(auto& i : old_value)
            {
                auto it = new_value.find(i);
                if(it == new_value.end())
                {
                    //delete logger
                    auto logger = CHORD_LOG_NAME(i.name);
                    logger->setLevel((LogLevel::Level)100); // too high to log
                    logger->clearAppenders();
                }
            }
        //
        });
    }
};

static LogIniter __log_init;
std::string LoggerManager::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    for(auto& i : m_logger)
    {
        node.push_back(YAML::Load(i.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}
void LoggerManager::init(){
}

}
