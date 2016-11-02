/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef HWIOAOIOSAJSBASF_VN_SINGLE_CONSUMER_QUEUE_H_
#define HWIOAOIOSAJSBASF_VN_SINGLE_CONSUMER_QUEUE_H_

/*-------------------------------------------------------------
    template <typename T>
    class single_consumer_queue

        +   any thread can insert Ts
        +   a single thread can consume Ts

/*-------------------------------------------------------------*/

#include <mutex>
#include <vector>

namespace vn {

template <typename T, typename A = std::allocator<T>>
class single_consumer_queue {

    enum class status { ready, busy };

    using container_type = std::vector<T, A>;

    container_type producer_container_, consumer_container_;

    // std::mutex mut_;
    std::atomic<status> status_flag_;

    void swap_containers()
    {
        // std::lock_guard<std::mutex> lock_{mut_};
        status test_flag_{status::ready};

        while (!status_flag_.compare_exchange_weak(test_flag_, status::busy, std::memory_order_release))
        {
            if (test_flag_ == status::busy)
            {
                test_flag_ = status::ready;
            }
        }

        using std::swap;
        swap(producer_container_, consumer_container_);

        status_flag_.store(status::ready, std::memory_order_release);
    }

public:
    single_consumer_queue() = default;
    single_consumer_queue(single_consumer_queue&&) = delete;
    single_consumer_queue& operator=(single_consumer_queue&&) = delete;

    single_consumer_queue(container_type v) : producer_container_(std::move(v))
    {
    }

    void insert(T&& t)
    { // can be executed in multiple threads at once
        // std::lock_guard<std::mutex> lock_{mut_};
        status test_flag_{status::ready};

        while (!status_flag_.compare_exchange_weak(test_flag_, status::busy, std::memory_order_release))
        {
            if (test_flag_ == status::busy)
            {
                test_flag_ = status::ready;
            }
        }
        producer_container_.emplace_back(std::move(t));

        status_flag_.store(status::ready, std::memory_order_release);
    }

    template <typename F>
    void consume(F&& func)
    { // may only be executed in a single thread at a time
        swap_containers();

        for (auto&& e : consumer_container_)
        {
            func(std::move(e));
        }

        consumer_container_.clear();
    }
};
}

#endif