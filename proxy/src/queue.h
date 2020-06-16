//
// Created by Lloyd Brown on 9/28/19.
//

#ifndef PANCAKE_QUEUE_H
#define PANCAKE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class queue {
public:

    /*
    queue (const queue &q) : m_queue_(), m_mtx_(), m_cond_() {
        m_queue_ = q.m_queue_;
        m_mtx_ = std::move(q.m_mtx_);
        m_cond_ = q.m_cond_;
    }
     */

    T pop() {
        std::unique_lock<std::mutex> mlock(m_mtx_);
        while (m_queue_.empty()) {
            m_cond_.wait(mlock);
        }
        size_--;
        auto item = std::move(m_queue_.front());
        m_queue_.pop();
        return item;
    }

    void pop(T &item) {
        std::unique_lock<std::mutex> mlock(m_mtx_);
        while (m_queue_.empty()) {
            m_cond_.wait(mlock);
        };
        size_--;
        item = m_queue_.front();
        m_queue_.pop();
    }

    void push(const T &item) {
        std::unique_lock<std::mutex> mlock(m_mtx_);
        m_queue_.push(item);
        size_++;
        mlock.unlock();
        this->m_cond.notify_one();
    }

    void push(T &&item) {
        std::unique_lock<std::mutex> mlock(m_mtx_);
        m_queue_.push(std::move(item));
        size_++;
        mlock.unlock();
        m_cond_.notify_one();
    }

    int size() {
        return size_;
    }

private:
    std::queue<T> m_queue_;
    std::mutex m_mtx_;
    std::condition_variable m_cond_;
    std::atomic<int> size_{0};
};

#endif //PANCAKE_QUEUE_H
