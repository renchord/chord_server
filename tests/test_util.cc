#include "../chord/chord.h"

chord::Logger::ptr g_logger = CHORD_LOG_ROOT();

void test_assert() 
{
    CHORD_LOG_INFO(g_logger) << chord::BacktraceToString(10, 2, "    ");
    //CHORD_ASSERT(false);
    CHORD_ASSERT2(0 == 1, "abcedf.txt")
}

int main(int argc, char** argv)
{
    test_assert();
    return 0;
}