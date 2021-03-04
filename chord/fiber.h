//协程的头文件
#ifndef  __CHORD_FIBER_H__
#define  __CHORD_FIBER_H__

#include <memory> //智能指针
#include <functional>
#include <ucontext.h>
#include "thread.h"

namespace chord{


class Fiber : public std::enable_shared_from_this<Fiber>{
public:
        
    typedef std::shared_ptr<Fiber> ptr;
    enum State { //Fiber status
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber(); //不允许在栈上生成Fiber

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();
    void reset(std::function<void()> cb);  //重置协程函数并重置状态 一般在INIT 或TERM状态
    void swapIn();  //切换到当前协程进行执行
    void swapOut(); //切换到后台执行
    uint64_t getId() const { return m_id; }

public:
    static void SetThis(Fiber* f);
    static Fiber::ptr GetThis(); //返回当前执行点的协程的智能指针
    static void YieldToReady(); //协程切换到后台 设置为Ready状态
    static void YieldToHold(); //协程切换到后台 设置为Hold状态
    static uint64_t TotalFibers(); //统计总协程数

    static void MainFunc(); //
    static uint64_t GetFiberId();

private:
    uint64_t m_id = 0; //协程id
    uint32_t m_stacksize = 0; //协程栈的大小
    State m_state = INIT;    //状态
    ucontext_t m_ctx; //上下文

    void* m_stack = nullptr;
    std::function<void()> m_cb; //真正执行的协程方法
};


}





#endif