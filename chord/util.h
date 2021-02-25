#ifndef __CHORD_UTIL_H__
#define __CHORD_UTIL_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>


namespace chord{

pid_t GetThreadId();
u_int32_t GetFiberId();

}


#endif