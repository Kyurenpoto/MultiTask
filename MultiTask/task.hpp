#pragma once

#include <chrono>
#include <thread>
#include <vector>
#include <functional>

struct task
{
    struct result_type
    {
        explicit result_type(size_t id)
            : id_{ id }
        {}

        size_t id() const
        {
            return id_;
        }

    private:
        size_t id_;
    };

    explicit task(size_t id, size_t delay) :
        id_{ id },
        delay_{ delay * 100 }
    {}

    result_type operator() ()
    {
        std::this_thread::sleep_for(delay_);

        return result_type{ id_ };
    }

private:
    size_t id_;
    std::chrono::milliseconds delay_;
};

template<class TaskArr, class Worker>
auto measureCompleteTasks(TaskArr && tasks, Worker && worker)
{
    auto start{ std::chrono::system_clock::now() };

    for (auto task : tasks)
        worker.putTask(task);

    for (auto task : tasks)
        worker.getResult();

    auto end{ std::chrono::system_clock::now() };

    return std::chrono::duration_cast<chrono::milliseconds>(end - start);
}

std::vector<task> generateTasks(size_t cnt, std::function<size_t()> && gen)
{
    std::vector<task> tasks;

    tasks.reserve(cnt);

    for (size_t i = 0; i < cnt; ++i)
        tasks.emplace_back(task{ i, gen() });

    return tasks;
}
