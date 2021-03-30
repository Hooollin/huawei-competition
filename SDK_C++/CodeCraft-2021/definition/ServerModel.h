#pragma once
#include <string>

using namespace std;

class ServerModel{
    public:
        string type;    // 型号
        int core;		// CPU数量
        int memory;		// 内存大小

        int deviceCost; // 硬件成本
        int dailyCost;  // 日常成本

        ServerModel(){}

        ServerModel(string type, int core, int memory, int deviceCost, int dailyCost){
            this->type = type;
            this->core = core;
            this->memory = memory;
            this->deviceCost = deviceCost;
            this->dailyCost = dailyCost;
        }

        string tostring(){
            return "ServerModel: {type: " + this->type
                + ", core: " + to_string(this->core)
                + ", memory: " + to_string(this->memory)
                + ", deviceCost: " + to_string(this->deviceCost)
                + ", dailyCost: " + to_string(this->dailyCost) + "}";
        }

};
