//
// Created by Lloyd Brown on 9/28/19.
//

#include "queue.h"
template<typename T>
T queue<T>::pop() {
    std::unique_lock<std::mutex> mlock(m_mtx_);
    while (m_queue_.empty()) {
        m_cond_.wait(mlock);
    }
    size_--;
    auto item = std::move(m_queue_.front());
    m_queue_.pop();
    return item;
}

template<typename T>
void queue<T>::pop(T &item) {
    std::unique_lock<std::mutex> mlock(m_mtx_);
    while (m_queue_.empty()) {
        m_cond_.wait(mlock);
    };
    size_--;
    item = m_queue_.front();
    m_queue_.pop();
}

template<typename T>
void queue<T>::push(const T &item) {
    std::unique_lock<std::mutex> mlock(m_mtx_);
    m_queue_.push(item);
    size_++;
    mlock.unlock();
    m_cond_.notify_one();
}

template<typename T>
void queue<T>::push(T &&item) {
    std::unique_lock<std::mutex> mlock(m_mtx_);
    m_queue_.push(std::move(item));
    size_++;
    mlock.unlock();
    m_cond_.notify_one();
}

template<typename T>
int queue<T>::size() {
    return size_;
}
