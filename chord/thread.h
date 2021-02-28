#ifndef __CHORD_THREAD_H__
#define __CHORD_THREAD_H__

#include <thread>
#include <functional> //线程执行方法可以依赖仿函数等?
#include <memory>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

//befroe c11
//  pthreaad_xxx

//c11
//  std::thread, pthread
namespace chord{



template<class T>
struct ScopedLockImpl 
{
public: 
    ScopedLockImpl(T& mutex)
        :m_mutex(mutex)
    {
        m_mutex.lock();
        m_locked = true;
    }
    ~ScopedLockImpl()
    {
        unlock();
    }
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct ReadScopedLockImpl 
{
public: 
    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex)
    {
        m_mutex.rdlock();
        m_locked = true;
    }
    ~ReadScopedLockImpl()
    {
        unlock();
    }
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};


template<class T>
struct WriteScopedLockImpl 
{
public: 
    WriteScopedLockImpl(T& mutex)
        :m_mutex(mutex)
    {
        m_mutex.wrlock();
        m_locked = true;
    }
    ~WriteScopedLockImpl()
    {
        unlock();
    }
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};


class RWMutex { //读写锁
public:
    typedef ReadScopedLockImpl<RWMutex>  ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;
    RWMutex() 
    {
        pthread_rwlock_init(&m_lock, nullptr); 
    }

    ~RWMutex()
    {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;

};


class Semaphore {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait(); // --
    void notify(); // ++
private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore operator=(const Semaphore&) = delete;
private:
    sem_t m_semaphore;
};
 
class Thread { //singleton
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
    Semaphore m_semaphore;
};

}

#endif