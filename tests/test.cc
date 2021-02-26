#include <iostream>
#include "../chord/log.h"
#include "../chord/config.h"
#include "../chord/util.h"

int main(int argc, char** argv) {

    chord::Logger::ptr logger(new chord::Logger);
    logger->addAppender(chord::LogAppender::ptr(new chord::StdOutLogAppender));

    chord::FileLogAppender::ptr file_appender(new chord::FileLogAppender("./log.txt"));
    logger->addAppender(file_appender);

    chord::LogFormatter::ptr fmt(new chord::LogFormatter("%d%T%p%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(chord::LogLevel::ERROR);
    //chord::LogEvent::ptr event(new chord::LogEvent(__FILE__, __LINE__, 0, chord::GetThreadId(), chord::GetFiberId(), time(0)));
    //event->getSS() << "Hello chord log";
    //logger->log(chord::LogLevel::DEBUG, event);
	std::cout << "Hello Chord Logger" << std::endl;

    CHORD_LOG_INFO(logger) << "test marco";
    CHORD_LOG_ERROR(logger) << "test marco error";
    CHORD_LOG_FMT_ERROR(logger, "test marco fmt error %s", "aa");


    auto l = chord::LoggerMgr::GetInstance()->getLogger("XX");
    CHORD_LOG_INFO(l) << "YYY";

    auto m = chord::LoggerMgrPtr::GetInstance()->getLogger("PP");
    CHORD_LOG_FATAL(m) << "FATAL";
    

    return 0;
}
