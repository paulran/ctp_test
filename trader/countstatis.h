#pragma once

#include "common.h"
#include <chrono>
#include <vector>

class CountStatis
{
public:
    CountStatis(std::size_t max_count) : max_count_(max_count), times_(max_count), times_count_(0), times_head_(0), times_tail_(0)
    {
    }

    // 增加一次请求
    void increase()
    {
        times_tail_ = (times_tail_ + 1) % times_.size();
        if (times_count_ < times_.size())
        {
            ++times_count_;
        }
        else
        {
            times_head_ = (times_head_ + 1) % times_.size();
        }
        times_[times_tail_] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    // 更新1秒内发送请求的次数
    std::size_t update()
    {
        auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        while (times_count_ > 0 && now - times_[times_head_] > 1000000000) // 1s
        {
            times_head_ = (times_head_ + 1) % times_.size();
            --times_count_;
        }
        return times_count_;
    }

    // 更新1秒内发送请求的次数并判断是否超过限制
    bool updateAndCheck()
    {
        return update() < max_count_;
    }

private:
    std::size_t max_count_;       // 1秒内最大发送的请求次数
    std::vector<uint64_t> times_; // 请求时间戳
    std::size_t times_count_;     // 请求时间戳数量
    std::size_t times_head_;      // 请求时间戳头部
    std::size_t times_tail_;      // 请求时间戳尾部
};
