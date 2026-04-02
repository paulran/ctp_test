#include "logger.h"
#include "traderspi.h"

CTraderSpi::CTraderSpi(CThostFtdcTraderApi *api, const string &frontAddress, const string &brokerID, const string &userID,
                       const string &userProductInfo, const string &authCode, const string &appID,
                       const string &password, const string &investorID)
    : api_(api), frontAddress_(frontAddress), brokerID_(brokerID), userID_(userID),
      userProductInfo_(userProductInfo), authCode_(authCode), appID_(appID),
      password_(password), investorID_(investorID)
{
}

/// 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CTraderSpi::OnFrontConnected()
{
    LogInfo("Connected to trading front.");
    int ret = ReqAuthenticate();
    if (ret != 0)
    {
        LogError("Failed to send authentication request, error code: {}", ret);
    }
}

/// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
void CTraderSpi::OnFrontDisconnected(int nReason)
{
    LogWarn("Disconnected from trading front, reason: {}", nReason);
}

/// 心跳超时警告。当长时间未收到报文时，该方法被调用。
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
    LogWarn("Heartbeat warning, time lapse since last message: {} seconds", nTimeLapse);
}

/// 客户端认证响应
void CTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Authentication failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Authentication successful.");
        int ret = ReqUserLogin();
        if (ret != 0)
        {
            LogError("Failed to send login request, error code: {}", ret);
        }
    }
}

/// 登录请求响应
void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Login failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Login successful. Trading day: {}, Login time: {}", pRspUserLogin->TradingDay, pRspUserLogin->LoginTime);
    }
}

/// 用户登出请求响应
void CTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Logout failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Logout successful.");
    }
}

/// 用户口令更新请求响应
void CTraderSpi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("User password update failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("User password update successful.");
    }
}

/// 资金账户口令更新请求响应
void CTraderSpi::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Trading account password update failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Trading account password update successful.");
    }
}

/// 查询用户当前支持的认证模式的回复
void CTraderSpi::OnRspUserAuthMethod(CThostFtdcRspUserAuthMethodField *pRspUserAuthMethod, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query user auth method failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query user auth method successful. Usable auth method: {}", pRspUserAuthMethod->UsableAuthMethod);
    }
}

/// 获取图形验证码请求的回复
void CTraderSpi::OnRspGenUserCaptcha(CThostFtdcRspGenUserCaptchaField *pRspGenUserCaptcha, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Failed to get user captcha, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Get user captcha successful. Captcha info: {}", pRspGenUserCaptcha->CaptchaInfo);
    }
}

/// 获取短信验证码请求的回复
void CTraderSpi::OnRspGenUserText(CThostFtdcRspGenUserTextField *pRspGenUserText, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Failed to get user text, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Get user text successful. User text sequence: {}", pRspGenUserText->UserTextSeq);
    }
}

/// 报单录入请求响应
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Order insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Order insert request accepted, order ref: {}", pInputOrder->OrderRef);
    }
}

/// 预埋单录入请求响应
void CTraderSpi::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Parked order insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Parked order insert request accepted, order ref: {}", pParkedOrder->OrderRef);
    }
}

/// 预埋撤单录入请求响应
void CTraderSpi::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Parked order action insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Parked order action insert request accepted, order ref: {}", pParkedOrderAction->OrderRef);
    }
}

/// 报单操作请求响应
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Order action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Order action request accepted, order ref: {}", pInputOrderAction->OrderRef);
    }
}

/// 查询最大报单数量响应
void CTraderSpi::OnRspQryMaxOrderVolume(CThostFtdcQryMaxOrderVolumeField *pQryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query max order volume failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query max order volume successful. Max volume: {}", pQryMaxOrderVolume->MaxVolume);
    }
}

/// 投资者结算结果确认响应
void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Settlement info confirm failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Settlement info confirm successful, settlement id: {}, confirm date: {}, confirm time: {}",
                pSettlementInfoConfirm->SettlementID, pSettlementInfoConfirm->ConfirmDate, pSettlementInfoConfirm->ConfirmTime);
    }
}

/// 删除预埋单响应
void CTraderSpi::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Remove parked order failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Remove parked order successful, order ref: {}", pRemoveParkedOrder->ParkedOrderID);
    }
}

/// 删除预埋撤单响应
void CTraderSpi::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Remove parked order action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Remove parked order action successful, order ref: {}", pRemoveParkedOrderAction->ParkedOrderActionID);
    }
}

/// 执行宣告录入请求响应
void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Exec order insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Exec order insert request accepted, exec order ref: {}", pInputExecOrder->ExecOrderRef);
    }
}

/// 执行宣告操作请求响应
void CTraderSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Exec order action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Exec order action request accepted, exec order ref: {}", pInputExecOrderAction->ExecOrderRef);
    }
}

/// 询价录入请求响应
void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("For quote insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("For quote insert request accepted, for quote ref: {}", pInputForQuote->ForQuoteRef);
    }
}

/// 报价录入请求响应
void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Quote insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Quote insert request accepted, quote ref: {}", pInputQuote->QuoteRef);
    }
}

/// 报价操作请求响应
void CTraderSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Quote action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Quote action request accepted, quote ref: {}", pInputQuoteAction->QuoteRef);
    }
}

/// 批量报单操作请求响应
void CTraderSpi::OnRspBatchOrderAction(CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Batch order action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Batch order action request accepted, request id: {}", nRequestID);
    }
}

/// 期权自对冲录入请求响应
void CTraderSpi::OnRspOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Option self close insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Option self close insert request accepted, option self close ref: {}", pInputOptionSelfClose->OptionSelfCloseRef);
    }
}

/// 期权自对冲操作请求响应
void CTraderSpi::OnRspOptionSelfCloseAction(CThostFtdcInputOptionSelfCloseActionField *pInputOptionSelfCloseAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Option self close action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Option self close action request accepted, option self close ref: {}", pInputOptionSelfCloseAction->OptionSelfCloseRef);
    }
}

/// 申请组合录入请求响应
void CTraderSpi::OnRspCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Comb action insert failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Comb action insert request accepted, comb action ref: {}", pInputCombAction->CombActionRef);
    }
}

/// 请求查询报单响应
void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query order failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query order successful. Order ref: {}, instrument: {}, order status: {}",
                pOrder->OrderRef, pOrder->InstrumentID, pOrder->OrderStatus);
    }
}

/// 请求查询成交响应
void CTraderSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query trade failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query trade successful. Trade ID: {}, order ref: {}, instrument: {}, direction: {}, price: {}, volume: {}, ",
                pTrade->TraderID, pTrade->OrderRef, pTrade->InstrumentID, pTrade->Direction, pTrade->Price, pTrade->Volume);
    }
}

/// 请求查询投资者持仓响应
void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query investor position failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query investor position successful. Instrument: {}, position: {}",
                pInvestorPosition->InstrumentID, pInvestorPosition->Position);
    }
}

/// 请求查询资金账户响应
void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query trading account failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query trading account successful. Account ID: {}, balance: {}, available: {}",
                pTradingAccount->AccountID, pTradingAccount->Balance, pTradingAccount->Available);
    }
}

/// 请求查询投资者响应
void CTraderSpi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query investor failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query investor successful. Investor ID: {}, name: {}, comm model ID: {}",
                pInvestor->InvestorID, pInvestor->InvestorName, pInvestor->CommModelID);
    }
}

/// 请求查询交易编码响应
void CTraderSpi::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query trading code failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query trading code successful. Investor ID: {}, broker ID: {}, exchange ID: {}, client ID: {}",
                pTradingCode->InvestorID, pTradingCode->BrokerID, pTradingCode->ExchangeID, pTradingCode->ClientID);
    }
}

/// 请求查询合约保证金率响应
void CTraderSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query instrument margin rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query instrument margin rate successful. Instrument: {}, long margin ratio: {}, short margin ratio: {}",
                pInstrumentMarginRate->InstrumentID, pInstrumentMarginRate->LongMarginRatioByMoney, pInstrumentMarginRate->ShortMarginRatioByMoney);
    }
}

/// 请求查询合约手续费率响应
void CTraderSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query instrument commission rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query instrument commission rate successful. Instrument: {}, open ratio: {}, close ratio: {}",
                pInstrumentCommissionRate->InstrumentID, pInstrumentCommissionRate->OpenRatioByMoney, pInstrumentCommissionRate->CloseRatioByMoney);
    }
}

/// 请求查询交易所响应
void CTraderSpi::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query exchange failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query exchange successful. Exchange ID: {}, name: {}",
                pExchange->ExchangeID, pExchange->ExchangeName);
    }
}

/// 请求查询产品响应
void CTraderSpi::OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query product failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query product successful. Product ID: {}, name: {}, exchange ID: {}",
                pProduct->ProductID, pProduct->ProductName, pProduct->ExchangeID);
    }
}

/// 请求查询合约响应
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query instrument failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query instrument successful. Instrument ID: {}, name: {}, product ID: {}",
                pInstrument->InstrumentID, pInstrument->InstrumentName, pInstrument->ProductID);
    }
}

/// 请求查询行情响应
void CTraderSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query depth market data failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query depth market data successful. Instrument ID: {}, last price: {}, bid price: {}, ask price: {}",
                pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice, pDepthMarketData->BidPrice1, pDepthMarketData->AskPrice1);
    }
}

/// 请求查询交易员报盘机响应
void CTraderSpi::OnRspQryTraderOffer(CThostFtdcTraderOfferField *pTraderOffer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query trader offer failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query trader offer successful. Exchange ID: {}, trader ID: {}, participant ID: {}",
                pTraderOffer->ExchangeID, pTraderOffer->TraderID, pTraderOffer->ParticipantID);
    }
}

/// 请求查询投资者结算结果响应
void CTraderSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query settlement info failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query settlement info successful. Settlement ID: {}, trading day: {}",
                pSettlementInfo->SettlementID, pSettlementInfo->TradingDay);
    }
}

/// 请求查询转帐银行响应
void CTraderSpi::OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query transfer bank failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query transfer bank successful. Bank ID: {}, bank name: {}",
                pTransferBank->BankID, pTransferBank->BankName);
    }
}

/// 请求查询投资者持仓明细响应
void CTraderSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query investor position detail failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query investor position detail successful. Instrument: {}, Volume: {}, open date: {}",
                pInvestorPositionDetail->InstrumentID, pInvestorPositionDetail->Volume, pInvestorPositionDetail->OpenDate);
    }
}

/// 请求查询客户通知响应
void CTraderSpi::OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query notice failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query notice successful. Sequence label: {}, content: {}", pNotice->SequenceLabel, pNotice->Content);
    }
}

/// 请求查询结算信息确认响应
void CTraderSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query settlement info confirm failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query settlement info confirm successful. Settlement ID: {}, confirm date: {}, confirm time: {}",
                pSettlementInfoConfirm->SettlementID, pSettlementInfoConfirm->ConfirmDate, pSettlementInfoConfirm->ConfirmTime);
    }
}

/// 请求查询投资者持仓明细响应
void CTraderSpi::OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query investor position combine detail failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query investor position combine detail successful. Instrument: {}, totalAmt: {}, open date: {}",
                pInvestorPositionCombineDetail->InstrumentID, pInvestorPositionCombineDetail->TotalAmt, pInvestorPositionCombineDetail->OpenDate);
    }
}

/// 查询保证金监管系统经纪公司资金账户密钥响应
void CTraderSpi::OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query CFMMC trading account key failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query CFMMC trading account key successful. Account ID: {}, key ID: {}",
                pCFMMCTradingAccountKey->AccountID, pCFMMCTradingAccountKey->KeyID);
    }
}

/// 请求查询仓单折抵信息响应
void CTraderSpi::OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query e warrant offset failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query e warrant offset successful. Instrument ID: {}, direction: {}, volume: {}",
                pEWarrantOffset->InstrumentID, pEWarrantOffset->Direction, pEWarrantOffset->Volume);
    }
}

/// 请求查询投资者品种/跨品种保证金响应
void CTraderSpi::OnRspQryInvestorProductGroupMargin(CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query investor product group margin failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query investor product group margin successful. Product Group ID: {}, frozen margin: {}, use margin: {}",
                pInvestorProductGroupMargin->ProductGroupID, pInvestorProductGroupMargin->FrozenMargin, pInvestorProductGroupMargin->UseMargin);
    }
}

/// 请求查询交易所保证金率响应
void CTraderSpi::OnRspQryExchangeMarginRate(CThostFtdcExchangeMarginRateField *pExchangeMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query exchange margin rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query exchange margin rate successful. Exchange ID: {}, instrument ID: {}, long margin ratio: {}, short margin ratio: {}",
                pExchangeMarginRate->ExchangeID, pExchangeMarginRate->InstrumentID, pExchangeMarginRate->LongMarginRatioByMoney, pExchangeMarginRate->ShortMarginRatioByMoney);
    }
}

/// 请求查询交易所调整保证金率响应
void CTraderSpi::OnRspQryExchangeMarginRateAdjust(CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query exchange margin rate adjust failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query exchange margin rate adjust successful. Instrument ID: {}, long margin ratio by money: {}, short margin ratio by money: {}",
                pExchangeMarginRateAdjust->InstrumentID, pExchangeMarginRateAdjust->LongMarginRatioByMoney, pExchangeMarginRateAdjust->ShortMarginRatioByMoney);
    }
}

/// 请求查询汇率响应
void CTraderSpi::OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query exchange rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query exchange rate successful. From currency: {}, to currency: {}, exchange rate: {}",
                pExchangeRate->FromCurrencyID, pExchangeRate->ToCurrencyID, pExchangeRate->ExchangeRate);
    }
}

/// 请求查询二级代理操作员银期权限响应
void CTraderSpi::OnRspQrySecAgentACIDMap(CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query sec agent ACID map failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query sec agent ACID map successful. Broker ID: {}, user ID: {}, account ID: {}, currency ID: {}, broker sec agent ID: {}",
                pSecAgentACIDMap->BrokerID, pSecAgentACIDMap->UserID, pSecAgentACIDMap->AccountID, pSecAgentACIDMap->CurrencyID, pSecAgentACIDMap->BrokerSecAgentID);
    }
}

/// 请求查询产品报价汇率
void CTraderSpi::OnRspQryProductExchRate(CThostFtdcProductExchRateField *pProductExchRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query product exchange rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query product exchange rate successful. Product ID: {}, exchange ID: {}, quote currency ID: {}, exchange rate: {}",
                pProductExchRate->ProductID, pProductExchRate->ExchangeID, pProductExchRate->QuoteCurrencyID, pProductExchRate->ExchangeRate);
    }
}

/// 请求查询产品组
void CTraderSpi::OnRspQryProductGroup(CThostFtdcProductGroupField *pProductGroup, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query product group failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query product group successful. Product group ID: {}, exchange ID: {}",
                pProductGroup->ProductGroupID, pProductGroup->ExchangeID);
    }
}

/// 请求查询做市商合约手续费率响应
void CTraderSpi::OnRspQryMMInstrumentCommissionRate(CThostFtdcMMInstrumentCommissionRateField *pMMInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query MM instrument commission rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query MM instrument commission rate successful. Instrument ID: {}, open ratio: {}, close ratio: {}",
                pMMInstrumentCommissionRate->InstrumentID, pMMInstrumentCommissionRate->OpenRatioByMoney, pMMInstrumentCommissionRate->CloseRatioByMoney);
    }
}

/// 请求查询做市商期权合约手续费响应
void CTraderSpi::OnRspQryMMOptionInstrCommRate(CThostFtdcMMOptionInstrCommRateField *pMMOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query MM option instrument commission rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query MM option instrument commission rate successful. Instrument ID: {}, open ratio: {}, close ratio: {}",
                pMMOptionInstrCommRate->InstrumentID, pMMOptionInstrCommRate->OpenRatioByMoney, pMMOptionInstrCommRate->CloseRatioByMoney);
    }
}

/// 请求查询报单手续费响应
void CTraderSpi::OnRspQryInstrumentOrderCommRate(CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query instrument order commission rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query instrument order commission rate successful. Instrument ID: {}, order comm by volume: {}, order action comm by volume: {}, order comm by trade: {}, order action comm by trade: {}",
                pInstrumentOrderCommRate->InstrumentID, pInstrumentOrderCommRate->OrderCommByVolume, pInstrumentOrderCommRate->OrderActionCommByVolume,
                pInstrumentOrderCommRate->OrderCommByTrade, pInstrumentOrderCommRate->OrderActionCommByTrade);
    }
}

/// 请求查询资金账户响应
void CTraderSpi::OnRspQrySecAgentTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query sec agent trading account failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query sec agent trading account successful. Account ID: {}, balance: {}, available: {}",
                pTradingAccount->AccountID, pTradingAccount->Balance, pTradingAccount->Available);
    }
}

/// 请求查询二级代理商资金校验模式响应
void CTraderSpi::OnRspQrySecAgentCheckMode(CThostFtdcSecAgentCheckModeField *pSecAgentCheckMode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query sec agent check mode failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query sec agent check mode successful. Broker ID: {}, currency ID: {}, broker sec agent ID: {}, check self account: {}",
                pSecAgentCheckMode->BrokerID, pSecAgentCheckMode->CurrencyID, pSecAgentCheckMode->BrokerSecAgentID, pSecAgentCheckMode->CheckSelfAccount);
    }
}

/// 请求查询二级代理商信息响应
void CTraderSpi::OnRspQrySecAgentTradeInfo(CThostFtdcSecAgentTradeInfoField *pSecAgentTradeInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query sec agent trade info failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query sec agent trade info successful. Broker ID: {}, broker sec agent ID: {}, investor ID: {}, long customer name: {}",
                pSecAgentTradeInfo->BrokerID, pSecAgentTradeInfo->BrokerSecAgentID, pSecAgentTradeInfo->InvestorID,
                pSecAgentTradeInfo->LongCustomerName);
    }
}

/// 请求查询期权交易成本响应
void CTraderSpi::OnRspQryOptionInstrTradeCost(CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query option instrument trade cost failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query option instrument trade cost successful. Instrument ID: {}, royalty: {}",
                pOptionInstrTradeCost->InstrumentID, pOptionInstrTradeCost->Royalty);
    }
}

/// 请求查询期权合约手续费响应
void CTraderSpi::OnRspQryOptionInstrCommRate(CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query option instrument commission rate failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query option instrument commission rate successful. Instrument ID: {}, open ratio by money: {}",
                pOptionInstrCommRate->InstrumentID, pOptionInstrCommRate->OpenRatioByMoney);
    }
}

/// 请求查询执行宣告响应
void CTraderSpi::OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query exec order failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query exec order successful. Instrument ID: {}, exec order ref: {}, offset flag: {}, volume: {}",
                pExecOrder->InstrumentID, pExecOrder->ExecOrderRef, pExecOrder->OffsetFlag, pExecOrder->Volume);
    }
}

/// 请求查询询价响应
void CTraderSpi::OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query for quote failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query for quote successful. Instrument ID: {}, for quote ref: {}, for quote status: {}",
                pForQuote->InstrumentID, pForQuote->ForQuoteRef, pForQuote->ForQuoteStatus);
    }
}

/// 请求查询报价响应
void CTraderSpi::OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query quote failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query quote successful. Instrument ID: {}, quote ref: {}, bid price: {}, ask price: {}",
                pQuote->InstrumentID, pQuote->QuoteRef, pQuote->BidPrice, pQuote->AskPrice);
    }
}

/// 请求查询期权自对冲响应
void CTraderSpi::OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query option self close failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query option self close successful. Instrument ID: {}, option self close ref: {}, volume: {}",
                pOptionSelfClose->InstrumentID, pOptionSelfClose->OptionSelfCloseRef, pOptionSelfClose->Volume);
    }
}

/// 请求查询投资单元响应
void CTraderSpi::OnRspQryInvestUnit(CThostFtdcInvestUnitField *pInvestUnit, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query invest unit failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query invest unit successful. Broker ID: {}, investor ID: {}, invest unit ID: {}",
                pInvestUnit->BrokerID, pInvestUnit->InvestorID, pInvestUnit->InvestUnitID);
    }
}

/// 请求查询组合合约安全系数响应
void CTraderSpi::OnRspQryCombInstrumentGuard(CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query comb instrument guard failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query comb instrument guard successful. Instrument ID: {}, guarant ratio: {}",
                pCombInstrumentGuard->InstrumentID, pCombInstrumentGuard->GuarantRatio);
    }
}

/// 请求查询申请组合响应
void CTraderSpi::OnRspQryCombAction(CThostFtdcCombActionField *pCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query comb action failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query comb action successful. Instrument ID: {}, comb action ref: {}, direction: {}, volume: {}",
                pCombAction->InstrumentID, pCombAction->CombActionRef, pCombAction->Direction, pCombAction->Volume);
    }
}

/// 请求查询转帐流水响应
void CTraderSpi::OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query transfer serial failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query transfer serial successful. Plate serial: {}, bank serial: {}, trade date: {}",
                pTransferSerial->PlateSerial, pTransferSerial->BankSerial, pTransferSerial->TradeDate);
    }
}

/// 请求查询银期签约关系响应
void CTraderSpi::OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        LogError("Query account register failed, error code: {}, error message: {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogInfo("Query account register successful. Bank ID: {}, bank account: {}, broker ID: {}, account ID: {}",
                pAccountregister->BankID, pAccountregister->BankAccount, pAccountregister->BrokerID, pAccountregister->AccountID);
    }
}

/// 错误应答
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

/// 报单通知
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    LogInfo("Order notification received. Instrument ID: {}, order ref: {}, direction: {}, volume: {}, order status: {}",
            pOrder->InstrumentID, pOrder->OrderRef, pOrder->Direction, pOrder->VolumeTotalOriginal, pOrder->OrderStatus);
}

/// 成交通知
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    LogInfo("Trade notification received. Instrument ID: {}, order ref: {}, direction: {}, volume: {}, price: {}",
            pTrade->InstrumentID, pTrade->OrderRef, pTrade->Direction, pTrade->Volume, pTrade->Price);
}

/// 报单录入错误回报
void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Order insert error. Instrument ID: {}, order ref: {}, direction: {}, volume: {}, price: {}, error code: {}, error message: {}",
                 pInputOrder->InstrumentID, pInputOrder->OrderRef, pInputOrder->Direction, pInputOrder->VolumeTotalOriginal, pInputOrder->LimitPrice,
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Order insert error with no details. Instrument ID: {}, order ref: {}, direction: {}, volume: {}, price: {}",
                 pInputOrder->InstrumentID, pInputOrder->OrderRef, pInputOrder->Direction, pInputOrder->VolumeTotalOriginal, pInputOrder->LimitPrice);
    }
}

/// 报单操作错误回报
void CTraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Order action error. Instrument ID: {}, order ref: {}, order action ref: {}, error code: {}, error message: {}",
                 pOrderAction->InstrumentID, pOrderAction->OrderRef, pOrderAction->OrderActionRef, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Order action error with no details. Instrument ID: {}, order ref: {}, order action ref: {}",
                 pOrderAction->InstrumentID, pOrderAction->OrderRef, pOrderAction->OrderActionRef);
    }
}

/// 合约交易状态通知
void CTraderSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
    LogInfo("Instrument status notification received. Instrument ID: {}, exchange ID: {}, instrument status: {}",
            pInstrumentStatus->InstrumentID, pInstrumentStatus->ExchangeID, pInstrumentStatus->InstrumentStatus);
}

/// 交易所公告通知
void CTraderSpi::OnRtnBulletin(CThostFtdcBulletinField *pBulletin)
{
    LogInfo("Bulletin notification received. Exchange ID: {}, bulletin ID: {}, sequence number: {}, content: {}",
            pBulletin->ExchangeID, pBulletin->BulletinID, pBulletin->SequenceNo, pBulletin->Content);
}

/// 交易通知
void CTraderSpi::OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo)
{
    LogInfo("Trading notice notification received. Notice sequence number: {}, content: {}",
            pTradingNoticeInfo->SequenceNo, pTradingNoticeInfo->FieldContent);
}

/// 提示条件单校验错误
void CTraderSpi::OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder)
{
    LogError("Error conditional order notification received. Instrument ID: {}, order ref: {}, order price type: {}, error code: {}, error message: {}",
             pErrorConditionalOrder->InstrumentID, pErrorConditionalOrder->OrderRef, pErrorConditionalOrder->OrderPriceType,
             pErrorConditionalOrder->ErrorID, pErrorConditionalOrder->ErrorMsg);
}

/// 执行宣告通知
void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
    LogInfo("Exec order notification received. Instrument ID: {}, exec order ref: {}, offset flag: {}, volume: {}",
            pExecOrder->InstrumentID, pExecOrder->ExecOrderRef, pExecOrder->OffsetFlag, pExecOrder->Volume);
}

/// 执行宣告录入错误回报
void CTraderSpi::OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Exec order insert error. Instrument ID: {}, exec order ref: {}, offset flag: {}, volume: {}, error code: {}, error message: {}",
                 pInputExecOrder->InstrumentID, pInputExecOrder->ExecOrderRef, pInputExecOrder->OffsetFlag, pInputExecOrder->Volume,
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Exec order insert error with no details. Instrument ID: {}, exec order ref: {}, offset flag: {}, volume: {}",
                 pInputExecOrder->InstrumentID, pInputExecOrder->ExecOrderRef, pInputExecOrder->OffsetFlag, pInputExecOrder->Volume);
    }
}

/// 执行宣告操作错误回报
void CTraderSpi::OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Exec order action error. Instrument ID: {}, exec order ref: {}, exec order action ref: {}, error code: {}, error message: {}",
                 pExecOrderAction->InstrumentID, pExecOrderAction->ExecOrderRef, pExecOrderAction->ExecOrderActionRef,
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Exec order action error with no details. Instrument ID: {}, exec order ref: {}, exec order action ref: {}",
                 pExecOrderAction->InstrumentID, pExecOrderAction->ExecOrderRef, pExecOrderAction->ExecOrderActionRef);
    }
}

/// 询价录入错误回报
void CTraderSpi::OnErrRtnForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("For quote insert error. Instrument ID: {}, for quote ref: {}, error code: {}, error message: {}",
                 pInputForQuote->InstrumentID, pInputForQuote->ForQuoteRef,
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("For quote insert error with no details. Instrument ID: {}, for quote ref: {}",
                 pInputForQuote->InstrumentID, pInputForQuote->ForQuoteRef);
    }
}

/// 报价通知
void CTraderSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
    LogInfo("Quote notification received. Instrument ID: {}, quote ref: {}, bid price: {}, ask price: {}",
            pQuote->InstrumentID, pQuote->QuoteRef, pQuote->BidPrice, pQuote->AskPrice);
}

/// 报价录入错误回报
void CTraderSpi::OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Quote insert error. Instrument ID: {}, quote ref: {}, bid price: {}, ask price: {}, error code: {}, error message: {}",
                 pInputQuote->InstrumentID, pInputQuote->QuoteRef, pInputQuote->BidPrice, pInputQuote->AskPrice,
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Quote insert error with no details. Instrument ID: {}, quote ref: {}, bid price: {}, ask price: {}",
                 pInputQuote->InstrumentID, pInputQuote->QuoteRef, pInputQuote->BidPrice, pInputQuote->AskPrice);
    }
}

/// 报价操作错误回报
void CTraderSpi::OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Quote action error. Instrument ID: {}, quote ref: {}, quote action ref: {}, error code: {}, error message: {}",
                 pQuoteAction->InstrumentID, pQuoteAction->QuoteRef, pQuoteAction->QuoteActionRef,
                 pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Quote action error with no details. Instrument ID: {}, quote ref: {}, quote action ref: {}",
                 pQuoteAction->InstrumentID, pQuoteAction->QuoteRef, pQuoteAction->QuoteActionRef);
    }
}

/// 询价通知
void CTraderSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
    LogInfo("For quote response notification received. Instrument ID: {}, for quote sys ID: {}, for quote time: {}",
            pForQuoteRsp->InstrumentID, pForQuoteRsp->ForQuoteSysID, pForQuoteRsp->ForQuoteTime);
}

/// 保证金监控中心用户令牌
void CTraderSpi::OnRtnCFMMCTradingAccountToken(CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken)
{
    LogInfo("CFMMC trading account token notification received. Broker ID: {}, account ID: {}, token: {}",
            pCFMMCTradingAccountToken->BrokerID, pCFMMCTradingAccountToken->AccountID, pCFMMCTradingAccountToken->Token);
}

/// 批量报单操作错误回报
void CTraderSpi::OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    if (pRspInfo)
    {
        LogError("Batch order action error. Order action ref: {}, error code: {}, error message: {}",
                 pBatchOrderAction->OrderActionRef, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        LogError("Batch order action error with no details. Order action ref: {}",
                 pBatchOrderAction->OrderActionRef);
    }
}

/// 期权自对冲通知
void CTraderSpi::OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose)
{
}

/// 期权自对冲录入错误回报
void CTraderSpi::OnErrRtnOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 期权自对冲操作错误回报
void CTraderSpi::OnErrRtnOptionSelfCloseAction(CThostFtdcOptionSelfCloseActionField *pOptionSelfCloseAction, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 申请组合通知
void CTraderSpi::OnRtnCombAction(CThostFtdcCombActionField *pCombAction)
{
}

/// 申请组合录入错误回报
void CTraderSpi::OnErrRtnCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 请求查询签约银行响应
void CTraderSpi::OnRspQryContractBank(CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求查询预埋单响应
void CTraderSpi::OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求查询预埋撤单响应
void CTraderSpi::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求查询交易通知响应
void CTraderSpi::OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求查询经纪公司交易参数响应
void CTraderSpi::OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求查询经纪公司交易算法响应
void CTraderSpi::OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求查询监控中心用户令牌
void CTraderSpi::OnRspQueryCFMMCTradingAccountToken(CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 银行发起银行资金转期货通知
void CTraderSpi::OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer)
{
}

/// 银行发起期货资金转银行通知
void CTraderSpi::OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer)
{
}

/// 银行发起冲正银行转期货通知
void CTraderSpi::OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField *pRspRepeal)
{
}

/// 银行发起冲正期货转银行通知
void CTraderSpi::OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField *pRspRepeal)
{
}

/// 期货发起银行资金转期货通知
void CTraderSpi::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer)
{
}

/// 期货发起期货资金转银行通知
void CTraderSpi::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer)
{
}

/// 系统运行时期货端手工发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
void CTraderSpi::OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField *pRspRepeal)
{
}

/// 系统运行时期货端手工发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
void CTraderSpi::OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField *pRspRepeal)
{
}

/// 期货发起查询银行余额通知
void CTraderSpi::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount)
{
}

/// 期货发起银行资金转期货错误回报
void CTraderSpi::OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 期货发起期货资金转银行错误回报
void CTraderSpi::OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 系统运行时期货端手工发起冲正银行转期货错误回报
void CTraderSpi::OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 系统运行时期货端手工发起冲正期货转银行错误回报
void CTraderSpi::OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 期货发起查询银行余额错误回报
void CTraderSpi::OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo)
{
}

/// 期货发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
void CTraderSpi::OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal)
{
}

/// 期货发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
void CTraderSpi::OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal)
{
}

/// 期货发起银行资金转期货应答
void CTraderSpi::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 期货发起期货资金转银行应答
void CTraderSpi::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 期货发起查询银行余额应答
void CTraderSpi::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 银行发起银期开户通知
void CTraderSpi::OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount)
{
}

/// 银行发起银期销户通知
void CTraderSpi::OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount)
{
}

/// 银行发起变更银行账号通知
void CTraderSpi::OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount)
{
}

/// 请求查询分类合约响应
void CTraderSpi::OnRspQryClassifiedInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 请求组合优惠比例响应
void CTraderSpi::OnRspQryCombPromotionParam(CThostFtdcCombPromotionParamField *pCombPromotionParam, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者风险结算持仓查询响应
void CTraderSpi::OnRspQryRiskSettleInvstPosition(CThostFtdcRiskSettleInvstPositionField *pRiskSettleInvstPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 风险结算产品查询响应
void CTraderSpi::OnRspQryRiskSettleProductStatus(CThostFtdcRiskSettleProductStatusField *pRiskSettleProductStatus, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPBM期货合约参数查询响应
void CTraderSpi::OnRspQrySPBMFutureParameter(CThostFtdcSPBMFutureParameterField *pSPBMFutureParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPBM期权合约参数查询响应
void CTraderSpi::OnRspQrySPBMOptionParameter(CThostFtdcSPBMOptionParameterField *pSPBMOptionParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPBM品种内对锁仓折扣参数查询响应
void CTraderSpi::OnRspQrySPBMIntraParameter(CThostFtdcSPBMIntraParameterField *pSPBMIntraParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPBM跨品种抵扣参数查询响应
void CTraderSpi::OnRspQrySPBMInterParameter(CThostFtdcSPBMInterParameterField *pSPBMInterParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPBM组合保证金套餐查询响应
void CTraderSpi::OnRspQrySPBMPortfDefinition(CThostFtdcSPBMPortfDefinitionField *pSPBMPortfDefinition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者SPBM套餐选择查询响应
void CTraderSpi::OnRspQrySPBMInvestorPortfDef(CThostFtdcSPBMInvestorPortfDefField *pSPBMInvestorPortfDef, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者新型组合保证金系数查询响应
void CTraderSpi::OnRspQryInvestorPortfMarginRatio(CThostFtdcInvestorPortfMarginRatioField *pInvestorPortfMarginRatio, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者产品SPBM明细查询响应
void CTraderSpi::OnRspQryInvestorProdSPBMDetail(CThostFtdcInvestorProdSPBMDetailField *pInvestorProdSPBMDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者商品组SPMM记录查询响应
void CTraderSpi::OnRspQryInvestorCommoditySPMMMargin(CThostFtdcInvestorCommoditySPMMMarginField *pInvestorCommoditySPMMMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者商品群SPMM记录查询响应
void CTraderSpi::OnRspQryInvestorCommodityGroupSPMMMargin(CThostFtdcInvestorCommodityGroupSPMMMarginField *pInvestorCommodityGroupSPMMMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPMM合约参数查询响应
void CTraderSpi::OnRspQrySPMMInstParam(CThostFtdcSPMMInstParamField *pSPMMInstParam, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPMM产品参数查询响应
void CTraderSpi::OnRspQrySPMMProductParam(CThostFtdcSPMMProductParamField *pSPMMProductParam, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// SPBM附加跨品种抵扣参数查询响应
void CTraderSpi::OnRspQrySPBMAddOnInterParameter(CThostFtdcSPBMAddOnInterParameterField *pSPBMAddOnInterParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RCAMS产品组合信息查询响应
void CTraderSpi::OnRspQryRCAMSCombProductInfo(CThostFtdcRCAMSCombProductInfoField *pRCAMSCombProductInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RCAMS同合约风险对冲参数查询响应
void CTraderSpi::OnRspQryRCAMSInstrParameter(CThostFtdcRCAMSInstrParameterField *pRCAMSInstrParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RCAMS品种内风险对冲参数查询响应
void CTraderSpi::OnRspQryRCAMSIntraParameter(CThostFtdcRCAMSIntraParameterField *pRCAMSIntraParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RCAMS跨品种风险折抵参数查询响应
void CTraderSpi::OnRspQryRCAMSInterParameter(CThostFtdcRCAMSInterParameterField *pRCAMSInterParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RCAMS空头期权风险调整参数查询响应
void CTraderSpi::OnRspQryRCAMSShortOptAdjustParam(CThostFtdcRCAMSShortOptAdjustParamField *pRCAMSShortOptAdjustParam, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RCAMS策略组合持仓查询响应
void CTraderSpi::OnRspQryRCAMSInvestorCombPosition(CThostFtdcRCAMSInvestorCombPositionField *pRCAMSInvestorCombPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者品种RCAMS保证金查询响应
void CTraderSpi::OnRspQryInvestorProdRCAMSMargin(CThostFtdcInvestorProdRCAMSMarginField *pInvestorProdRCAMSMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RULE合约保证金参数查询响应
void CTraderSpi::OnRspQryRULEInstrParameter(CThostFtdcRULEInstrParameterField *pRULEInstrParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RULE品种内对锁仓折扣参数查询响应
void CTraderSpi::OnRspQryRULEIntraParameter(CThostFtdcRULEIntraParameterField *pRULEIntraParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// RULE跨品种抵扣参数查询响应
void CTraderSpi::OnRspQryRULEInterParameter(CThostFtdcRULEInterParameterField *pRULEInterParameter, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/// 投资者产品RULE保证金查询响应
void CTraderSpi::OnRspQryInvestorProdRULEMargin(CThostFtdcInvestorProdRULEMarginField *pInvestorProdRULEMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

/* private */
int CTraderSpi::ReqAuthenticate()
{
    CThostFtdcReqAuthenticateField req;
    memset(&req, 0, sizeof(req));
    strncpy(req.BrokerID, brokerID_.c_str(), sizeof(req.BrokerID) - 1);
    strncpy(req.UserID, userID_.c_str(), sizeof(req.UserID) - 1);
    strncpy(req.UserProductInfo, userProductInfo_.c_str(), sizeof(req.UserProductInfo) - 1);
    strncpy(req.AuthCode, authCode_.c_str(), sizeof(req.AuthCode) - 1);
    strncpy(req.AppID, appID_.c_str(), sizeof(req.AppID) - 1);
    return api_->ReqAuthenticate(&req, ++requestId_);
}

int CTraderSpi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strncpy(req.BrokerID, brokerID_.c_str(), sizeof(req.BrokerID) - 1);
    strncpy(req.UserID, userID_.c_str(), sizeof(req.UserID) - 1);
    strncpy(req.Password, password_.c_str(), sizeof(req.Password) - 1);
    return api_->ReqUserLogin(&req, ++requestId_);
}

int CTraderSpi::ReqQryInvestorPosition()
{
    // 结算单确认
    // 请求查询结算单: ReqQrySettlementInfo
    // 请求确认结算单及: ReqSettlementInfoConfirm
    // 查询结算单确认的日期: ReqQrySettlementInfoConfirm
    // 查询持仓（汇总）: ReqQryInvestorPosition
    CThostFtdcQryInvestorPositionField qryInvestorPositionField;
    memset(&qryInvestorPositionField, 0, sizeof(qryInvestorPositionField));
    return api_->ReqQryInvestorPosition(&qryInvestorPositionField, ++requestId_);
}

int CTraderSpi::ReqOrderInsert()
{
    // 请求报单
    CThostFtdcInputOrderField inputOrderField;
    memset(&inputOrderField, 0, sizeof(inputOrderField));
    strncpy(inputOrderField.BrokerID, brokerID_.c_str(), sizeof(inputOrderField.BrokerID) - 1);
    strncpy(inputOrderField.InvestorID, investorID_.c_str(), sizeof(inputOrderField.InvestorID) - 1);
    strncpy(inputOrderField.ExchangeID, "SHFE", sizeof(inputOrderField.ExchangeID) - 1);
    strncpy(inputOrderField.InstrumentID, "ag1801", sizeof(inputOrderField.InstrumentID) - 1);
    inputOrderField.OrderPriceType = THOST_FTDC_OPT_LimitPrice;   // 限价
    inputOrderField.Direction = THOST_FTDC_D_Buy;                 // 买
    inputOrderField.CombOffsetFlag[0] = THOST_FTDC_OF_Open;       // 开
    inputOrderField.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation; // 投机
    inputOrderField.LimitPrice = 100;
    inputOrderField.VolumeTotalOriginal = 1;            // 数量
    inputOrderField.TimeCondition = THOST_FTDC_TC_GFD;  // 当日有效
    inputOrderField.VolumeCondition = THOST_FTDC_VC_AV; // 任意数量
    inputOrderField.MinVolume = 1;
    inputOrderField.ContingentCondition = THOST_FTDC_CC_Immediately;
    inputOrderField.StopPrice = 0;
    inputOrderField.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    inputOrderField.IsAutoSuspend = 0;
    return api_->ReqOrderInsert(&inputOrderField, ++requestId_);
}
