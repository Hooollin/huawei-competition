#pragma once


#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <queue>
#include <set>
#include <unordered_map>
#include "../definition/StorageStructure.h"

int addcount = 0;
int delcount = 0;
int vmcount = 0;
int dailycost = 0;


// 用来生成服务器编号
int localServerNum = 0;
int globalServerNumber = 0;

int MAX_VM_CORE = 0;
int MAX_VM_MEMORY = 0;
int MAX_SERVERMODEL_CORE = 0;
int MAX_SERVERMODEL_MEMORY = 0;
int MEAN_VM_CORE = 0;
int MEAN_VM_MEMORY = 0;
int VM_AMOUNT = 0;
double MAX_SIMI = -1, MIN_SIMI = 0x3f3f3f3f;
int MAX_DEVICE_COST = -1, MAX_DAILY_COST = -1;

int MAXSOURCE = 0;

// initialization
void initializeOperationVector();

//output
int getNextGlobalServerId();

string makeDeploymentOutput(int serverId, int node);

string makeMigrateOutput(int vmid,int serverId,int node);

void doOutput();

//input
void readServerModel();

void readVirtualMachineModel();

void readOperation();

//statiInformation
void  statiInformation();

// debug
bool checkServer(Server &server);

bool checkUsage(Server &server);
//**********************************DECLEAR END*****************************

void readServerModel(){
    ServerModel sm;

    stringstream ss;
    string line;
    getline(cin, line);
    ss << line;

    // 解析每行
    char lp, comma, rp;
    ss >> lp;
    string type;
    int core, memory, deviceCost, dailyCost;
    ss >> type;
    type.pop_back();
    sm.type = type;
    ss >> sm.core >> comma;
    ss >> sm.memory >> comma;
    ss >> sm.deviceCost >> comma;
    ss >> sm.dailyCost;
    ss >> rp;

#ifdef DEBUG
    //cout << sm.tostring() << endl;
#endif

    // 分别添加到map和vector
    mTypeToServerModel[sm.type] = sm;
    vServerModel.push_back(sm);
}

void readVirtualMachineModel(){
    VirtualMachineModel vmd;

    stringstream ss;
    string line;
    getline(cin, line);

    ss << line;
    char lp, comma, rp;

    ss >> lp;
    string type;
    ss >> type;
    type.pop_back();
    vmd.type = type;
    ss >> vmd.core >> comma;
    ss >> vmd.memory >> comma;
    ss >> vmd.single;
    vmd.single = !vmd.single;
    ss >> rp;

#ifdef DEBUG
    //cout << vmd.tostring() << endl;
#endif
    mTypeToVirtualMachineModel[type] = vmd;
    vVirtualMachineModel.push_back(vmd);
}

void readOperation(){
    stringstream ss;

    string line;
    getline(cin, line); ss << line;

    string type;
    char lp, comma, rp;

    ss >> lp;
    ss >> type;

    type.pop_back();
    OP op;
    if(type == "add"){
        op.opType = ADD;
        string vmtype;
        ss >> vmtype;
        vmtype.pop_back();
        op.machineType = vmtype;
        ss >> op.id;
#ifdef DEBUG
        addcount += 1;
#endif
    }else{
        op.opType = DEL;
        ss >> op.id;
#ifdef DEBUG
        delcount += 1;
#endif
    }

    ss >> rp;
    vOperation.push_back(op);
}

int getNextGlobalServerId(){
    return globalServerNumber++;
}

void initializeOperationVector(){
  vOperation.clear();
  vOperation.resize(0);
  vMigration.clear();
  vMigration.resize(0);
  vDeployment.clear();
  vDeployment.resize(0);
  vPurchasedServer.clear();
  vPurchasedServer.resize(0);
}

string makeDeploymentOutput(int serverId, int node){
    string output;
    if(node == A){
        output = "(" + to_string(mLocalServerIdGlobalServerId[serverId]) + ", A" + ")";
    }else if(node == B){
        output = "(" + to_string(mLocalServerIdGlobalServerId[serverId]) + ", B" + ")";
    }else{
        output = "(" + to_string(mLocalServerIdGlobalServerId[serverId]) + ")";
    }
    return output;
}

void doOutput(){
    map<string, vector<int>> cnt;
    for(int serverId : vPurchasedServer){
        string type = mServerIdToServer[serverId].getType();
        if (cnt.find(type) == cnt.end()) {
            cnt[type] = vector<int>();
        }
        cnt[type].push_back(serverId);
    }
#ifndef SEEK_PARAMETER
    cout << "(purchase, " << to_string(cnt.size()) << ")" << endl;
#endif
    for(auto &p : cnt){
        for(int serverid : p.second){
            mLocalServerIdGlobalServerId[serverid] = getNextGlobalServerId();
        }
        string curr = "(" + p.first + ", " + to_string(p.second.size()) + ")";
#ifndef DEBUG
#ifndef SEEK_PARAMETER
        cout << curr << endl;
#endif
#endif
    }
#ifndef SEEK_PARAMETER
    //migration
    cout << "(" << "migration, " << vMigration.size() << ")" << endl;
#endif
    for(auto &s : vMigration){
#ifdef DEBUG
        vmcount ++;
#endif // DEBUG
#ifndef DEBUG
#ifndef SEEK_PARAMETER
        cout << s << endl;
#endif
#endif
    }

    //输出deployment
    for(auto &p : vDeployment){
#ifndef DEBUG
        cout << makeDeploymentOutput(p.first, p.second) << endl;
#endif
    }
    fflush(stdout);
}

string makeMigrateOutput(int vmid,int serverId,int node){
    string res = "(" + to_string(vmid) + ", " + to_string(serverId);
    if(node == A || node == B){
        res += node == A ? ", A" : ", B";
    }
    res += ")";
    return res;
}

void  statiInformation(){
    int totalPrice = dailycost;
#ifndef SEEK_PARAMETER
    cout << "N: " << vServerModel.size() << endl;
    cout << "M: " << vVirtualMachineModel.size() << endl;
    cout << "add count: " << addcount << endl;
    cout << "del count: " << delcount << endl;
    cout << "vm count: " << vmcount << endl;
    int unfilledSize = 0;
    for (auto &s : vAllServer) {
      totalPrice += s.getDeviceCost();
      if (!checkServer(s)) {
        //cout << s.tostring() << endl;
        return ;
      }
      if (!checkUsage(s)) {
        cout << s.tostring() << endl;
        unfilledSize += 1;
      }
    }
    cout << "total price: " << totalPrice << " " << vAllServer.size() << " "
         << unfilledSize << endl;
#endif
#ifdef SEEK_PARAMETER
    for(auto &s : vAllServer){
        totalPrice += s.getDeviceCost();
    }
    cout << totalPrice;
#endif
}

//测试用，判断server的资源是否合法
bool checkServer(Server &server){
    if(server.nodeA.coreRem < 0 || server.nodeB.coreRem < 0){
        return false;
    }
    if(server.nodeA.memoryRem < 0 || server.nodeB.memoryRem < 0){
        return false;
    }
    if(server.nodeA.coreUsed + server.nodeB.coreUsed + server.nodeA.coreRem + server.nodeB.coreRem != server.getCore()){
        return false;
    }
    if(server.nodeA.memoryUsed + server.nodeA.memoryRem + server.nodeB.memoryRem + server.nodeB.memoryUsed != server.getMemory()){
        return false;
    }
    return true;
}

// 检查服务器是否到了使用度
bool checkUsage(Server &server){
    float minimumUseage = .8;
    int usedCore = server.nodeA.coreUsed + server.nodeB.coreUsed;
    int usedMem = server.nodeA.memoryUsed + server.nodeB.memoryUsed;
    if((float)usedCore / server.getCore() < minimumUseage){
#ifdef CHECKUSAGE
        cout << (float)(usedCore) / server.getCore() << " ";
        cout << (float)(usedMem) / server.getMemory() << " ";
        cout << endl;
#endif // CHECKUSAGE
        return false;
    }
    if((float)usedMem / server.getMemory() < minimumUseage){
#ifdef CHECKUSAGE
        cout << (float)(usedCore) / server.getCore() << " ";
        cout << (float)(usedMem) / server.getMemory() << " ";
        cout << endl;
#endif // CHECKUSAGE
        return false;
    }
    return true;
}
