#include "gtest/gtest.h"

#include "../MultiTask/task.hpp"
#include "../MultiTask/worker.hpp"

#include <random>

using namespace std;

TEST(IntegrateTest, Generate)
{
    uniform_int_distribution<size_t> dis{ 1, 10 };
    default_random_engine gen;

    gen.seed(0);

    auto tasks = generateTasks(10, [&dis, &gen]() { return dis(gen); });

    ASSERT_EQ(10, tasks.size());
}

void Worker_Single()
{
    {
        single_worker<task> worker;

        worker.putTask(task{ 0, 10 });
        
        ASSERT_EQ(0, (*(worker.getResult())).id());
    }
    {
        single_worker<task> worker;

        for (int i = 0; i < 4; ++i)
            worker.putTask(task{ 0, 10 });

        for (int i = 0; i < 4; ++i)
            ASSERT_EQ(0, (*(worker.getResult())).id());
    }
}

void Worker_Parallel()
{
    {
        parallel_worker<task> worker;

        worker.putTask(task{ 0, 10 });

        ASSERT_EQ(0, (*(worker.getResult())).id());
    }
    {
        parallel_worker<task> worker;

        for (int i = 0; i < 4; ++i)
            worker.putTask(task{ 0, 10 });

        for (int i = 0; i < 4; ++i)
            ASSERT_EQ(0, (*(worker.getResult())).id());
    }
}

TEST(IntegrateTest, Worker)
{
    Worker_Single();
    Worker_Parallel();
}

bool isCorrectMeasured(size_t expected, size_t actual)
{
    return actual >= expected && actual < expected + 100;
}

void Measure_Single()
{
    {
        uniform_int_distribution<size_t> dis{ 1, 1 };
        default_random_engine gen;
        
        size_t time = static_cast<size_t>(measureCompleteTasks(
            generateTasks(1, [&dis, &gen]() { return dis(gen) * 10; }),
            single_worker<task>{}).count());

        ASSERT_EQ(true, (isCorrectMeasured(1'000, time)));
    }
    {
        uniform_int_distribution<size_t> dis{ 1, 1 };
        default_random_engine gen;

        size_t time = static_cast<size_t>(measureCompleteTasks(
            generateTasks(4, [&dis, &gen]() { return dis(gen) * 10; }),
            single_worker<task>{}).count());

        ASSERT_EQ(true, (isCorrectMeasured(4'000, time)));
    }
}

void Measure_Parallel()
{
    {
        uniform_int_distribution<size_t> dis{ 1, 1 };
        default_random_engine gen;

        size_t time = static_cast<size_t>(measureCompleteTasks(
            generateTasks(1, [&dis, &gen]() { return dis(gen) * 10; }),
            parallel_worker<task>{}).count());

        ASSERT_EQ(true, (isCorrectMeasured(1'000, time)));
    }
    {
        uniform_int_distribution<size_t> dis{ 1, 1 };
        default_random_engine gen;

        size_t time = static_cast<size_t>(measureCompleteTasks(
            generateTasks(4, [&dis, &gen]() { return dis(gen) * 10; }),
            parallel_worker<task>{}).count());

        ASSERT_EQ(true, (isCorrectMeasured(1'000, time)));
    }
}

TEST(IntegrateTest, Measure)
{
    Measure_Single();
    Measure_Parallel();
}
