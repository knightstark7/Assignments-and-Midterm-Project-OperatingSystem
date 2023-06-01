#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <vector>
#include <queue>

using namespace std;

// Số lượng sản phẩm trong kho
const int num_products = 10;

// Thời gian sản xuất một sản phẩm
const int production_time = 500;

// Thời gian tiêu thụ một sản phẩm
const int consumption_time = 1000;

// Semaphore và Mutex cho sản phẩm
std::mutex product_mutex;
std::condition_variable product_cv;
int product_counter = 0;

// Queue cho kho
std::queue<int> product_queue;

void producer(int id)
{
    while (true)
    {
        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng sản phẩm
        std::unique_lock<std::mutex> lock(product_mutex);

        // Chờ Semaphore có giá trị đếm nhỏ hơn số lượng sản phẩm
        product_cv.wait(lock, [] { return product_counter < num_products; });

        // Tăng giá trị đếm của Semaphore
        product_counter++;

        // Thêm sản phẩm vào kho
        product_queue.push(id);

        // Giải phóng Semaphore
        product_cv.notify_all();

        // Thực hiện sản xuất sản phẩm
        cout << "Producer " << id << " produced a product" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(production_time));
    }
}

void consumer(int id)
{
    while (true)
    {
        // Khởi tạo Semaphore với giá trị đếm ban đầu là số lượng sản phẩm
        std::unique_lock<std::mutex> lock(product_mutex);

        // Chờ Semaphore có giá trị đếm lớn hơn 0
        product_cv.wait(lock, [] { return product_counter > 0; });

        // Giảm giá trị đếm của Semaphore
        product_counter--;

        // Lấy sản phẩm từ kho
        int product_id = product_queue.front();
        product_queue.pop();

        // Giải phóng Semaphore
        product_cv.notify_all();

        // Thực hiện tiêu thụ sản phẩm
        cout << "Consumer " << id << " consumed product " << product_id << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(consumption_time));
    }
}

int main()
{
    // Tạo các luồng cho nhà sản xuất
    std::vector<std::thread> producer_threads;
    for (int i = 1; i <= num_products; i++)
    {
        producer_threads.emplace_back(producer, i);
    }

    // Tạo các luồng cho người tiêu thụ
    std::vector<std::thread> consumer_threads;
    for (int i = 1; i <= num_products; i++)
    {
        consumer_threads.emplace_back(consumer, i);
    }

    // Chờ cho tất cả các luồng hoàn thành
    for (auto& thread : producer_threads)
    {
        thread.join();
    }
    for (auto& thread : consumer_threads)
    {
        thread.join();
    }

    return 0;
}
