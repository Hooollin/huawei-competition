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
#include "definition/ServerModel.h"
#include "definition/VirtualMachineModel.h"
#include "definition/VirtualMachine.h"
#include "definition/Server.h"
#include "definition/CONSTANTS.h"
#include "definition/OP.h"

using namespace std;


//***************************FUNCTION DEFINITION**********************************



// work
void solve(int day, int T);

// initialization
void initializeOperationVector();


// migrate
void migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode);

void optimized_migrate();


// purchase
pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T);


// output
int getNextServerId();

int getNextGlobalServerId();

string makeDeploymentOutput(int serverId, int node);

void doOutput();


// deployment
pair<int,int> selectServer(VirtualMachineModel vmd);

double selectServerFun(Server &currServer, VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem);

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice);

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode);

void allocateServer(OP addop, int today, int T);

void releaseRes(OP delop);

//input
void readServerModel();

void readVirtualMachineModel();

void readOperation();


// helper
void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel);

double similarity(VirtualMachineModel &vmd, Server &server);

double similarity(VirtualMachineModel &vmd, ServerModel &server);

int canPut(Server server, VirtualMachineModel vmd);

int canPut(Server server, VirtualMachine vm);

bool canBuy(ServerModel sm, int neededCore, int neededMem);

bool compareNode(Node nodeA, Node nodeB, VirtualMachine vm);


// debug
bool checkServer(Server &server);

bool checkUsage(Server &server);

//***************************END FUNCTION DEFINITION**********************************



// ******************************ALGO*********************************
unordered_map<string, ServerModel> mTypeToServerModel; // 所有服务器型号； key: type -> value: 服务器型号
unordered_map<string, VirtualMachineModel> mTypeToVirtualMachineModel; // 所有虚拟机型号； key: type -> value:

vector<ServerModel> vServerModel;
vector<VirtualMachineModel> vVirtualMachineModel;

unordered_map<int, int> mLocalServerIdGlobalServerId; //serverId到输出id的映射

unordered_map<int, pair<int, int>> mVirtualMachineInServer; // 虚拟机编号到服务器编号以及节点的映射
unordered_map<int, VirtualMachine> mVmidToVirtualMachine; //虚拟机编号到虚拟机到映射
unordered_map<int, set<pair<int, int>>> mServerHasVirtualMachine;

vector<Server> vAllServer;                    //所有已经创建的服务器
unordered_map<int, Server> mServerIdToServer; //服务器编号到服务器的映射
unordered_map<int, int> mServerIdVectorPos;    //服务器编号到vector下标的映射

//输出数据
vector<string> vMigration;
vector<pair<int, int>> vDeployment;
vector<int> vPurchasedServer;

vector<OP> vOperation;

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

// purchase时获取服务器编号
int getNextServerId(){
    return localServerNum++;
}

int getNextGlobalServerId(){
    return globalServerNumber++;
}

void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel){
    int sign = isDel ? 1 : -1;
    int corediff = sign * vmcore;
    int memdiff = sign * vmmemory;
    switch(node){
        case A:
            server.nodeA.coreRem += corediff;
            server.nodeA.coreUsed -= corediff;
            server.nodeA.memoryRem += memdiff;
            server.nodeA.memoryUsed -= memdiff;
            break;
        case B:
            server.nodeB.coreRem += corediff;
            server.nodeB.coreUsed -= corediff;
            server.nodeB.memoryRem += memdiff;
            server.nodeB.memoryUsed -= memdiff;
            break;
        case BOTH:
            corediff /= 2;
            memdiff /= 2;

            server.nodeA.coreRem += corediff;
            server.nodeA.coreUsed -= corediff;
            server.nodeA.memoryRem += memdiff;
            server.nodeA.memoryUsed -= memdiff;

            server.nodeB.coreRem += corediff;
            server.nodeB.coreUsed -= corediff;
            server.nodeB.memoryRem += memdiff;
            server.nodeB.memoryUsed -= memdiff;
            break;
    }
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
    cout << "(purchase, " << to_string(cnt.size()) << ")" << endl;
    for(auto &p : cnt){
        for(int serverid : p.second){
            mLocalServerIdGlobalServerId[serverid] = getNextGlobalServerId();
        }
        string curr = "(" + p.first + ", " + to_string(p.second.size()) + ")";
#ifndef DEBUG
        cout << curr << endl;
#endif
    }
    //migration
    cout << "(" << "migration, " << vMigration.size() << ")" << endl;
    for(auto &s : vMigration){
#ifndef DEBUG
        cout << s << endl;
#endif
    }

    //输出deployment
    for(auto &p : vDeployment){
#ifndef DEBUG
        cout << makeDeploymentOutput(p.first, p.second) << endl;
#endif
    }
}

double similarity(VirtualMachineModel &vmd, Server &server){
    return abs((double)vmd.core / vmd.memory - (double)server.getCore() / server.getMemory());
}

double similarity(VirtualMachineModel &vmd, ServerModel &server){
    return abs((double)vmd.core / vmd.memory - (double)server.core / server.memory);
}

// 判断虚拟机是否可以放在服务器中
int canPut(Server server, VirtualMachineModel vmd){
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        int choice = NONE;
        // 可以放在A节点
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // 可以放在B节点
        if(server.nodeB.coreRem >= neededCore && server.nodeB.memoryRem >= neededMem){
            choice = (choice == NONE) ? B : BOTH;
        }
        return choice;
    }else{
        neededCore /= 2; neededMem /= 2;
        if(server.nodeA.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeB.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeA.memoryRem < neededMem){
            return NONE;
        }
        if(server.nodeB.memoryRem < neededMem){
            return NONE;
        }
        return BOTH;
    }
}

int canPut(Server server, VirtualMachine vm){
    int neededCore = vm.getCore(), neededMem = vm.getMemory();
    if(vm.getSingle()){
        int choice = NONE;
        // 可以放在A节点
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // 可以放在B节点
        if(server.nodeB.coreRem >= neededCore && server.nodeB.memoryRem >= neededMem){
            choice = (choice == NONE) ? B : BOTH;
        }
        return choice;
    }else{
        neededCore /= 2; neededMem /= 2;
        if(server.nodeA.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeB.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeA.memoryRem < neededMem){
            return NONE;
        }
        if(server.nodeB.memoryRem < neededMem){
            return NONE;
        }
        return BOTH;
    }
}

//计算选择值
double selectServerFun(Server &currServer, VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem, leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() >> 1;
    int totalMem = currServer.getMemory() >> 1;
    //计算碎片参数
    double chipF;
    double total = currServer.getCore() + currServer.getMemory();
    double Sim = 0.0, Near = 0.0;

    if(occupyACore == 0){
        chipF = ((1.0 * totalCore - leftBCore) / currServer.getCore() + (1.0 * totalMem - leftBMem) / currServer.getMemory()) / (total / 2);
        Near = 1 - (abs(currServer.getCore() - occupyBCore) + abs(currServer.getMemory() - occupyBMem)) / MAXSOURCE;
        Sim = 1.0 - abs(1.0 * occupyBCore / occupyBMem - 1.0 * currServer.getCore() / currServer.getMemory()) / (MAX_SIMI - MIN_SIMI);
    } else if(occupyBCore == 0){
        chipF = ((1.0 * totalCore - leftACore) / currServer.getCore() + (1.0 * totalMem - leftAMem) / currServer.getMemory()) / (total / 2);
        Near = 1 - (abs(currServer.getCore() - occupyACore) + abs(currServer.getMemory() - occupyAMem)) / MAXSOURCE;
        Sim = 1. - abs(1.0 * occupyACore / occupyAMem - 1.0 * currServer.getCore() / currServer.getMemory()) / (MAX_SIMI - MIN_SIMI);
    } else{
        chipF = (1.0 * total- leftACore - leftAMem - leftBCore - leftBMem) / total;
        Near = 1 - (abs((currServer.getCore()>>1) - occupyBCore) + abs((currServer.getMemory()>>1) - occupyBMem)) / MAXSOURCE;
        Sim = 1.0 - abs(1.0 * occupyBCore / occupyBMem - 1.0 * currServer.getCore() / currServer.getMemory()) / (MAX_SIMI - MIN_SIMI);
    }

    //计算平衡参数
    double balanceF = (abs(1 - abs(1.0 * leftACore / totalCore - 1.0 * leftAMem / totalMem)) + abs(1 - abs(1.0 * leftBCore / totalCore - 1.0 * leftBMem / totalMem))) / 2;
    double balanceNode = 1 - ((double)abs(leftACore - leftBCore) / totalCore + (double)abs(leftAMem - leftBMem) / totalMem) / total;

    //返回加权值
    return put_SelectWeight * chipF + put_SimWeight * Sim + put_NearWeight * Near + balanceF * put_BalanceWeight + put_NodeBlanceWeight * balanceNode;
}

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice){
    double res = 0;
    int choseNode = 0;
    double cal;
    if(vmd.single){
        if(choice == A || choice == BOTH){
            res = selectServerFun(currServer, vmd, vmd.core,vmd.memory,0,0);
            choseNode = A;
        }
        if(choice == B || choice == BOTH){
            cal = selectServerFun(currServer, vmd, 0,0,vmd.core,vmd.memory);
            if(cal > res){
                res = cal;
                choseNode = B;
            }
        }
    }else{
        res = selectServerFun(currServer,vmd, vmd.core >> 1,vmd.memory >> 1,vmd.core >> 1,vmd.memory >> 1);
        choseNode = BOTH;
    }
    return make_pair(res,choseNode);
}

pair<int,int> selectServer(VirtualMachineModel vmd){
    double maxn = 0; //函数获得的最大值
    pair<double,int> res;//返回值
    int finalChoice;
    int targetServerIdx = -1;
    for (int i = 0; i < vAllServer.size(); i++) {
      Server currServer = vAllServer[i];
      int choice = canPut(currServer, vmd);
      if (choice > 0) {
        res = selectServerCal(currServer, vmd, choice);
        if (res.first > maxn)
          maxn = res.first, targetServerIdx = i, finalChoice = res.second;
      }
    }
    return targetServerIdx == -1
               ? make_pair(-1, -1)
               : make_pair(vAllServer[targetServerIdx].id, finalChoice);
}

bool canBuy(ServerModel sm, int neededCore, int neededMem){
    return sm.core >= neededCore && sm.memory >= neededMem;
}

pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();
    //当前虚拟机需要的core和内存大小
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        neededCore *= 2;
        neededMem *= 2;
    }
    double maxP = -1,minP = 0x3f3f3f3f;
    double maxC = -1,minC = 0x3f3f3f3f;
    double maxSm = -1, minSm = 0x3f3f3f3f;
    double maxNear = -1, minNear = 0x3f3f3f3f;
    int leftCore,leftMem,devicePrice,dayPrice,totalCore,totalMem,leftNodeCore,leftNodeMem,totalNodeCore,totalNodeMem;

    vector<double> PriceF(vServerModel.size(), -1);
    vector<double> PriceC(vServerModel.size(), -1);
    vector<double> Sm(vServerModel.size(), -1);
    vector<double> Near(vServerModel.size(), -1);

    for (int i = 0; i < vServerModel.size(); i++) {
      ServerModel p = vServerModel[i];
      if (canBuy(p, neededCore, neededMem)) {
        devicePrice = p.deviceCost;
        dayPrice = p.dailyCost * (T - today);
        totalCore = p.core;
        totalMem = p.memory;
        PriceF[i] = devicePrice + dayPrice;
        PriceC[i] = 1.0 * (devicePrice + dayPrice) / totalMem +
                    1.0 * (devicePrice + dayPrice) / totalCore;
        Near[i]   = abs(1. * totalCore - neededCore) + abs(1. * totalMem - neededMem);
        Sm[i]     = abs((1. * MEAN_VM_CORE / MEAN_VM_MEMORY) -
                    (1. * totalCore / totalMem));

        maxP = max(maxP, PriceF[i]), minP = min(minP, PriceF[i]);
        maxC = max(maxC, PriceC[i]), minC = min(minC, PriceC[i]);
        maxSm = max(maxSm, Sm[i]), minSm = min(minSm, Sm[i]);
        maxNear = max(maxNear, Near[i]), minNear = min(minNear, Near[i]);
      }
    }

    int k = -1;
    double balanceF,spaceF,newPriceF,choseF, newPriceC, newSm, newNear;
    double maxn = -1;
    for (int i = 0; i < vServerModel.size(); i++) {
      ServerModel p = vServerModel[i];
      if (canBuy(p, neededCore, neededMem)) {
        leftCore = p.core - vmd.core;
        leftMem = p.memory - vmd.memory;
        totalCore = p.core;
        totalMem = p.memory;
        totalNodeCore = totalCore >> 1;
        totalNodeMem = totalMem >> 1;
        if (vmd.single) {
          leftNodeCore = (totalCore >> 1) - vmd.core;
          leftNodeMem = (totalMem >> 1) - vmd.memory;
        } else {
          leftNodeCore = (totalCore >> 1) - (vmd.core >> 1);
          leftNodeMem = (totalMem >> 1) - (vmd.memory >> 1);
        }

        balanceF = 1 - abs((1.0 * leftNodeCore / totalNodeCore) - (1.0 * leftNodeMem / totalNodeMem));

        spaceF = (1.0 * leftCore + leftMem) / MAXSOURCE;
        newPriceF = 1 - (PriceF[i] - minP) / (maxP - minP);
        newPriceC = 1 - (PriceC[i] - minC) / (maxC - minC);
        newSm = 1 - (Sm[i] - minSm) / (maxSm - minSm);
        newNear = 1 - (Near[i] - minNear) / (maxNear - minNear);
       
        choseF = buy_PriceWeight * newPriceF + buy_BalanceWeight * balanceF +
                 buy_leftSpaceWeight * spaceF +
                 buy_PriceWithCapacituWeight * newPriceC + newSm * buy_SmWeight;

        if (maxn < choseF)
          k = i, maxn = choseF;
      }
    }

#ifdef DEBUG
    assert(k != -1);
#endif

    ServerModel sm = vServerModel[k];
    Server purchasedServer(sm.type, newServerId, sm.core, sm.memory, sm.deviceCost, sm.dailyCost);
    vPurchasedServer.push_back(newServerId);

    vAllServer.push_back(purchasedServer);
    mServerIdVectorPos[newServerId] = vAllServer.size() - 1;
    mServerIdToServer[newServerId] = purchasedServer;

    if(vmd.single) return make_pair(newServerId,A);
    else return make_pair(newServerId,BOTH);
}

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode){
    //找到对应的server
    int serverId = serverAndNode.first, node = serverAndNode.second;
    Server server = mServerIdToServer[serverId];
#ifdef DEBUG
    assert(canPut(server, vmd));
#endif
    updateResource(server, node, vmd.core, vmd.memory, false);
    if (mServerHasVirtualMachine.find(serverId) ==
        mServerHasVirtualMachine.end()) {
      mServerHasVirtualMachine[serverId] = set<pair<int, int>>();
    }
#ifdef DEBUG
    assert(vmid != 0);
#endif
    
    //更新数据结构
    mVirtualMachineInServer[vmid] = {serverId, node};
    mServerHasVirtualMachine[serverId].insert(
        make_pair(vmd.core + vmd.memory, vmid));
    mServerIdToServer[serverId] = server;
    vAllServer[mServerIdVectorPos[serverId]] = server;

    //输出
    vDeployment.push_back({serverId, node});
}


void migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode){
  Server fromServer = mServerIdToServer[mVirtualMachineInServer[vmid].first];
  int fromnode = mVirtualMachineInServer[vmid].second;

  Server toServer = mServerIdToServer[serverAndNode.first];
  int tonode = serverAndNode.second;

  updateResource(fromServer, fromnode, mVmidToVirtualMachine[vmid].getCore(),
                 mVmidToVirtualMachine[vmid].getMemory(), true);

  updateResource(toServer, tonode, mVmidToVirtualMachine[vmid].getCore(),
                 mVmidToVirtualMachine[vmid].getMemory(), false);

  mServerIdToServer[fromServer.id] = fromServer;
  vAllServer[mServerIdVectorPos[fromServer.id]] = fromServer;

  mServerIdToServer[toServer.id] = toServer;
  vAllServer[mServerIdVectorPos[toServer.id]] = toServer;

  mVirtualMachineInServer[vmid] = serverAndNode;

  mServerHasVirtualMachine[toServer.id].insert(
      {mVmidToVirtualMachine[vmid].getCore() +
           mVmidToVirtualMachine[vmid].getMemory(),
       vmid});
}


void allocateServer(OP addop, int today, int T){
    int vmid = addop.id;
    VM_AMOUNT += 1;
    // 需要新增虚拟机的型号
    VirtualMachineModel vmd = mTypeToVirtualMachineModel[addop.machineType];
    //新增的虚拟机实例
    pair<int,int> serverAndNode = selectServer(vmd);
    if(serverAndNode.first == -1){       // 没有合适的服务器，需要新购买服务器
        serverAndNode = makePurchase(vmd, today, T);
    }

    VirtualMachine vm(addop.machineType, vmid, vmd.core, vmd.memory, vmd.single);
    mVmidToVirtualMachine[vmid] = vm;

    putVirtualMachineToServer(vmd, vmid, serverAndNode);
}


void releaseRes(OP delop){
    VM_AMOUNT -= 1;
    int vmid = delop.id;
    VirtualMachine vm = mVmidToVirtualMachine[vmid];

    pair<int, int> t = mVirtualMachineInServer[vmid];
    int serverId = t.first, loc = t.second;
    Server server = mServerIdToServer[serverId];

    updateResource(server, loc, vm.getCore(), vm.getMemory(), true);
    vAllServer[mServerIdVectorPos[serverId]] = server;
    mServerIdToServer[serverId] = server;
    
    //更新数据数据结构
    mVmidToVirtualMachine.erase(vmid);
    mVirtualMachineInServer.erase(vmid);

    mServerHasVirtualMachine[serverId].erase(
        {vm.getCore() + vm.getMemory(), vmid});
}

bool compareNode(Node nodeA, Node nodeB, VirtualMachine vm){
    //只能放在A、B中的一个
    // A节点剩下的资源不够
    if(vm.getCore() > nodeA.coreRem || vm.getMemory() > nodeA.memoryRem){
        return false;
    }
    // B节点剩下的资源不够
    if(vm.getCore() > nodeB.coreRem || vm.getMemory() > nodeB.memoryRem){
        return true;
    }

    //A, B节点都可以放时
    //如果A可以放并且放后core和mem都比B多
    if(nodeA.coreRem - vm.getCore() >= nodeB.coreRem && nodeA.memoryRem - vm.getMemory() >= nodeB.memoryRem){
        return true;
    }
    //如果B可以放并且放后core和mem都比A多
    if(nodeB.coreRem - vm.getCore() >= nodeA.coreRem && nodeB.memoryRem - vm.getMemory() >= nodeA.memoryRem){
        return false;
    }
    // 其他的情况取决于vmd所需的core和memory
    if(vm.getCore() > vm.getMemory()){
        return nodeA.coreRem > nodeB.coreRem;
    }
    return nodeA.memoryRem > nodeB.memoryRem;
}

void optimized_migrate(){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 5 * VM_AMOUNT / 1000;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // 存储所有server的chipF
    vector<double> fragments;
    // vServers的下标，用于排序
    vector<int> serverIdxs;
    int idx = 0;
    for (auto &server : vAllServer) {
      double fragment =
          (1.0 * MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem -
           server.nodeB.coreRem - server.nodeB.memoryRem) /
          MAXSOURCE;
      fragments.push_back(fragment);
      serverIdxs.push_back(idx++);
    }
    // 通过chipF排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return fragments[a] < fragments[b];
            });
    sort(fragments.begin(),fragments.end());
    int end = serverIdxs.size() - 1;
    while(end > 0 && abs(fragments[serverIdxs[end]] - 1.) < 1e-3){
        end -= 1;
    }
    for(int i = 0; i < end && totalOperation > 0; i++){
        // fromServer: 当前需要移除虚拟机的服务器
        int fromServerIdx = serverIdxs[i];
        // 需要移除虚拟机的服务器在vServers中的下标
        int fromServerId = vAllServer[fromServerIdx].id;

        // 存储循环中被移动到了新的服务器的虚拟机的vmid
        vector<pair<int, int>> modified;

        int last = end;
        for (auto p : mServerHasVirtualMachine[fromServerId]) {
          int vmid = p.second;
          VirtualMachine &vm = mVmidToVirtualMachine[vmid];
          double chipF =
              (1.0 * MAXSOURCE - vm.getCore() - vm.getMemory()) / MAXSOURCE;
          last = upper_bound(fragments.begin(), fragments.end(), chipF) -
                 fragments.begin();
          last--;
#ifdef DEBUG
          assert(mVmidToVirtualMachine.find(vmid) !=
                 mVmidToVirtualMachine.end());
#endif
             //从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
            while(last > i && totalOperation > 0){
                // toServer：被迁移虚拟机的去向
                int toServerIdx = serverIdxs[last];
                // toServer在vServers中的下标
                int toServerId = vAllServer[toServerIdx].id;

                // choice可以放在哪个结点
                int choice = canPut(vAllServer[toServerId], vm);
                if(choice > 0){
                    string output;
                    output += "(" + to_string(vmid) + ", " + to_string(mLocalServerIdGlobalServerId[toServerId]);
                    if(vm.getSingle()){
                        // 如果是单节点并且两个都能放，那么放在剩余资源多的结点上
                        if(choice == A){
                            output += ", A";
                        }else if(choice == B){
                            output += ", B";
                        } else if (compareNode(
                                       mServerIdToServer[toServerId].nodeA,
                                       mServerIdToServer[toServerId].nodeB,
                                       vm)) {
                          output += ", A";
                          choice = A;
                        } else {
                          output += ", B";
                          choice = B;
                        }
                    }
                    output += ")";
                    vMigration.push_back(output);
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // 在for loop中修改集合中的元素会导致bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // 可迁移次数减1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
            //break;
        }
        // 更新数据结构
        for(auto &p : modified){
          mServerHasVirtualMachine[fromServerId].erase(p);
        }

    }
}

void solve(int today, int T){
#ifdef DEBUG
  cout << today << " " << vOperation.size() << endl;
#endif
    // 顺序遍历每次操作
    optimized_migrate();
    //migrate();
    for (auto &op : vOperation) {
      switch (op.opType) {
      case ADD:
        allocateServer(op, today, T);
        break;
      case DEL:
        releaseRes(op);
        break;
      }
    }
    doOutput();
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
        cout << (float)(usedCore) / server.getCore() << " ";
        cout << (float)(usedMem) / server.getMemory() << " ";
        cout << endl;
        return false;
    }
    if((float)usedMem / server.getMemory() < minimumUseage){
        cout << (float)(usedCore) / server.getCore() << " ";
        cout << (float)(usedMem) / server.getMemory() << " ";
        cout << endl;
        return false;
    }
    return true;
}

// ***********************END ALGO*************************
//
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int N, M, T;
    cin >> N;
    cin.ignore(); //忽略回车
    for(int i = 0; i < N; i++){
      readServerModel();
    }
    for (int i = 0; i < vServerModel.size(); i++) {
      MAX_SERVERMODEL_CORE = max(MAX_SERVERMODEL_CORE, vServerModel[i].core);
      MAX_SERVERMODEL_MEMORY =
          max(MAX_SERVERMODEL_MEMORY, vServerModel[i].memory);
    }

    cin >> M;
    cin.ignore(); //忽略回车
    for(int i = 0; i < M; i++){
      readVirtualMachineModel();
    }
    long long vmcoresum = 0, vmmemsum = 0;
    for (int i = 0; i < vVirtualMachineModel.size(); i++) {
      vmcoresum += vVirtualMachineModel[i].core;
      vmmemsum += vVirtualMachineModel[i].memory;
      MAX_VM_CORE = max(MAX_VM_CORE, vVirtualMachineModel[i].core);
      MAX_VM_MEMORY = max(MAX_VM_CORE, vVirtualMachineModel[i].memory);
    }
    MEAN_VM_CORE = (double)vmcoresum / M;
    MEAN_VM_MEMORY = (double)vmmemsum / M;
    MAXSOURCE = MAX_SERVERMODEL_CORE + MAX_SERVERMODEL_MEMORY;
    for (auto &vmd : vVirtualMachineModel) {
      for (auto &sm : vServerModel) {
        MAX_SIMI = max(MAX_SIMI, similarity(vmd, sm));
        MIN_SIMI = min(MIN_SIMI, similarity(vmd, sm));
      }
    }
    //计算相似度
    cin >> T;

    for(int i = 1; i <= T; i++){
        int R;
        cin >> R;
        cin.ignore(); //忽略回车

        initializeOperationVector();
        while(R-- > 0){
          readOperation();
        }
        solve(i, T);
    }

#ifdef DEBUG
    cout << "N: " << vServerModel.size() << endl;
    cout << "M: " << vVirtualMachineModel.size() << endl;
    cout << "add count: " << addcount << endl;
    cout << "del count: " << delcount << endl;
    int unfilledSize = 0;
    int totalPrice = 0;
    for (auto &s : vAllServer) {
      totalPrice += s.getDeviceCost();
      if (!checkServer(s)) {
        cout << s.tostring() << endl;
        return 0;
      }
      if (!checkUsage(s)) {
        // cout << s.tostring() << endl;
        unfilledSize += 1;
      }
    }
    cout << "total price: " << totalPrice << " " << vAllServer.size() << " "
         << unfilledSize << endl;

#endif
    return 0;
}
