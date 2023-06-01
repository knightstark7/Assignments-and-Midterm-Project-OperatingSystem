#define HAVE_STRUCT_TIMESPEC
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 2

int sum = 0;
sem_t semaphore_send;
pthread_mutex_t mutex_sum;

void* threadA(void* arg)
{
    int num1;
    std::cout << "Enter number 1: ";
    std::cin >> num1;
    sem_post(&semaphore_send); // signal B to receive num1
    pthread_mutex_lock(&mutex_sum);
    sum += num1;
    pthread_mutex_unlock(&mutex_sum);
    int* ret = new int;
    *ret = 0;
    pthread_exit((void*)ret);
}

void* threadB(void* arg)
{
    int num2;
    sem_wait(&semaphore_send); // wait for num1 from A
    std::cout << "Enter number 2: ";
    std::cin >> num2;
    pthread_mutex_lock(&mutex_sum);
    sum += num2;
    pthread_mutex_unlock(&mutex_sum);
    std::cout << "Sum: " << sum << std::endl;
    int* ret = new int;
    *ret = 0;
    pthread_exit((void*)ret);
}

int main(int argc, char* argv[])
{
    pthread_t threads[NUM_THREADS];
    sem_init(&semaphore_send, 0, 0);
    pthread_mutex_init(&mutex_sum, NULL);

    // create threads
    pthread_create(&threads[0], NULL, threadA, NULL);
    pthread_create(&threads[1], NULL, threadB, NULL);

    // join threads
    void* ret;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], &ret);
        delete (int*)ret;
    }

    sem_destroy(&semaphore_send);
    pthread_mutex_destroy(&mutex_sum);

    return 0;
}
