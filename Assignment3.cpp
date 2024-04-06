#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <random>

std::mutex mtx;

//get temperatures randomly between -100F - 70F
void generateTemperatures(std::vector<int> &temperatures, int start, int end)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-100, 70);

    for (int i = start; i < end; ++i)
    {
        std::lock_guard<std::mutex> lock(mtx);
        temperatures[i] = dis(gen);
    }
}

int main()
{
    //make threads
    const int numThreads = 8;
    //number of measurements an hour
    const int totalTemperatures = 60;
    const int chunkSize = totalTemperatures / numThreads;

    std::vector<int> temperatures(totalTemperatures);
//use threads to generate values randomly
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i)
    {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? totalTemperatures : (i + 1) * chunkSize;
        threads.emplace_back(generateTemperatures, std::ref(temperatures), start, end);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    std::sort(temperatures.begin(), temperatures.end());
    //find the 5 highest and 5 lowest values in the hour
    std::cout << "5 Lowest Temperatures:" << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        std::cout << temperatures[i] << "F ";
    }
    std::cout << std::endl;

    std::cout << "5 Highest Temperatures:" << std::endl;
    for (int i = totalTemperatures - 5; i < totalTemperatures; ++i)
    {
        std::cout << temperatures[i] << "F ";
    }
    std::cout << std::endl;

//get the largest difference
    int largestDifference = 0;
    for (int i = 0; i < 10; ++i)
    {
        int difference = temperatures[i + 1] - temperatures[i];
        if (difference > largestDifference)
        {
            largestDifference = difference;
        }
    }
    std::cout << "Largest difference from 10 consecutive values: " << largestDifference << "F" << std::endl;

    return 0;
}
