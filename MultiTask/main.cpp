#include <iostream>
#include <random>

#include "worker.hpp"
#include "task.hpp"

using namespace std;

size_t cnt, delayMin, delayMax, seed;

int main()
{
    cout << "Number of tasks: ";
    cin >> cnt;
    cout << "Min/Max value of delay(100ms unit): ";
    cin >> delayMin >> delayMax;
    cout << "Seed of random value generator: ";
    cin >> seed;

    uniform_int_distribution<size_t> dis{ delayMin, delayMax };
    default_random_engine gen;

    gen.seed(seed);

    auto tasks{ generateTasks(cnt, [&dis, &gen]() { return dis(gen); }) };
    size_t timeSingle{ static_cast<size_t>(
        measureCompleteTasks(tasks, single_worker<task>{}).count()) },
        timeParallel{ static_cast<size_t>(
        measureCompleteTasks(tasks, parallel_worker<task>{}).count()) };

    cout << "single worker: " << timeSingle << " ms\n";
    cout << "parallel worker: " << timeParallel << " ms\n";

    return 0;
}
