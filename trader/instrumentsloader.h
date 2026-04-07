#pragma once

#include "common.h"
#include "ThostFtdcUserApiStruct.h"

// 单例类，负责加载合约信息，并提供查询接口
class CInstrumentsLoader
{
public:
    static CInstrumentsLoader &Instance()
    {
        static CInstrumentsLoader instance;
        return instance;
    }

    int Load(const string &instrumentsFile);
    int Save();
    
    int AddInstrument(CThostFtdcInstrumentField *pInstrument);
    std::shared_ptr<CThostFtdcInstrumentField> GetInstrument(const string &exchangeID, const string &instrumentID) const;

private:
    CInstrumentsLoader() = default;
    ~CInstrumentsLoader() = default;

private:
    string instrumentsFile_;
    std::unordered_map<string, std::unordered_map<string, std::shared_ptr<CThostFtdcInstrumentField>>> instrumentsByExchange_; // ExchangeID -> InstrumentID -> InstrumentField
};
