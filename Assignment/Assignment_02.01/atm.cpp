#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include<vector>

using namespace std;

// Số lượng máy ATM
const int num_atms = 1;

// Số lượng khách hàng
const int num_customers = 10;

// Thời gian giữa các giao dịch
const int transaction_time = 500;

// Semaphore và Mutex cho máy ATM
std::mutex atm_mutex;
std::condition_variable atm_cv;
int atm_counter = num_atms;

// Semaphore và Mutex cho khách hàng
std::mutex customer_mutex;
std::condition_variable customer_cv;
int customer_counter = num_customers;

void atm(int id)
{
    while (true)
    {
        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng máy ATM
        std::unique_lock<std::mutex> lock(atm_mutex);

        // Chờ Semaphore có giá trị đếm lớn hơn 0
        atm_cv.wait(lock, [] { return atm_counter > 0; });

        // Giảm giá trị đếm của Semaphore
        atm_counter--;

        // Giải phóng Semaphore
        atm_cv.notify_all();

        // Thực hiện giao dịch
        cout << "ATM " << id << " is processing transaction" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(transaction_time));

        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng máy ATM
        std::unique_lock<std::mutex> lock2(atm_mutex);

        // Tăng giá trị đếm của Semaphore
        atm_counter++;

        // Giải phóng Semaphore
        atm_cv.notify_all();
    }
}

void customer(int id)
{
    while (true)
    {
        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng khách hàng
        std::unique_lock<std::mutex> lock(customer_mutex);

        // Chờ Semaphore có giá trị đếm lớn hơn 0
        customer_cv.wait(lock, [] { return customer_counter > 0; });

        // Giảm giá trị đếm của Semaphore
        customer_counter--;

        // Giải phóng Semaphore
        customer_cv.notify_all();

        // Chờ cho máy ATM khả dụng
        std::unique_lock<std::mutex> lock2(atm_mutex);
        atm_cv.wait(lock2, [] { return atm_counter > 0; });

        // Giảm giá trị đếm của Semaphore
        atm_counter--;

        // Giải phóng Semaphore
        atm_cv.notify_all();

        // Thực hiện giao dịch tại máy ATM
        cout << "Customer " << id << " is at the ATM" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(transaction_time));

        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng khách hàng
        std::unique_lock<std::mutex> lock3(customer_mutex);

        // Tăng giá trị đếm của Semaphore
        customer_counter++;

        // Giải phóng Semaphore
        lock3.unlock();
        customer_cv.notify_all();

        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng máy ATM
        std::unique_lock<std::mutex> lock4(atm_mutex);

        // Tăng giá trị đếm của Semaphore
        atm_counter++;

        // Giải phóng Semaphore
        lock4.unlock();
        atm_cv.notify_all();
    }
}

int main() {
    std::vector<std::thread> atm_threads;
    for (int i = 1; i <= num_atms; i++)
    {
        atm_threads.emplace_back(atm, i);
    }
    // Tạo các luồng cho khách hàng
    std::vector<std::thread> customer_threads;
    for (int i = 1; i <= num_customers; i++)
    {
        customer_threads.emplace_back(customer, i);
    }

    // Chờ cho tất cả các luồng hoàn thành
    for (auto& thread : atm_threads)
    {
        thread.join();
    }
    for (auto& thread : customer_threads)
    {
        thread.join();
    }

    return 0;

}


