#include <atomic>
#include "fiber.h"
#include "config.h"
#include "marco.h"

namespace chord {

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};
static thread_local Fiber* t_fiber = nullptr; //线程级变量
static thread_local std::shared_ptr<Fiber::ptr> t_threadFiber = nullptr; // likely main fiber 主协程

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
        Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size"); //配置系统派上用场

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) 
    {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }

};

typedef  MallocStackAllocator StackAllocator;

Fiber::Fiber()
{
    //是主协程
    m_state = EXEC;
    SetThis(this); 

    if(getcontext(&m_ctx))
    {
        CHORD_ASSERT2(false, "get_context");
    }

    ++s_fiber_count;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize) //真正运行的协程 需要栈空间
    :m_id(++s_fiber_id)
    ,m_cb(cb)
{
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    //栈的申请
    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx))
    {
        CHORD_ASSERT2(false, "get_context");
    }
    m_ctx.uc_link = nullptr;//此context 关联的上下文 是指针
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);

}

Fiber::~Fiber() //协程结束后 回收资源
{
    --s_fiber_count; //cnt --
    if(m_stack)
    {
        CHORD_ASSERT(m_state == TERM || m_state == INIT );
        StackAllocator::Dealloc(m_stack, m_stacksize); //回收栈
    }
    else
    {//主协程的回收
        CHORD_ASSERT(!m_cb); //肯定没有cb
        CHORD_ASSERT(m_state = EXEC);

        Fiber* cur = t_fiber;
        if(cur == this)
        {
            SetThis(nullptr);
        }
    }
}

void Fiber::reset(std::function<void()> cb) //充分利用内存 还可以基于这个内存重新生成新的协程
{
    CHORD_ASSERT(m_stack); //协程一定要有栈
    CHORD_ASSERT(m_state == INIT || m_state == TERM); //状态确认

    m_cb = cb;
    if()
}  

void Fiber::swapIn()
{

} 

void Fiber::swapOut()
{

} 


Fiber::ptr Fiber::GetThis() 
{

}

void Fiber::YieldToReady()
{

}

void Fiber::YieldToTHold()
{

}

uint64_t Fiber::TotalFibers()
{
}

void Fiber::MainFunc()
{

}


};