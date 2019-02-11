#pragma once

#include <functional>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "threadex.h"

template<class Task>
struct worker_base
{
    using Result = typename Task::result_type;

    virtual void putTask(Task task) = 0;
    virtual std::optional<Result> getResult() = 0;
};

template<class Task>
struct single_worker :
    worker_base<Task>
{
    using Result = typename Task::result_type;

    single_worker() = default;

    void putTask(Task task) override
    {
        completes_.push(task());
    }

    std::optional<Result> getResult() override
    {
        Result x{ completes_.front() };

        completes_.pop();

        return x;
    }

private:
    std::queue<Result> completes_;
};

struct lock_base
{
    lock_base() :
        running_{ true }
    {}

    virtual ~lock_base()
    {
        stop();
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock{ mutex_ };

        running_ = false;
        lock.unlock();
        cond_.notify_all();
    }

protected:
    bool running_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template<class Type>
struct lock_queue :
    public lock_base
{
    lock_queue() :
        lock_base{}
    {}

    ~lock_queue()
    {}

    template<class _Type>
    void enqueue(_Type && item)
    {
        std::unique_lock<std::mutex> lock{ mutex_ };

        running_ = true;
        queue_.push(item);
        lock.unlock();
        cond_.notify_one();
    }

    std::optional<Type> dequeue()
    {
        std::unique_lock<std::mutex> lock{ mutex_ };

        while (running_ && queue_.empty())
            cond_.wait(lock);

        if (!running_)
            return std::nullopt;

        auto item{ queue_.front() };

        queue_.pop();
        lock.unlock();

        return item;
    }

private:
    std::queue<Type> queue_;
};

template<class Task>
struct parallel_worker :
    worker_base<Task>
{
    using Result = typename Task::result_type;

    parallel_worker(size_t cnt_thread = std::thread::hardware_concurrency()) :
        counter_{ cnt_thread }
    {
        threads_.run(cnt_thread, [this]()
        {
            while (true)
            {
                auto task{ requests_.dequeue() };

                if (!task)
                    break;

                completes_.enqueue((*task)());
                ++counter_;
            }
        });
    }

    ~parallel_worker()
    {
        requests_.stop();
        completes_.stop();
        threads_.join_all();
    }

    void putTask(Task task) override
    {
        --counter_;
        requests_.enqueue(task);
    }

    std::optional<Result> getResult() override
    {
        return completes_.dequeue();
    }

private:
    lock_queue<std::function<Result()>> requests_;
    lock_queue<Result> completes_;
    std::atomic_uint counter_;
    thread_manager threads_;
};
