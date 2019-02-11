#pragma once

#include <vector>
#include <functional>
#include <thread>

struct thread_manager
{
    void run(const size_t cnt, const std::function<void()> func);
    void join_all();

private:
    std::vector<std::thread> threads_;
};
