/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef UAOBBWWGHGHHWEF_VN_SWAP_OBJECT_H_
#define UAOBBWWGHGHHWEF_VN_SWAP_OBJECT_H_

#include <atomic>
#include <memory>

namespace vn {

template <typename T>
class swap_object {

    enum class status { stale, fresh, busy };

    std::atomic<status> status_flag_;

    T local_object_;

public:
    swap_object() : status_flag_{status::stale}
    {
    }

    swap_object(T t) : local_object_(std::move(t)), status_flag_{status::fresh}
    {
    }

    swap_object(swap_object const&) = delete;
    swap_object& operator=(swap_object const&) = delete;

    void swap_in(T& v)
    {
        status test_flag_{status::fresh};
        while (!status_flag_.compare_exchange_weak(test_flag_, status::busy, std::memory_order_release))
        {
            if (test_flag_ == status::busy)
            {
                test_flag_ = status::fresh;
            }
        }
        // status_flag_ is now busy..
        using std::swap;
        swap(local_object_, v);
        status_flag_.store(status::fresh, std::memory_order_release);
        // status_flag_ is now fresh..
    }

    bool swap_out(T& v)
    {
        status test_flag_{status::fresh};
        if (status_flag_.compare_exchange_strong(test_flag_, status::busy, std::memory_order_release))
        {
            // status_flag_ is now busy..
            using std::swap;
            swap(local_object_, v);
            status_flag_.store(status::stale, std::memory_order_release);
            // status_flag_ is now fresh..
            return true; // swap occurred
        }
        return false; // swap did not occur
    }
};

} // namespace
#endif