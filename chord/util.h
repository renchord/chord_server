#ifndef __CHORD_UTIL_H__
#define __CHORD_UTIL_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace chord{

pid_t GetThreadId();

u_int32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size, int skip = 1);

std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "");


}


#endif