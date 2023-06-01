#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <string>
std::mutex mtx, print_mtx;

using namespace std;

class Bank {
private:
    std::vector<int> available;
    std::vector<std::vector<int>> max;
    std::vector<std::vector<int>> allocation;
    std::vector<std::vector<int>> need;

public:
    Bank(const std::vector<int>& resources, const std::vector<std::vector<int>>& max_resources)
        : available(resources), max(max_resources) {
        int process_count = max_resources.size();
        allocation = std::vector<std::vector<int>>(process_count, std::vector<int>(resources.size(), 0));
        need = max_resources;
    }

    bool is_safe(int process, const std::vector<int>& request) {
        std::vector<int> temp_available = available;
        std::vector<std::vector<int>> temp_allocation = allocation;
        std::vector<std::vector<int>> temp_need = need;

        for (size_t i = 0; i < request.size(); ++i) {
            temp_available[i] -= request[i];
            temp_allocation[process][i] += request[i];
            temp_need[process][i] -= request[i];
        }

        std::vector<bool> finish(max.size(), false);
        bool found;

        do {
            found = false;
            for (size_t i = 0; i < max.size(); ++i) {
                if (!finish[i] && std::equal(temp_need[i].begin(), temp_need[i].end(), temp_available.begin(), temp_available.end(), [](int a, int b) { return a <= b; })) {
                    for (size_t j = 0; j < temp_available.size(); ++j) {
                        temp_available[j] += temp_allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                }
            }
        } while (found);

        return std::all_of(finish.begin(), finish.end(), [](bool a) { return a; });
    }

    bool request_resources(int process, const std::vector<int>& request) {
        std::unique_lock<std::mutex> lock(mtx);
        if (!is_safe(process, request)) {
            lock.unlock();
            return false;
        }

        for (size_t i = 0; i < request.size(); ++i) {
            available[i] -= request[i];
            allocation[process][i] += request[i];
            need[process][i] -= request[i];
        }
        lock.unlock();
        return true;
    }

    void release_resources(int process) {
        std::unique_lock<std::mutex> lock(mtx);
        for (size_t i = 0; i < available.size(); ++i) {
            available[i] += allocation[process][i];
            allocation[process][i] = 0;
            need[process][i] = max[process][i];
        }
        lock.unlock();
    }
};

void atm_withdrawal(Bank& bank, int process, const std::vector<int>& request) {
    if (bank.request_resources(process, request)) {
        {
            std::unique_lock<std::mutex> lock(print_mtx);
            std::cout << "Process
