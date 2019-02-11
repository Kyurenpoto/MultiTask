#include "threadex.h"

void thread_manager::run(const size_t cnt, const std::function<void()> func)
{
    for (auto i = 0; i < cnt; ++i)
        threads_.emplace_back(std::thread{ func });
}

void thread_manager::join_all()
{
    for (auto & t : threads_)
        t.join();
}
