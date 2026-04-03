#include "datasaved.h"
#include <ctime>

CDataSaved::CDataSaved()
{
    // 根据当前日期生成文件路径，例如 "data_20240630.json"
    std::time_t now_c = std::time(nullptr);
    std::tm *now_tm = std::localtime(&now_c);
    char date[11];
    std::strftime(date, sizeof(date), "%Y%m%d", now_tm);
    filePath_ = std::string("data_") + std::string(date) + std::string(".json");
}

CDataSaved::~CDataSaved()
{
    if (ofs_.is_open())
    {
        ofs_.close();
    }
}

void CDataSaved::save()
{
    if (ofs_.is_open())
    {
        // 覆盖写入文件内容：先清空文件，再写入新的数据
        ofs_.seekp(0);         // 将写入位置移动到文件开头
        ofs_ << data_.dump(4); // 以漂亮的格式写入JSON数据
        ofs_.flush();          // 确保数据写入文件
    }
    else
    {
        // 如果文件没有打开，尝试打开并写入数据
        ofs_.open(filePath_, std::ofstream::out | std::ofstream::trunc);
        if (ofs_.is_open())
        {
            ofs_ << data_.dump(4); // 以漂亮的格式写入JSON数据
            ofs_.flush();          // 确保数据写入文件
        }
    }
}

void CDataSaved::load()
{
    std::ifstream ifs(filePath_);
    if (ifs.is_open())
    {
        ifs >> data_;
        ifs.close();
    }
    else
    {
        // 文件不存在，初始化一个空的JSON对象
        data_ = Json::object();
    }
    // 初始化ofs_以便后续保存时使用
    ofs_.open(filePath_, std::ofstream::out | std::ofstream::trunc);
    ofs_ << data_.dump(4); // 以漂亮的格式写入JSON数据
    ofs_.flush();          // 确保数据写入文件
}

void CDataSaved::updateOrderRef(int orderRef)
{
    data_["orderRef"] = orderRef;
}

int CDataSaved::getOrderRef() const
{
    if (data_.contains("orderRef"))
    {
        return data_["orderRef"].get<int>();
    }
    return 0; // 默认值
}

void CDataSaved::updateOrderActionRef(int orderActionRef)
{
    data_["orderActionRef"] = orderActionRef;
}

int CDataSaved::getOrderActionRef() const
{
    if (data_.contains("orderActionRef"))
    {
        return data_["orderActionRef"].get<int>();
    }
    return 0; // 默认值
}
