//
//  threadpool.h
//
//  Copyright (c) 2019 Yuji Hirose. All rights reserved.
//  MIT License
//

#ifndef CPPHTTPLIB_THREADPOOL_H
#define CPPHTTPLIB_THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

namespace threadpool {

class pool {
public:
  pool() : shutdown_(false) {}
  pool(const pool &) = delete;

  ~pool() {}

  void start(size_t n) {
    while (n) {
      auto t = std::make_shared<std::thread>(worker(*this));
      threads_.push_back(t);
      n--;
    }
  }

  void shutdown() {
    // Stop all worker threads...
    {
      std::unique_lock<std::mutex> lock(mutex_);
      shutdown_ = true;
    }

    cond_.notify_all();

    // Join...
    for (auto t : threads_) {
      t->join();
    }
  }

  void enqueue(std::function<void()> fn) {
    std::unique_lock<std::mutex> lock(mutex_);
    jobs_.push_back(fn);
    cond_.notify_one();
  }

private:
  struct worker {
    worker(pool &pool) : pool_(pool) {}

    void operator()() {
      for (;;) {
        std::function<void()> fn;
        {
          std::unique_lock<std::mutex> lock(pool_.mutex_);

          pool_.cond_.wait(
              lock, [&] { return !pool_.jobs_.empty() || pool_.shutdown_; });

          if (pool_.shutdown_ && pool_.jobs_.empty()) { break; }

          fn = pool_.jobs_.front();
          pool_.jobs_.pop_front();
        }

        assert(true == (bool)fn);
        fn();
      }
    }

    pool &pool_;
  };
  friend struct worker;

  std::vector<std::shared_ptr<std::thread>> threads_;
  std::list<std::function<void()>> jobs_;

  bool shutdown_;

  std::condition_variable cond_;
  std::mutex mutex_;
};

}; // namespace threadpool

#endif // CPPHTTPLIB_THREADPOOL_H
