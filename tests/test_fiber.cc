#include "../chord/chord.h"

chord::Logger::ptr g_logger = CHORD_LOG_ROOT();

void run_in_fiber()
{
    CHORD_LOG_INFO(g_logger) << "run_in_fiber begin";
    //chord::Fiber::GetThis()->swapOut(); //冲swapIn返回
    chord::Fiber::YieldToHold();
    CHORD_LOG_INFO(g_logger) << "run_in_fiber end";
    chord::Fiber::YieldToHold();
}

int main(int argc, char** argv)
{
    chord::Fiber::GetThis();
    CHORD_LOG_INFO(g_logger) << "main begin";
    chord::Fiber::ptr fiber(new chord::Fiber(run_in_fiber));
    fiber->swapIn();
    CHORD_LOG_INFO(g_logger) << "main after swapIn";
    fiber->swapIn();
    CHORD_LOG_INFO(g_logger) << "main after end";
    return 0;
}