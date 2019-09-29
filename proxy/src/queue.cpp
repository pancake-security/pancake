//
// Created by Lloyd Brown on 9/28/19.
//

#include "queue.h"

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
T queue::pop() {
    std::unique_lock<std::mutex> mlock(m_mtx);
    while (m_queue.empty()) {
        m_cond.wait(mlock);
    }
    size--;
    auto item = std::move(m_queue.front());
    m_queue.pop();
    return item;
}

void queue::pop(T &item) {
    std::unique_lock<std::mutex> mlock(m_mtx);
    while (m_queue.empty()) {
        m_cond.wait(mlock);
    };
    size--;
    item = m_queue.front();
    m_queue.pop();
}

void queue::push(const T &item) {
    std::unique_lock<std::mutex> mlock(m_mtx);
    m_queue.push(item);
    size++;
    mlock.unlock();
    m_cond.notify_one();
}

void queue::push(T &&item) {
    std::unique_lock<std::mutex> mlock(m_mtx);
    m_queue.push(std::move(item));
    size++;
    mlock.unlock();
    m_cond.notify_one();
}

int size() {
    return size_;
}

};