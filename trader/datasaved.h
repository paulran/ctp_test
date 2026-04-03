#pragma once

#include "common.h"
#include <fstream>

class CDataSaved
{
public:
    CDataSaved();
    ~CDataSaved();

    void save();
    void load();

    void updateOrderRef(int orderRef);
    int getOrderRef() const;

    void updateOrderActionRef(int orderActionRef);
    int getOrderActionRef() const;

private:
    std::string filePath_;
    std::ofstream ofs_;
    Json data_;
};
