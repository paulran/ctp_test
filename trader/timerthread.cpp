#include "common.h"
#include "timerthread.h"
#include "logger.h"

CTimerThread::CTimerThread(CThostFtdcTraderApi *tapi, CTraderSpi *spi, const string &exchangeID,
                           const string &instrumentID, int volume)
    : tapi_(tapi), spi_(spi), exchangeID_(exchangeID), instrumentID_(instrumentID), volume_(volume)
{
}

CTimerThread::~CTimerThread()
{
    Stop();
    Join();
}

void CTimerThread::Start()
{
    running_ = true;
    timerThread_ = std::thread(&CTimerThread::Run, this);
}

void CTimerThread::Stop()
{
    running_ = false;
}

void CTimerThread::Join()
{
    if (timerThread_.joinable())
    {
        timerThread_.join();
    }
}

void CTimerThread::Run()
{
    // TODO: 配置这些参数，或者从外部传入
    TThostFtdcExchangeIDType exchangeID = {0};
    // strncpy(exchangeID, "DCE", sizeof(exchangeID) - 1); // 示例交易所代码，实际使用时应根据需要设置
    strncpy(exchangeID, exchangeID_.c_str(), sizeof(exchangeID) - 1);
    TThostFtdcInstrumentIDType instrumentID = {0};
    // strncpy(instrumentID, "a2605", sizeof(instrumentID) - 1); // 示例合约代码，实际使用时应根据需要设置
    strncpy(instrumentID, instrumentID_.c_str(), sizeof(instrumentID) - 1);
    TThostFtdcExchangeIDType emptyExchangeID = {0};
    TThostFtdcInstrumentIDType emptyInstrumentID = {0};
    TThostFtdcVolumeType volume = volume_;  // 示例数量，实际使用时应根据需要设置
    TThostFtdcPriceType buyPrice = 4660.1;  // 示例买入价格，实际使用时应根据需要设置
    TThostFtdcPriceType sellPrice = 4661.1; // 示例卖出价格，实际使用时应根据需要设置

    bool initFromLocalFile = true; // 是否根据本地文件初始化
    bool queryInstrumentSent = false;
    bool queryPositionSent = false;
    bool queryOrderSent = false;
    bool insertOpenOrderSent = false;
    bool insertCloseOrderSent = false;
    bool cancelOrderSent = false;
    TThostFtdcOrderRefType orderRef = {0};
    TThostFtdcOrderActionRefType orderActionRef = 0;

    // 定义新的变量记录最近运行的时间戳, 如果当前时间与上次运行的时间差超过设定的间隔, 则执行定时任务
    auto lastRunTime = std::chrono::steady_clock::now();
    while (running_)
    {
        if (!spi_->isLoggedIn() || !spi_->isSettlementInfoConfirm())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待登录成功
            continue;
        }

        if (!queryInstrumentSent)
        {
            int ret = spi_->ReqQryInstrument(exchangeID, instrumentID);
            if (ret != 0)
            {
                LogError("Failed to send query instrument request, error code: {}", ret);
            }
            else
            {
                LogInfo("Sent query instrument request successfully.");
            }
            queryInstrumentSent = true;
        }

        if (!initFromLocalFile && !spi_->isInitialized())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待合约信息查询完成
            continue;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRunTime).count();
        if (elapsed >= intervalMs_)
        {
            // 执行定时任务
            if (!queryPositionSent)
            {
                int ret = spi_->ReqQryInvestorPosition(exchangeID, instrumentID);
                if (ret != 0)
                {
                    LogError("Failed to send query investor position request, error code: {}", ret);
                    std::this_thread::sleep_for(std::chrono::microseconds(1000));
                }
                else
                {
                    LogInfo("Sent query investor position request successfully.");
                    queryPositionSent = true;
                }
            }
            else if (!insertOpenOrderSent)
            {
                int ret = spi_->ReqOrderInsert(exchangeID, instrumentID, THOST_FTDC_D_Buy, buyPrice, volume,
                                               THOST_FTDC_OF_Open, orderRef);
                if (ret != 0)
                {
                    LogError("Failed to send insert open order request, error code: {}", ret);
                }
                else
                {
                    LogInfo("Sent insert open order request successfully, order ref: {}.", orderRef);
                }
                insertOpenOrderSent = true;
            }
            else if (!insertCloseOrderSent)
            {
                int ret = spi_->ReqOrderInsert(exchangeID, instrumentID, THOST_FTDC_D_Sell, sellPrice, volume,
                                               THOST_FTDC_OF_Close, orderRef);
                if (ret != 0)
                {
                    LogError("Failed to send insert close order request, error code: {}", ret);
                }
                else
                {
                    LogInfo("Sent insert close order request successfully, order ref: {}.", orderRef);
                }
                insertCloseOrderSent = true;
            }
            else if (!cancelOrderSent)
            {
                // 先查询所有报单，找到之前插入的报单，然后发送撤单请求
                if (!queryOrderSent)
                {
                    int ret = spi_->ReqQryOrder(exchangeID, instrumentID);
                    if (ret != 0)
                    {
                        LogError("Failed to send query order request, error code: {}", ret);
                    }
                    else
                    {
                        LogInfo("Sent query order request successfully.");
                    }
                    queryOrderSent = true;
                }
                // 这里假设之前插入的报单的OrderRef就是orderRef变量
                if (orderRef[0] != '\0')
                {
                    int ret = spi_->ReqOrderAction(exchangeID, instrumentID, orderRef, orderActionRef);
                    if (ret != 0)
                    {
                        LogError("Failed to send cancel order request, error code: {}", ret);
                    }
                    else
                    {
                        LogInfo("Sent cancel order request successfully.");
                    }
                    cancelOrderSent = true;
                }
            }

            // 更新最近运行的时间戳
            lastRunTime = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 避免CPU占用过高
    }
}
