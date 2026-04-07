#include "instrumentsloader.h"
#include "logger.h"
#include <fstream>

int CInstrumentsLoader::Load(const string &instrumentsFile)
{
    instrumentsFile_ = instrumentsFile;
    std::ifstream inFile(instrumentsFile_);
    if (!inFile.is_open())
    {
        LogError("Failed to open instruments file: {}", instrumentsFile_);
        return -1;
    }
    Json instrumentsJson;
    try
    {
        inFile >> instrumentsJson;
    }
    catch (const std::exception &e)
    {
        LogError("Failed to parse instruments file: {}, error: {}", instrumentsFile_, e.what());
        return -1;
    }
    for (const auto &exchangePair : instrumentsJson.items())
    {
        for (const auto &instrumentPair : exchangePair.value().items())
        {
            auto pInstrument = std::make_shared<CThostFtdcInstrumentField>();
            try
            {
                strncpy(pInstrument->ExchangeID, exchangePair.key().c_str(), sizeof(pInstrument->ExchangeID) - 1);
                strncpy(pInstrument->InstrumentName, instrumentPair.value()["InstrumentName"].get<string>().c_str(), sizeof(pInstrument->InstrumentName) - 1);
                pInstrument->ProductClass = instrumentPair.value()["ProductClass"].get<TThostFtdcProductClassType>();
                pInstrument->DeliveryYear = instrumentPair.value()["DeliveryYear"].get<TThostFtdcYearType>();
                pInstrument->DeliveryMonth = instrumentPair.value()["DeliveryMonth"].get<TThostFtdcMonthType>();
                pInstrument->MaxMarketOrderVolume = instrumentPair.value()["MaxMarketOrderVolume"].get<TThostFtdcVolumeType>();
                pInstrument->MinMarketOrderVolume = instrumentPair.value()["MinMarketOrderVolume"].get<TThostFtdcVolumeType>();
                pInstrument->MaxLimitOrderVolume = instrumentPair.value()["MaxLimitOrderVolume"].get<TThostFtdcVolumeType>();
                pInstrument->MinLimitOrderVolume = instrumentPair.value()["MinLimitOrderVolume"].get<TThostFtdcVolumeType>();
                pInstrument->VolumeMultiple = instrumentPair.value()["VolumeMultiple"].get<TThostFtdcVolumeMultipleType>();
                pInstrument->PriceTick = instrumentPair.value()["PriceTick"].get<TThostFtdcPriceType>();
                strncpy(pInstrument->CreateDate, instrumentPair.value()["CreateDate"].get<string>().c_str(), sizeof(pInstrument->CreateDate) - 1);
                strncpy(pInstrument->OpenDate, instrumentPair.value()["OpenDate"].get<string>().c_str(), sizeof(pInstrument->OpenDate) - 1);
                strncpy(pInstrument->ExpireDate, instrumentPair.value()["ExpireDate"].get<string>().c_str(), sizeof(pInstrument->ExpireDate) - 1);
                strncpy(pInstrument->StartDelivDate, instrumentPair.value()["StartDelivDate"].get<string>().c_str(), sizeof(pInstrument->StartDelivDate) - 1);
                strncpy(pInstrument->EndDelivDate, instrumentPair.value()["EndDelivDate"].get<string>().c_str(), sizeof(pInstrument->EndDelivDate) - 1);
                pInstrument->InstLifePhase = instrumentPair.value()["InstLifePhase"].get<TThostFtdcInstLifePhaseType>();
                pInstrument->IsTrading = instrumentPair.value()["IsTrading"].get<TThostFtdcBoolType>();
                pInstrument->PositionType = instrumentPair.value()["PositionType"].get<TThostFtdcPositionTypeType>();
                pInstrument->PositionDateType = instrumentPair.value()["PositionDateType"].get<TThostFtdcPositionDateTypeType>();
                pInstrument->LongMarginRatio = instrumentPair.value()["LongMarginRatio"].get<TThostFtdcRatioType>();
                pInstrument->ShortMarginRatio = instrumentPair.value()["ShortMarginRatio"].get<TThostFtdcRatioType>();
                pInstrument->MaxMarginSideAlgorithm = instrumentPair.value()["MaxMarginSideAlgorithm"].get<TThostFtdcMaxMarginSideAlgorithmType>();
                pInstrument->StrikePrice = instrumentPair.value()["StrikePrice"].get<TThostFtdcPriceType>();
                pInstrument->OptionsType = instrumentPair.value()["OptionsType"].get<TThostFtdcOptionsTypeType>();
                pInstrument->UnderlyingMultiple = instrumentPair.value()["UnderlyingMultiple"].get<TThostFtdcUnderlyingMultipleType>();
                pInstrument->CombinationType = instrumentPair.value()["CombinationType"].get<TThostFtdcCombinationTypeType>();
                strncpy(pInstrument->InstrumentID, instrumentPair.value()["InstrumentID"].get<string>().c_str(), sizeof(pInstrument->InstrumentID) - 1);
                strncpy(pInstrument->ExchangeInstID, instrumentPair.value()["ExchangeInstID"].get<string>().c_str(), sizeof(pInstrument->ExchangeInstID) - 1);
                strncpy(pInstrument->ProductID, instrumentPair.value()["ProductID"].get<string>().c_str(), sizeof(pInstrument->ProductID) - 1);
                strncpy(pInstrument->UnderlyingInstrID, instrumentPair.value()["UnderlyingInstrID"].get<string>().c_str(), sizeof(pInstrument->UnderlyingInstrID) - 1);
            }
            catch (const std::exception &e)
            {
                LogError("Exception to load instrument info of {}, {}. {}", exchangePair.key(), instrumentPair.key(), e.what());
                return -1;
            }
            instrumentsByExchange_[pInstrument->ExchangeID][pInstrument->InstrumentID] = pInstrument;
            LogInfo("Loaded instrument: {} {} {}", pInstrument->ExchangeID, pInstrument->InstrumentID, pInstrument->InstrumentName);
        }
    }
    inFile.close();
    LogInfo("Loaded {} instruments from file: {}", instrumentsByExchange_.size(), instrumentsFile_);
    return 0;
}

int CInstrumentsLoader::Save()
{
    std::ofstream outFile(instrumentsFile_, std::ios::out | std::ios::trunc);
    if (!outFile.is_open())
    {
        LogError("Failed to open instruments file for writing: {}", instrumentsFile_);
        return -1;
    }
    Json instrumentsJson;
    for (const auto &exchangePair : instrumentsByExchange_)
    {
        instrumentsJson[exchangePair.first] = Json::object();
        for (const auto &instrumentPair : exchangePair.second)
        {
            instrumentsJson[exchangePair.first][instrumentPair.first] = Json::object();
            auto pInstrument = instrumentPair.second;
            instrumentsJson[exchangePair.first][instrumentPair.first]["ExchangeID"] = pInstrument->ExchangeID;
            instrumentsJson[exchangePair.first][instrumentPair.first]["InstrumentName"] = pInstrument->InstrumentName;
            instrumentsJson[exchangePair.first][instrumentPair.first]["ProductClass"] = pInstrument->ProductClass;
            instrumentsJson[exchangePair.first][instrumentPair.first]["DeliveryYear"] = pInstrument->DeliveryYear;
            instrumentsJson[exchangePair.first][instrumentPair.first]["DeliveryMonth"] = pInstrument->DeliveryMonth;
            instrumentsJson[exchangePair.first][instrumentPair.first]["MaxMarketOrderVolume"] = pInstrument->MaxMarketOrderVolume;
            instrumentsJson[exchangePair.first][instrumentPair.first]["MinMarketOrderVolume"] = pInstrument->MinMarketOrderVolume;
            instrumentsJson[exchangePair.first][instrumentPair.first]["MaxLimitOrderVolume"] = pInstrument->MaxLimitOrderVolume;
            instrumentsJson[exchangePair.first][instrumentPair.first]["MinLimitOrderVolume"] = pInstrument->MinLimitOrderVolume;
            instrumentsJson[exchangePair.first][instrumentPair.first]["VolumeMultiple"] = pInstrument->VolumeMultiple;
            instrumentsJson[exchangePair.first][instrumentPair.first]["PriceTick"] = pInstrument->PriceTick;
            instrumentsJson[exchangePair.first][instrumentPair.first]["CreateDate"] = pInstrument->CreateDate;
            instrumentsJson[exchangePair.first][instrumentPair.first]["OpenDate"] = pInstrument->OpenDate;
            instrumentsJson[exchangePair.first][instrumentPair.first]["ExpireDate"] = pInstrument->ExpireDate;
            instrumentsJson[exchangePair.first][instrumentPair.first]["StartDelivDate"] = pInstrument->StartDelivDate;
            instrumentsJson[exchangePair.first][instrumentPair.first]["EndDelivDate"] = pInstrument->EndDelivDate;
            instrumentsJson[exchangePair.first][instrumentPair.first]["InstLifePhase"] = pInstrument->InstLifePhase;
            instrumentsJson[exchangePair.first][instrumentPair.first]["IsTrading"] = pInstrument->IsTrading;
            instrumentsJson[exchangePair.first][instrumentPair.first]["PositionType"] = pInstrument->PositionType;
            instrumentsJson[exchangePair.first][instrumentPair.first]["PositionDateType"] = pInstrument->PositionDateType;
            instrumentsJson[exchangePair.first][instrumentPair.first]["LongMarginRatio"] = pInstrument->LongMarginRatio;
            instrumentsJson[exchangePair.first][instrumentPair.first]["ShortMarginRatio"] = pInstrument->ShortMarginRatio;
            instrumentsJson[exchangePair.first][instrumentPair.first]["MaxMarginSideAlgorithm"] = pInstrument->MaxMarginSideAlgorithm;
            instrumentsJson[exchangePair.first][instrumentPair.first]["StrikePrice"] = pInstrument->StrikePrice;
            instrumentsJson[exchangePair.first][instrumentPair.first]["OptionsType"] = pInstrument->OptionsType;
            instrumentsJson[exchangePair.first][instrumentPair.first]["UnderlyingMultiple"] = pInstrument->UnderlyingMultiple;
            instrumentsJson[exchangePair.first][instrumentPair.first]["CombinationType"] = pInstrument->CombinationType;
            instrumentsJson[exchangePair.first][instrumentPair.first]["InstrumentID"] = pInstrument->InstrumentID;
            instrumentsJson[exchangePair.first][instrumentPair.first]["ExchangeInstID"] = pInstrument->ExchangeInstID;
            instrumentsJson[exchangePair.first][instrumentPair.first]["ProductID"] = pInstrument->ProductID;
            instrumentsJson[exchangePair.first][instrumentPair.first]["UnderlyingInstrID"] = pInstrument->UnderlyingInstrID;
        }
    }
    outFile << instrumentsJson.dump(4) << std::endl;
    outFile.close();
    LogInfo("Instruments saved to file: {}", instrumentsFile_);
    return 0;
}

int CInstrumentsLoader::AddInstrument(CThostFtdcInstrumentField *pInstrument)
{
    if (!pInstrument)
    {
        LogError("Invalid instrument pointer: null");
        return -1;
    }
    instrumentsByExchange_[pInstrument->ExchangeID][pInstrument->InstrumentID] = std::make_shared<CThostFtdcInstrumentField>(*pInstrument);
    LogInfo("Adding instrument: {}, {}", pInstrument->ExchangeID, pInstrument->InstrumentID);
    return 0;
}

std::shared_ptr<CThostFtdcInstrumentField> CInstrumentsLoader::GetInstrument(const string &exchangeID, const string &instrumentID) const
{
    auto it = instrumentsByExchange_.find(exchangeID);
    if (it == instrumentsByExchange_.end())
    {
        LogError("Exchange not found: {}", exchangeID);
        return nullptr;
    }
    auto instrumentIt = it->second.find(instrumentID);
    if (instrumentIt == it->second.end())
    {
        LogError("Instrument not found: {} {}", exchangeID, instrumentID);
        return nullptr;
    }
    LogInfo("Found instrument: {} {}", exchangeID, instrumentID);
    return instrumentIt->second;
}
