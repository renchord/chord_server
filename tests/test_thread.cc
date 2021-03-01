#include "../chord/chord.h"
//#include <vector>
#include <unistd.h>

chord::Logger::ptr g_logger = CHORD_LOG_ROOT(); //就用root logger

int count = 0;
//chord::RWMutex s_mutex;

chord::Mutex s_mutex;
void fun1()
{
    CHORD_LOG_INFO(g_logger) << " name: " << chord::Thread::GetName() //线程类的名字
                             << " this.name: " << chord::Thread::GetThis()->getName() //线程的名字(system)
                             << " id: " << chord::GetThreadId() //线程id
                             << " this.id: " << chord::Thread::GetThis()->getId(); //id
    //sleep(20);

    for(int i = 0; i < 10000; ++i)
    {
        //chord::RWMutex::ReadLock lock(s_mutex);
        chord::Mutex::Lock lock(s_mutex); //退出作用域自己析构
        ++count;
    }
}

void fun2()
{
    while(true)
    {
        CHORD_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }

    
}

void fun3()
{
    while(true)
    {
        CHORD_LOG_INFO(g_logger) << "==================================================";
    }
    
}
int main()
{
    CHORD_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/chord/workspace/chord_server/bin/conf/log2.yml"); //Load了Yaml文件的全集
    chord::Config::LoadFromYaml(root);
    std::vector<chord::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i)
    {   
        chord::Thread::ptr thr1(new chord::Thread(&fun2, "name_" + std::to_string(i * 2)));
        chord::Thread::ptr thr2(new chord::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.emplace_back(thr1);
        thrs.emplace_back(thr2);
    }

    //sleep(20);
    for(size_t i = 0; i < thrs.size(); ++i)
    {
        thrs[i]->join();
    }

    CHORD_LOG_INFO(g_logger) << "thread test end";
    CHORD_LOG_INFO(g_logger) << "count=" << count;
    return 0;
}