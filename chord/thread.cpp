#include "thread.h"
#include "log.h"
#include "config.h"

namespace chord{

static thread_local Thread* t_thread = nullptr; //指向当前线程
static thread_local std::string t_thread_name = "UNKNOW"; //当前线程的名称

static chord::Logger::ptr g_logger = CHORD_LOG_NAME("system"); //获取system的g_logger


Semaphore::Semaphore(uint32_t count)
{
    if(sem_init(&m_semaphore, 0, count))
    {
        throw std::logic_error("sem_init error");
    }
}
Semaphore::~Semaphore()
{
    sem_destroy(&m_semaphore);
}
void Semaphore::wait() // --
{

    if(sem_wait(&m_semaphore))//直接拿信号量 trywait不会造成阻塞
    {
        throw std::logic_error("sem_wait error");
    }
    //if(errno != EINTR)
    
}
void Semaphore::notify() // ++ post 信号量 唤醒wait的
{
    if(sem_post(&m_semaphore))
    {
        throw std::logic_error("sem_post error");
    }
}


Thread* Thread::GetThis()
{
    return t_thread;
}
const std::string& Thread::GetName() //获取线程名称
{
    return t_thread_name;
}

void Thread::SetName(const std::string& name)
{
    if(name.empty())
        return;
    if(t_thread != nullptr)
    {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name) //入参是返回值类型为void的不带参数的函数指针???
    :m_cb(cb)
    ,m_name(name)
{
    if(name.empty())
    {
        m_name = "UNKNOW";

    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    // m_thread 存放线程handle
    // attr 不需要因此填 nullptr
    // para3 开始的线程函数地址
    if(rt)
    {
        CHORD_LOG_ERROR(g_logger) << "pthread_create thread fail, rt = "
            << rt << "name = " << name;
            throw std::logic_error("pthread_create error");
    }

    m_semaphore.wait();//等初始化
}

Thread::~Thread()
{
    if(m_thread)
    {
        pthread_detach(m_thread); //析构了 detach创建了的m_thread
    }
}

void Thread::join()
{
    if(m_thread)
    {
        int rt = pthread_join(m_thread, nullptr); //以阻塞的方式等待线程结束
        if(rt)
        {
            CHORD_LOG_ERROR(g_logger) << "pthread_join thread fail, rt = "
            << rt << "name = " << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void* arg)//线程run的函数
{
    Thread* thread = (Thread*) arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = chord::GetThreadId(); //拿到线程id
    //获取当前线程id 并命名
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());//给线程命名 最大16个字符数

    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return 0;

}

}

