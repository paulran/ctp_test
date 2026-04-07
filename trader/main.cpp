#include "common.h"
#include "logger.h"
#include "instrumentsloader.h"
#include "traderspi.h"
#include "timerthread.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <getopt.h>

void help_info(char *argv[])
{
    printf("Usage: %s --config=<configfile> or -c <configfile>\n", argv[0]);
}

int main(int argc, char *argv[])
{
    int opt, index;
    const struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"config", required_argument, 0, 'c'},
        {0, 0, 0, 0}};

    string configfile = "";
    while ((opt = getopt_long(argc, argv, "hc:", long_options, &index)) != -1)
    {
        switch (opt)
        {
        case 'h':
            help_info(argv);
            return 0;
        case 'c':
            configfile = optarg;
            break;
        }
    }
    if (configfile == "")
    {
        help_info(argv);
        return 0;
    }

    Json config;
    try
    {
        // Read config file and parse it.
        std::ifstream fconfig(configfile);
        if (fconfig.fail())
        {
            std::cerr << "Failed to open config file: " << configfile << std::endl;
            return 1;
        }
        config = Json::parse(fconfig);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to parse config file: " << configfile << std::endl;
        std::cerr << e.what() << '\n';
        return 1;
    }

    LogInit(config["log"]["level"].get<string>());
    LogInfo("Start {} ... ", argv[0]);

    auto frontAddress = config["trader"]["frontAddress"].get<string>();
    auto brokerID = config["trader"]["brokerID"].get<string>();
    auto userID = config["trader"]["userID"].get<string>();
    auto userProductInfo = config["trader"]["userProductInfo"].get<string>();
    auto authCode = config["trader"]["authCode"].get<string>();
    auto appID = config["trader"]["appID"].get<string>();
    auto password = config["trader"]["password"].get<string>();
    auto investorID = config["trader"]["investorID"].get<string>();
    auto bIsProductionMode = config["trader"]["bIsProductionMode"].get<bool>();
    auto maxInsertOrderCountPerSecond = config["trader"]["maxInsertOrderCountPerSecond"].get<int>();
    auto maxCancelOrderCountPerSecond = config["trader"]["maxCancelOrderCountPerSecond"].get<int>();
    LogInfo("Configure frontAddress: {}, brokerID: {}, userID: {}, userProductInfo: {}, appID: {}, investorID: {}, "
            "bIsProductionMode: {}, maxInsertOrderCountPerSecond: {}, maxCancelOrderCountPerSecond: {}",
            frontAddress, brokerID, userID, userProductInfo, appID, investorID,
            bIsProductionMode, maxInsertOrderCountPerSecond, maxCancelOrderCountPerSecond);

    if (CInstrumentsLoader::Instance().Load(config["trader"]["instrumentsFile"].get<string>()))
    {
        LogError("Fail to load instruments info from file: {}", config["trader"]["instrumentsFile"].get<string>());
        return 2;
    }

    CThostFtdcTraderApi *api = CThostFtdcTraderApi::CreateFtdcTraderApi("", bIsProductionMode);
    auto apiVersion = api->GetApiVersion();
    LogInfo("CTP Trader API Version: {}", apiVersion);
    // sleep for a while to let the API thread start and print its version info
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CTraderSpi traderSpi(api, frontAddress, brokerID, userID, userProductInfo, authCode, appID, password, investorID,
                         maxInsertOrderCountPerSecond, maxCancelOrderCountPerSecond);
    traderSpi.Init();
    api->RegisterSpi(&traderSpi);
    char pszTdFrontAddress[64];
    strncpy(pszTdFrontAddress, frontAddress.c_str(), 63);
    api->RegisterFront(pszTdFrontAddress);
    api->SubscribePrivateTopic(THOST_TERT_RESUME); // 从上次收到的续传
    api->SubscribePublicTopic(THOST_TERT_QUICK); // 只传送登录后公共流的内容
    api->Init();
    // 启动timerthread
    auto exchangeID = config["trader"]["exchangeID"].get<string>();
    auto instrumentID = config["trader"]["instrumentID"].get<string>();
    auto volume = config["trader"]["volume"].get<int>();
    LogInfo("Configure exchangeID: {}, instrumentID: {}, volume: {}", exchangeID, instrumentID, volume);
    CTimerThread timerThread(api, &traderSpi, exchangeID, instrumentID, volume);
    timerThread.Start();
    int ret = api->Join();
    LogWarn("Api thread exited with code: {}, main thread will exit too.", ret);
    timerThread.Stop();
    timerThread.Join();
    return 0;
}
