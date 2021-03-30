#pragma once
#include <string>
#include "CONSTANTS.h"

using namespace std;

class Node{
    public:
        int coreUsed;
        int coreRem;            //还可以用的核心数
        int memoryUsed;
        int memoryRem;          //还可以用的内存大小

        string tostring(){
            return "Node: {coreUsed: " + to_string(coreUsed)
                + ", coreRem: " + to_string(coreRem)
                + ", memoryUsed: " + to_string(memoryUsed)
                + ", memoryRem: " + to_string(memoryRem)
                + "}";
        }
};

class Server{
    public:
        string type;
        int core;
        int memory;
        int deviceCost;
        int dailyCost;
        int id;			            //本地服务器编号
        Node nodeA, nodeB;          //节点A，B

        Server(string type,  int id, int core, int memory, int deviceCost, int dailyCost){
            this->id = id;
            this->core = core;
            this->memory = memory;
            this->type = type;
            this->deviceCost = deviceCost;
            this->dailyCost = dailyCost;

            nodeA.coreUsed = 0;
            nodeA.memoryUsed = 0;
            nodeB.coreUsed = 0;
            nodeB.memoryUsed = 0;

            nodeA.coreRem = core / 2;
            nodeA.memoryRem = memory / 2;
            nodeB.coreRem = core / 2;
            nodeB.memoryRem = memory / 2;
        }

        Server(){}
        
        int getId(){
            return this->id;
        }
        string getType(){
            return this->type;
        }
        int getCore(){
            return this->core;
        }

        int getMemory(){
            return this->memory;
        }

        int getDeviceCost(){
            return this->deviceCost;
        }

        int getDailyCost(){
            return this->dailyCost;
        }

        Node getNode(int which){
            Node n;
            switch(which){
                case A:
                    n = nodeA;
                    break;
                case B:
                    n = nodeB;
                    break;
            }
            return n;
        }

        string tostring(){
            return "Server{type: " + type
                + ", id: " + to_string(id)
                + ", core: " + to_string(core)
                + ", memory: " + to_string(memory)
                + ", deviceCost: " + to_string(deviceCost)
                + ", dailyCost: " + to_string(dailyCost)
                + ", A: " + nodeA.tostring() + ", B: " + nodeB.tostring()
                + "}";
        }
};
