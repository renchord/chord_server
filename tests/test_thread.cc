#include "../chord/chord.h"
//#include <vector>
#include <unistd.h>

chord::Logger::ptr g_logger = CHORD_LOG_ROOT(); //就用root logger

void fun1()
{
    CHORD_LOG_INFO(g_logger) << " name: " << chord::Thread::GetName() //线程类的名字
                             << " this.name: " << chord::Thread::GetThis()->getName() //线程的名字(system)
                             << " id: " << chord::GetThreadId() //线程id
                             << " this.id: " << chord::Thread::GetThis()->getId(); //id
    sleep(200);
}

void fun2()
{

}
int main()
{
    CHORD_LOG_INFO(g_logger) << "thread test begin";
    std::vector<chord::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i)
    {   
        chord::Thread::ptr thr(new chord::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.emplace_back(thr);

    }

    //sleep(20);
    for(int i = 0; i < 5; ++i)
    {
        thrs[i]->join();
    }

    CHORD_LOG_INFO(g_logger) << "thread test end";
    return 0;
}