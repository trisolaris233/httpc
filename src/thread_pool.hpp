#pragma once

#include <mutex>
#include <queue>
#include <vector>
#include <atomic>
#include <future>
#include <thread>
#include <memory>
#include <functional>
#include <type_traits>

namespace httpc {

    class ThreadPool {
    public:
        using TaskType = std::function<void()>;

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        ThreadPool(std::size_t num_of_thread) : 
            num_of_thread_(num_of_thread),
            num_of_thread_free_(num_of_thread_) {
            
            for (std::size_t i = 0; i < this->num_of_thread_; ++i) {
                thread_.emplace_back(std::make_shared<std::thread>([this](){
                    // wait until the thread pool is set to stop
                    while (!this->stopped_atomic_mutex_) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock { this->tasks_mutex_ };
                            this->cv_task_.wait(
                                lock, [this] {
                                    return (this->stopped_atomic_mutex_) ||
                                         (this->num_of_thread_free_ > 0 && !this->tasks_.empty());
                                }
                            );
                            // if there's no task and the stop flag is set
                            if (this->tasks_.empty() && this->stopped_atomic_mutex_) {
                                return;
                            }
                            task = std::move(this->tasks_.front());
                            this->tasks_.pop();
                        }
                        // decrease the number of free thread
                        this->num_of_thread_free_--;
                        task();
                        // increase the number of free thread
                        this->num_of_thread_free_++;
                    }
                }));
            }
        }

        ~ThreadPool() noexcept {
            this->stopped_atomic_mutex_ = true;
            this->cv_task_.notify_all();
            for (auto thread : this->thread_) {
                if (thread->joinable()) {
                    thread->join();
                }
            }
        }

        template <typename Function, typename... Args>
        auto AddTask(Function&& func, Args&&... args) {
            using ResultType = decltype(func(std::forward<Args>(args)...));
            // bind a function that could run async
            auto task = std::make_shared<std::packaged_task<ResultType()>>(
                std::bind(std::forward<Function>(func), std::forward<Args>(args)...)
            );
            // get the result future
            std::future<ResultType> res = task->get_future();
            // push the task into the thread pool
            {
                std::lock_guard<std::mutex> guard(this->tasks_mutex_);
                this->tasks_.emplace(
                    [task]() {
                        (*task)();
                    });
            }
            this->cv_task_.notify_one();

            return res;
        }

    private:
        std::size_t     num_of_thread_;
        std::vector<std::shared_ptr<std::thread>>
                        thread_;
        std::queue<TaskType>
                        tasks_;
        std::mutex      tasks_mutex_;
        mutable std::atomic<bool>
                        stopped_atomic_mutex_{false};
        mutable std::atomic<int>
                        num_of_thread_free_;
        std::condition_variable
                        cv_task_;
        
        

    };

}