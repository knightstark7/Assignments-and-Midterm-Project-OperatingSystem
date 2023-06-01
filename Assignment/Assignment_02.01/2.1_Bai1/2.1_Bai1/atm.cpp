#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <string>
std::mutex mtx, print_mtx;

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

    std::vector<int> get_remaining_resources() const {
        std::vector<int> remaining_resources(available.size(), 0);
        for (size_t i = 0; i < available.size(); ++i) {
            for (size_t j = 0; j < allocation.size(); ++j) {
                remaining_resources[i] += allocation[j][i];
            }
            remaining_resources[i] = available[i] - remaining_resources[i];
        }
        return remaining_resources;
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
            std::cout << "The withdrawal request " << process + 1 << " has been accepted.\n";
            lock.unlock();
        }
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::seconds(1));
        {
            std::unique_lock<std::mutex> lock(print_mtx);
            std::cout << "The withdrawal request " << process + 1 << " has been completed and resource returned.\n";
            lock.unlock();
        }
        bank.release_resources(process);
    }
    else {
        std::unique_lock<std::mutex> lock(print_mtx);
        std::cout << "The withdrawal request " << process + 1 << " is not accepted due to lack of resources\n";
        lock.unlock();
    }
}

int main() {
    // Input resources and max resources
    std::vector<int> resources;
    std::vector<std::vector<int>> max_resources;
    int num_processes;

    std::cout << "Enter the number of resource types for the bank: ";
    int num_resources;
    std::cin >> num_resources;
    std::cout << "\n";
    std::cout << "Enter the available resources for each type\n";
    for (int i = 0; i < num_resources; ++i) {
        int resource;
        std::cout << "Enter the available resources for resource's type " << i + 1 << ": ";
        std::cin >> resource;
        resources.push_back(resource);
    }
    std::cout << "\n";
    std::cout << "Enter the number of processes: ";
    std::cin >> num_processes;
    std::cout << "\n";
    std::cout << "Enter the maximum resources for each process:\n";
    for (int i = 0; i < num_processes; ++i) {
        std::vector<int> process_max;
        std::cout << "Process " << i + 1 << ": \n";
        for (int j = 0; j < num_resources; ++j) {
            int max_resource;
            std::cout << "Enter the maximum resources for resource's type " << j + 1 << ": ";
            std::cin >> max_resource;
            process_max.push_back(max_resource);
        }
        max_resources.push_back(process_max);
    }
    std::cout << "\n";
    // Initialize Bank object
    Bank bank(resources, max_resources);

    // Input requests
    std::vector<std::vector<int>> requests;
    std::cout << "Enter the requests for each process:\n";
    for (int i = 0; i < num_processes; ++i) {
        std::vector<int> process_request;
        std::cout << "Process " << i + 1 << ": \n";
        for (int j = 0; j < num_resources; ++j) {
            int request;
            std::cout << "Enter the request resource for resource's type " << j + 1 << ": ";
            std::cin >> request;
            process_request.push_back(request);
        }
        requests.push_back(process_request);
    }

    // Process requests
    std::vector<std::thread> threads;
    for (int i = 0; i < num_processes; ++i) {
        threads.emplace_back(atm_withdrawal, std::ref(bank), i, requests[i]);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
