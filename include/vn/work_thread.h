#ifndef BUWOAOBOAAFAWWFAG_VN_WORK_THREAD_H_
#define BUWOAOBOAAFAWWFAG_VN_WORK_THREAD_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              
    
    namespace vn::
    
    class work_thread
-----------------------------------------------------------------------------*/

#include <atomic>
#include <condition_variable>
#include <exception>
#include <future>
#include <mutex>
#include <thread>

namespace vn {

class work_thread {

    class never_available : public std::exception {
        // work_thread::when_available throws this if the state becomes null_state (i.e., will never become available..)
    public:
        virtual const char* what() const noexcept override
        {
            return "exception: work_thread is in null_state and cannot become available";
        }
    };

    class already_consumed : public std::exception {
        // thrown if the produced state is consumed more than once
    public:
        virtual const char* what() const noexcept override
        {
            return "exception: work_thread state is already consumed";
        }
    };

    enum class sig {
        null_state,
        available,
        being_consumed,
        consumed,
    };

    std::mutex mutex_;
    std::condition_variable cv_;
    bool quit_; // the mutex guards access to quit_, so using non-atomic bool

    std::atomic<sig> state_;
    std::thread thread_;

    class work_request {
        std::atomic<sig>& state_;

    public:
        work_request(std::atomic<sig>& s) noexcept : state_{ s } {}
        work_request(work_request&&) = delete;
        work_request& operator=(work_request&&) = delete;

        void make_available(std::memory_order mo = std::memory_order_release) { state_.store(sig::available, mo); }
        friend void make_available(work_request& p, std::memory_order mo = std::memory_order_release) noexcept
        {
            p.make_available(mo);
        }
    };

    class work_ready {
        std::atomic<sig>& state_;
        std::condition_variable& cv_;
        bool state_consumed_{ false };

    public:
        work_ready(std::atomic<sig>& s, std::condition_variable& c) noexcept : state_{ s }, cv_{ c } {}
        work_ready(work_ready&&) = delete;
        work_ready& operator=(work_ready&&) = delete;

        void consume_and_notify(std::memory_order mo = std::memory_order_release)
        {
            if (state_consumed_) {
                throw already_consumed{};
            } else {
                state_consumed_ = true;
                state_.store(sig::consumed, mo);
                cv_.notify_one();
            }
        }

        friend void consume_and_notify(work_ready& p, std::memory_order mo = std::memory_order_release)
        {
            p.consume_and_notify(mo);
        }
    };

    struct scoped_state_cleanup {
        std::atomic<sig>& state_;
        ~scoped_state_cleanup()
        {
            state_.store(sig::null_state, std::memory_order_release);
        }
    };

    template <typename F, typename H>
    void dowork(F&& func, H&& exception_handler, std::promise<void> started_) noexcept
    {
        scoped_state_cleanup cleanup{ state_ };
        try {
            std::unique_lock<std::mutex> lock_{ mutex_ };
            started_.set_value();
            while (!quit_) {
                // note: cv_ may wake up on its own, so once it is in the consumed state it may begin producing
                cv_.wait(lock_, [&]() { return state_.load(std::memory_order_acquire) == sig::consumed; });
                if (quit_) {
                    return;
                } // early exit..
                work_request w{ state_ };
                func(w);
            }
        } catch (...) {
            exception_handler();
        }
    }

public:
    work_thread(work_thread&&) = delete; // no moving/copying
    work_thread& operator=(work_thread&&) = delete; // no assigning

    template <typename F, typename H>
    work_thread(F&& func, H&& exception_handler)
        : quit_{ false }
        , state_{ sig::null_state }
    {
        std::promise<void> prom_;
        std::future<void> started_ = prom_.get_future();
        thread_ = std::thread{ [this](auto&&... ps) {
                                  this->dowork(std::forward<decltype(ps)>(ps)...);
                              },
            std::forward<F>(func), std::forward<H>(exception_handler), std::move(prom_) };
        // ...
        started_.get();
        std::unique_lock<std::mutex> lock_{ mutex_ };
        // we are now synchronized with the waiting condition_variable
        state_.store(sig::consumed, std::memory_order_release);
    }

    template <typename F, typename G>
    void if_available(F&& func, G&& gunc)
    {
        sig local_ = state_.load(std::memory_order_acquire);
        if (local_ == sig::available) {
            work_ready w{ state_, cv_ };
            func(w);
        } else {
            gunc();
            if (local_ == sig::consumed) {
                cv_.notify_one();
            }
        }
    }

    template <typename F>
    void when_available(F&& func)
    {
        sig local_ = state_.load(std::memory_order_acquire);

        while (local_ != sig::available) {

            if (local_ == sig::consumed) {
                cv_.notify_one();
                std::this_thread::yield();
            } else if (local_ == sig::null_state) {
                throw never_available{};
            }

            local_ = state_.load(std::memory_order_acquire);
        }

        work_ready w{ state_, cv_ };
        func(w);
    }

    ~work_thread()
    {
        {
            std::unique_lock<std::mutex> lock{ mutex_ };
            quit_ = true;
            state_.store(sig::consumed, std::memory_order_release);
            cv_.notify_one();
        } // lock is destroyed, thread can now advance to exit..

        if (thread_.joinable()) {
            thread_.join();
        }
    }
};

} // namespace
#endif