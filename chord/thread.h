#ifndef __CHORD_THREAD_H__
#define __CHORD_THREAD_H__

#include <thread>
#include <functional> //线程执行方法可以依赖仿函数等?
#include <memory>
#include <string>
#include <stdint.h>
#include <pthread.h>
//befroe c11
//  pthreaad_xxx

//c11
//  std::thread, pthread
namespace chord{

class Thread{ //singleton

public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const {return m_id; }
    void join();
    const std::string& getName() const { return m_name; }
    //staic 的方法针对static成员
    static Thread* GetThis();
    static const std::string& GetName(); //获取线程名称
    static void SetName(const std::string& name);
private:

    Thread(const Thread& ) = delete; 
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread &) = delete;

    static void* run(void* arg);

private:
    pid_t  m_id = -1; //线程id
    pthread_t m_thread = 0; //初始化线程, pthread_t的类型是常整型
    std::function<void()> m_cb;
    std::string m_name;

};



}

#endif