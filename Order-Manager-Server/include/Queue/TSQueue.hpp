#ifndef TSQUEUE_HPP
#define TSQUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class TSQueue
{
private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;

public:
    void push(T item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(std::move(item));
        m_cond.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]
                    { return !m_queue.empty(); });
        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};

#endif
