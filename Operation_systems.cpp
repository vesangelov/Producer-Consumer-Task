#include <iostream>
#include <stdexcept>
#include <pthread.h>
#include <queue>

const size_t MAXCOUNT = 10;
const size_t QUEUELIMIT = 5;

struct shared_resource {
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_full;
    pthread_cond_t cond_not_empty;
    std::queue<int> queue;
    bool done = false;
};

void* producer_fn(void* arg) {
    auto& res = *static_cast<shared_resource*>(arg);
    bool done = false;
    size_t counter = 0;
    while (!done) {
        if (pthread_mutex_lock(&res.mutex)) {
            throw std::runtime_error("Failed to lock mutex");
        }
        while (res.queue.size() >= QUEUELIMIT) {
            if (pthread_cond_wait(&res.cond_not_full, &res.mutex)) {
                throw std::runtime_error("Failed to wait on condition");
            }
        }

        res.queue.push(rand());
        if (counter++ > MAXCOUNT) done = res.done = true;
        if (pthread_cond_signal(&res.cond_not_empty)) {
            throw std::runtime_error("Failed to signal condition");
        }
        if (pthread_mutex_unlock(&res.mutex)) {
            throw std::runtime_error("Failed to unlock mutex");
        }
    }
    return nullptr;
}

void* consumer_fn(void* arg) {
    auto& res = *static_cast<shared_resource*>(arg);
    bool done = false;
    while (!done) {
        if (pthread_mutex_lock(&res.mutex)) {
            throw std::runtime_error("Failed to lock mutex");
        }
        while (res.queue.empty()) {
            if (pthread_cond_wait(&res.cond_not_empty, &res.mutex)) {
                throw std::runtime_error("Failed to wait on condition");
            }
        }
        std::cout << res.queue.front() << "\n";
        res.queue.pop();
        if (res.done && res.queue.empty()) done = true;
        if (pthread_cond_signal(&res.cond_not_full)) {
            throw std::runtime_error("Failed to signal condition");
        }
        if (pthread_mutex_unlock(&res.mutex)) {
            throw std::runtime_error("Failed to unlock mutex");
        }
    }
    return nullptr;
}

int main() {

    shared_resource res;
    pthread_t producer, consumer;

    if (pthread_mutex_init(&res.mutex, nullptr)) {
        throw std::runtime_error("Failed to create mutex");
    }

    if (pthread_cond_init(&res.cond_not_full, nullptr)) {
        throw std::runtime_error("Failed to create condition variable");
    }

    if (pthread_cond_init(&res.cond_not_empty, nullptr)) {
        throw std::runtime_error("Failed to create condition variable");
    }

    if (pthread_create(&producer, nullptr, producer_fn, &res)) {
        throw std::runtime_error("Failed to create thread");
    }

    if (pthread_create(&consumer, nullptr, consumer_fn, &res)) {
        throw std::runtime_error("Failed to create thread");
    }

    if (pthread_join(producer, nullptr)) {
        throw std::runtime_error("Failed to join thread");
    }

    if (pthread_join(consumer, nullptr)) {
        throw std::runtime_error("Failed to join thread");
    }

    if (pthread_cond_destroy(&res.cond_not_full)) {
        throw std::runtime_error("Failed to destroy condition variable");
    }

    if (pthread_cond_destroy(&res.cond_not_empty)) {
        throw std::runtime_error("Failed to destroy condition variable");
    }

    if (pthread_mutex_destroy(&res.mutex)) {
        throw std::runtime_error("Failed to destroy mutex");
    }

}