#include <atomic>
#include "fiber.h"
#include "config.h"
#include "marco.h"

namespace chord {

static Logger::ptr g_logger = CHORD_LOG_NAME("system");
static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};
static thread_local Fiber* t_fiber = nullptr; //线程级变量
static thread_local Fiber::ptr t_threadFiber = nullptr; // likely main fiber 主协程

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
    CHORD_LOG_INFO(g_logger) << "Fiber::Fiber";
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
    CHORD_LOG_INFO(g_logger) << "Fiber::Fiber id" << m_id;
}

Fiber::~Fiber() //协程结束后 回收资源
{
    --s_fiber_count; //cnt --
    if(m_stack)
    {
        CHORD_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
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
    CHORD_LOG_INFO(g_logger) << "Fiber::~Fiber id" << m_id;
}

void Fiber::reset(std::function<void()> cb) //充分利用内存 还可以基于这个内存重新生成新的协程
{
    CHORD_ASSERT(m_stack); //协程一定要有栈
    CHORD_ASSERT(m_state == INIT || m_state == TERM || m_state == EXCEPT); //状态确认

    m_cb = cb;
    if(getcontext(&m_ctx))
    {
        CHORD_ASSERT2(false, "get_context");
    }

    m_ctx.uc_link = nullptr;//此context 关联的上下文 是指针
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}  

// main -> now
void Fiber::swapIn()
{
    //只适用于当前操作的协程，与运行的协程交换
    //操作的对象正常情况下是子协程,而不是主协程

    SetThis(this);
    CHORD_ASSERT(m_state != EXEC); //当前的协程肯定不能在执行中

    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx))
    {
        CHORD_ASSERT2(false, "swapIn_context");
    }
} 

// now -> main
void Fiber::swapOut()
{
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx))
    {
        CHORD_ASSERT2(false, "swapOut_context");
    }

} 

void Fiber::SetThis(Fiber *f) //设置主协程
{
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() //获取主协程
{
    if(t_fiber) //如果主协程存在,直接返回这个主协程的智能指针
    {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);//如果不存在 先new一个主协程， new的过程中 t_fiber会被定义成main_fiber
    CHORD_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::YieldToReady()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();    
}

uint64_t Fiber::TotalFibers()
{
    return s_fiber_count;
}

void Fiber::MainFunc()
{
    Fiber::ptr cur = GetThis();
    CHORD_ASSERT(cur); //
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;

    } catch (std::exception& ex)
    {
        cur->m_state = EXCEPT;
        CHORD_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();
    } catch (...)
    {
        cur->m_state = EXCEPT;
        CHORD_LOG_ERROR(g_logger) << "Fiber Except: ";
    }

}

uint64_t Fiber::GetFiberId()
{
    if(t_fiber) // 
        return t_fiber->getId();
    else 
        return 0;
}

};