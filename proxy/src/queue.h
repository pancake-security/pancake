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
    T pop();

    void pop(T &item);

    void push(const T &item);

    void push(T &&item);

    int size();

private:
    std::queue<T> m_queue_;
    std::mutex m_mtx_;
    std::condition_variable m_cond_;
    int size_ = 0;
};

#endif //PANCAKE_QUEUE_H
