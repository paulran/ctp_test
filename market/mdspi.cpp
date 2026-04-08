#include "mdspi.h"
#include "logger.h"

CMdSpi::CMdSpi(CThostFtdcMdApi *api, const string &brokerID, const string &userID, const string &password, const vector<string> &instrumentIDs)
    : api_(api), brokerID_(brokerID), userID_(userID), password_(password), instrumentIDs_(instrumentIDs)
{
    ppInstrumentID_.reserve(instrumentIDs_.size());
    for (size_t i = 0; i < instrumentIDs_.size(); ++i)
    {
        ppInstrumentID_[i] = const_cast<char *>(instrumentIDs_[i].c_str());
    }
}

/// 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CMdSpi::OnFrontConnected()
{
    LogInfo("Connected to market front.");
    // login
    CThostFtdcReqUserLoginField req = {0};
    strncpy(req.BrokerID, brokerID_.c_str(), sizeof(req.BrokerID) - 1);
    strncpy(req.UserID, userID_.c_str(), sizeof(req.UserID) - 1);
    strncpy(req.Password, password_.c_str(), sizeof(req.Password) - 1);
    int ret = api_->ReqUserLogin(&req, ++requestId_);
    if (ret)
    {
        LogError("Failed to send login request, error code: {}", ret);
    }
    else
    {
        LogInfo("Success to send login request.");
    }
}

/// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
///@param nReason 错误原因
///         0x1001 网络读失败
///         0x1002 网络写失败
///         0x2001 接收心跳超时
///         0x2002 发送心跳失败
///         0x2003 收到错误报文
void CMdSpi::OnFrontDisconnected(int nReason)
{
    LogWarn("Disconnected from market front, reason: {}", nReason);
    loggedIn_.store(false, std::memory_order_release);
}

/// 心跳超时警告。当长时间未收到报文时，该方法被调用。
///@param nTimeLapse 距离上次接收报文的时间
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
    LogWarn("Heartbeat warning, time lapse since last message: {} seconds", nTimeLapse);
}

/// 登录请求响应
void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Login failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        loggedIn_.store(false, std::memory_order_release);
    }
    else
    {
        LogInfo("Login successful. Trading day: {}, Login time: {}", pRspUserLogin->TradingDay, pRspUserLogin->LoginTime);
        loggedIn_.store(true, std::memory_order_release);
        // subscribe
        int ret = api_->SubscribeMarketData(ppInstrumentID_.data(), ppInstrumentID_.size());
        if (ret)
        {
            LogError("Failed to subscribe market data, error: {}", ret);
        }
    }
}

/// 登出请求响应
void CMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Logout failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Logout successful.");
        loggedIn_.store(false, std::memory_order_release);
    }
}

/// 请求查询组播合约响应
void CMdSpi::OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query multicast instrument failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (!pMulticastInstrument)
    {
        LogWarn("Query multicast instrument successful, but multicast instrument is null.");
    }
    else
    {
        LogInfo("Query multicast instrument successful. InstrumentID: {}, InstrumentNo: {}, CodePrice: {}, VolumeMultiple: {}, PriceTick: {}",
                pMulticastInstrument->InstrumentID, pMulticastInstrument->InstrumentNo, pMulticastInstrument->CodePrice, pMulticastInstrument->VolumeMultiple, pMulticastInstrument->PriceTick);
    }
}

/// 错误应答
void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo)
    {
        LogError("Error response received, error code: {}, error message: {}, request ID: {}, is last: {}",
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
    }
    else
    {
        LogError("Error response received with no details, request ID: {}, is last: {}", nRequestID, bIsLast);
    }
}

/// 订阅行情应答
void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Subscribe market data failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (!pSpecificInstrument)
    {
        LogWarn("Subscribe market data successful, but specific instrument is null.");
    }
    else
    {
        LogInfo("Subscribe market data successful. InstrumentID: {}", pSpecificInstrument->InstrumentID);
    }
}

/// 取消订阅行情应答
void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Unsubscribe market data failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (!pSpecificInstrument)
    {
        LogWarn("Unsubscribe market data successful, but specific instrument is null.");
    }
    else
    {
        LogInfo("Unsubscribe market data successful. InstrumentID: {}", pSpecificInstrument->InstrumentID);
    }
}

/// 订阅询价应答
void CMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Subscribe for quote failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (!pSpecificInstrument)
    {
        LogWarn("Subscribe for quote successful, but specific instrument is null.");
    }
    else
    {
        LogInfo("Subscribe for quote successful. InstrumentID: {}", pSpecificInstrument->InstrumentID);
    }
}

/// 取消订阅询价应答
void CMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Unsubscribe for quote failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (!pSpecificInstrument)
    {
        LogWarn("Unsubscribe for quote successful, but specific instrument is null.");
    }
    else
    {
        LogInfo("Unsubscribe for quote successful. InstrumentID: {}", pSpecificInstrument->InstrumentID);
    }
}

/// 深度行情通知
void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    LogDebug("Depth notification received. TradingDay: {}, UpdateTime: {}, UpdateMillisec: {}, ExchangeID: {}, InstrumentID: {}, "
             "LastPrice: {}, BidPrice1: {}, AskPrice1: {}",
             pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec,
             pDepthMarketData->ExchangeID, pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice, pDepthMarketData->BidPrice1, pDepthMarketData->AskPrice1);
}

/// 询价通知
void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
    // ForQuoteSysID：交易所给的一个询价编号，以此定位一笔询价。
    LogDebug("For quote received. TradingDay: {}, ForQuoteTime: {}, ForQuoteSysID: {}, ExchangeID: {}, InstrumentID: {}",
             pForQuoteRsp->TradingDay, pForQuoteRsp->ForQuoteTime, pForQuoteRsp->ForQuoteSysID,
             pForQuoteRsp->ExchangeID, pForQuoteRsp->InstrumentID);
}