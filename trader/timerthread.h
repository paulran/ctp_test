#pragma once

#include "traderspi.h"
#include <atomic>
#include <thread>

class CTimerThread
{
public:
    CTimerThread(CThostFtdcTraderApi *tapi, CTraderSpi *spi);
    ~CTimerThread();

    void Start();
    void Stop();
    void Join();

private:
    void Run();

private:
    CThostFtdcTraderApi *tapi_ = nullptr;
    CTraderSpi *spi_ = nullptr;

    std::thread timerThread_;
    std::atomic<bool> running_{false};
    const int intervalMs_ = 100; // 时间间隔，单位为毫秒
};
